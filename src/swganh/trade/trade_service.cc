/*
 This file is part of SWGANH. For more information, visit http://swganh.com
 
 Copyright (c) 2006 - 2011 The SWG:ANH Team

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "swganh/trade/trade_service.h"
#include "swganh/trade/trade_session.h"

#ifdef WIN32
#include <regex>
#else
#include <boost/regex.hpp>
#endif

#include <boost/log/trivial.hpp>

#include "anh/app/kernel_interface.h"
#include "anh/network/soe/session.h"
#include "anh/service/service_manager.h"

///// CHECK THIS
#include "swganh/connection/connection_client.h"

#include "swganh/messages/controllers/command_queue_enqueue.h"
#include "swganh/messages/controllers/secure_trade.h"
#include "swganh/messages/out_of_band.h"

#include "swganh/messages/abort_trade_message.h"
#include "swganh/messages/accept_transaction_message.h"
#include "swganh/messages/add_item_failed_message.h"
#include "swganh/messages/add_item_message.h"
#include "swganh/messages/begin_trade_message.h"
#include "swganh/messages/deny_trade_message.h"
#include "swganh/messages/give_money_message.h"
#include "swganh/messages/remove_item_message.h"
#include "swganh/messages/trade_complete_message.h"
#include "swganh/messages/unaccept_transaction_message.h"
#include "swganh/messages/verify_trade_message.h"

#include "swganh/object/object.h"
#include "swganh/object/object_controller.h"
#include "swganh/object/creature/creature.h"
#include "swganh/object/player/player.h"

#include "swganh/command/command_service.h"
#include "swganh/connection/connection_service.h"
#include "swganh/simulation/simulation_service.h"

using namespace anh::app;
using namespace anh::service;
using namespace std;
using namespace swganh::trade;
using namespace swganh::command;
using namespace swganh::connection;
using namespace swganh::messages;
using namespace swganh::messages::controllers;
using namespace swganh::object;
using namespace swganh::object::creature;
using namespace swganh::simulation;

#ifdef WIN32
using std::wregex;
using std::wsmatch;
using std::regex_match;
#else
using boost::wregex;
using boost::wsmatch;
using boost::regex_match;
#endif

TradeService::TradeService(KernelInterface* kernel) : BaseService(kernel)
{}

ServiceDescription TradeService::GetServiceDescription()
{
	ServiceDescription service_description(
		"TradeService",
		"trade",
		"0.1",
		"127.0.0.1",
		0,
		0,
		0);
		
	return service_description;
}

void TradeService::RequestTrade(
	const std::shared_ptr<Creature>& actor,
	const std::shared_ptr<Creature>& target)
{
	SendSecureTrade_(target->GetController()->GetRemoteClient(), actor->GetObjectId(), target->GetObjectId());
	//actor->GetController()->SendSystemMessage(OutOfBand("ui_trade", "request_sent_prose"));
	target->GetController()->SendSystemMessage(OutOfBand("ui_trade", "requested_prose", TU, actor->GetObjectId()));

	BeginTrade(actor, target);
}

void TradeService::BeginTrade(
	const std::shared_ptr<Creature>& actor,
	const std::shared_ptr<Creature>& target)
{
	StartTradeSession_(actor->GetObjectId(), target->GetObjectId());
	
	SendBeginTradeMessage_(actor->GetController()->GetRemoteClient(), target->GetObjectId());
	SendBeginTradeMessage_(target->GetController()->GetRemoteClient(), actor->GetObjectId());
}

// Handlers

// Kronos advised that I don't need this if I am going to
// go with a python script as my handler.
/*void TradeService::HandleTrade_(
	const std::shared_ptr<swganh::object::creature::Creature>& actor, // creature object
	const std::shared_ptr<swganh::object::tangible::Tangible>& target, // target object
	const swganh::messages::controllers::CommandQueueEnqueue& command)
{
	
		So, I'm not quite certain what I should do with the CQE object that
		gets passed to this handler. Other than the "target," there are no
		command parameters that I need to parse out from the /trade CQE. I'm not
		even sure that it needs to be passed to this handler, but I am
		kind of following along with chat_service & combat_service as
		my examples.
	
	
	SecureTrade secure_trade;
	secure_trade.trader_id = actor->GetObjectId();
	
	if (target)
	{
		secure_trade.target_id = target->GetObjectId();
	}

	// I know that the server is supposed to receive this object controller from the actor,
	// and also send it to the target, but i'm not sure if the trader and target IDs should be swapped
	// or left as they are in the one the server receives. So, here I send the ObjController as-is to
	// the target.
	target->GetController()->GetRemoteClient()->SendMessage(ObjControllerMessage(0x0000000B, secure_trade));
}*/

void TradeService::HandleSecureTrade_(
	const std::shared_ptr<ObjectController>& controller,
	const ObjControllerMessage& message)
{
	BOOST_LOG_TRIVIAL(info) << "Handling SecureTrade";

	SecureTrade secure_trade;
	secure_trade.Deserialize(message.data);

	auto actor = static_pointer_cast<Creature>(controller->GetObject());
	auto target = simulation_service_->GetObjectById<Creature>(secure_trade.target_id);

	// if target is not a creature, it cannot be traded with
	if (target == nullptr)
	{
		actor->GetController()->SendSystemMessage(OutOfBand("ui_trade", "start_fail_target_not_player"));
		return;
	}
	// actor cannot trade with self, only other players
	else if (actor->GetObjectId() == target->GetObjectId())
	{
		actor->GetController()->SendSystemMessage(OutOfBand("ui_trade", "start_fail_target_not_player"));
		return;
	}
	// trader dead or incapacitated?
	else if (actor->IsDead() || actor->IsIncapacitated())
	{
		actor->GetController()->SendSystemMessage(OutOfBand("error_message", "wrong_state"));
		return;
	}
	// target dead or incapacitated?
	else if (target->IsDead() || target->IsIncapacitated())
	{
		actor->GetController()->SendSystemMessage(OutOfBand("error_message", "wrong_state"));
		return;
	}

	// actor and target not in any kind of negative or preoccupied state?
	if (actor->HasState(NONE) && target->HasState(NONE))
	{
		RequestTrade(actor, target);
	}
	else
	{
		actor->GetController()->SendSystemMessage(OutOfBand("error_message", "wrong_state"));
	}
}

// Kronos advised that I don't need this if I am going to
// go with a python script as my handler.
/*void TradeService::HandleTradeAccept_(
	const std::shared_ptr<swganh::object::creature::Creature>& actor, // creature object
	const std::shared_ptr<swganh::object::tangible::Tangible>& target, // target object
	const swganh::messages::controllers::CommandQueueEnqueue& command)
{
	SendBeginTradeMessage_(actor->GetController()->GetRemoteClient(), target->GetObjectId());
	SendBeginTradeMessage_(target->GetController()->GetRemoteClient(), actor->GetObjectId());
}*/

void TradeService::HandleAbortTradeMessage_(
	const std::shared_ptr<ConnectionClient>& client,
	const AbortTradeMessage& message)
{
	auto trade_session = GetTradeSessionByActorId_(client->GetController()->GetObject()->GetObjectId());
	auto target = simulation_service_->GetObjectById<Creature>(trade_session.target_id);
	
	SendAbortTradeMessage_(target->GetController()->GetRemoteClient());
	SendTradeCompleteMessage_(target->GetController()->GetRemoteClient()); // Abort needs to be followed with TradeCompleteMessage to end the trade session and close the trade window

	client->GetController()->SendSystemMessage(OutOfBand("ui_trade", "aborted"));
	target->GetController()->SendSystemMessage(OutOfBand("ui_trade", "aborted"));
	
	EndTradeSession_(client->GetController()->GetObject()->GetObjectId()); // end the TradeSession
}

void TradeService::HandleAcceptTransactionMessage_(
	const std::shared_ptr<ConnectionClient>& client,
	const AcceptTransactionMessage& message)
{
	auto trade_session = GetTradeSessionByActorId_(client->GetController()->GetObject()->GetObjectId());
	auto target = simulation_service_->GetObjectById<Creature>(trade_session.target_id);

	SendAcceptTransactionMessage_(target->GetController()->GetRemoteClient());
	client->GetController()->SendSystemMessage(OutOfBand("ui_trade", "waiting_complete_prose", TU, target->GetObjectId()));

	// Set the TradeSession to reflect that the actor has accepted the trade
	trade_session.actor_accepted = true;
}

void TradeService::HandleAddItemMessage_(
	const std::shared_ptr<ConnectionClient>& client,
	const AddItemMessage& message)
{
	/*
		Any AddItemMessage, RemoveItemMessage, or GiveMoneyMessage, which modifies the
		contents of the trade window after one party has already sent an AcceptTransactionMessage,
		should cause the server to dispatch an UnAcceptTransactionMessage, to notify the other
		player that the trade contents have changed, otherwise they'd potentially accept a trade
		they don't agree to, or perhaps items/credits could fail to be sent, or could be duped.
		
		The UnAcceptTransactionMessage doesn't necessarily need to be sent after every modification message,
		but only to undo a previous AcceptTransactionMessage if and when a trade's contents are modified.

		In addition, the TradeSession should also be set to reflect that the actor has unaccepted the trade.
	*/
	auto trade_session = GetTradeSessionByActorId_(client->GetController()->GetObject()->GetObjectId());
	auto target = simulation_service_->GetObjectById<Creature>(trade_session.target_id);

	/*
		Some checking should ultimately be done here to make sure the item being added is
		a tradeable item. If it isn't, the server should send an AddItemFailedMessage to the actor,
		and refrain from sending an AddItemMessage to the target.
	*/
	SendAddItemMessage_(target->GetController()->GetRemoteClient(), message.item_id);
	// When the item can't be added, call SendAddItemFailed_() and send the following OutOfBand package
	// client->GetController()->SendSystemMessage(OutOfBand("ui_trade", "add_item_failed_prose", TT, item_id));

	// Add the item's ID to the list of item IDs of the actor's trade window contents
	trade_session.actor_trade_items.push_back(message.item_id);

	if (trade_session.actor_accepted)
	{
		// The trade contents have changed, send an unaccept to the trade partner to prevent a faulty trade
		SendUnAcceptTransactionMessage_(target->GetController()->GetRemoteClient());

		// Set the TradeSession to reflect that the actor has modified, and therefore unaccepted the trade
		trade_session.actor_accepted = false;
	}
}

void TradeService::HandleDenyTradeMessage_(
	const std::shared_ptr<ConnectionClient>& client,
	const DenyTradeMessage& message)
{
	auto trade_session = GetTradeSessionByActorId_(client->GetController()->GetObject()->GetObjectId());
	auto target = simulation_service_->GetObjectById<Creature>(trade_session.target_id);
	
	SendDenyTradeMessage_(target->GetController()->GetRemoteClient());
}

void TradeService::HandleGiveMoneyMessage_(
	const std::shared_ptr<ConnectionClient>& client,
	const GiveMoneyMessage& message)
{
	auto trade_session = GetTradeSessionByActorId_(client->GetController()->GetObject()->GetObjectId());
	auto target = simulation_service_->GetObjectById<Creature>(trade_session.target_id);

	SendGiveMoneyMessage_(target->GetController()->GetRemoteClient(), message.credit_amount);

	// Set the actor's credit amount to the amount of credits the actor has committed to the trade window
	trade_session.actor_trade_credit_amount = message.credit_amount;	

	if (trade_session.actor_accepted)
	{
		// The trade contents have changed, send an unaccept to the trade partner to prevent a faulty trade
		SendUnAcceptTransactionMessage_(target->GetController()->GetRemoteClient());

		// Set the TradeSession to reflect that the actor has modified, and therefore unaccepted the trade
		trade_session.actor_accepted = false;
	}
}

void TradeService::HandleRemoveItemMessage_(
	const std::shared_ptr<ConnectionClient>& client,
	const RemoveItemMessage& message)
{
	auto trade_session = GetTradeSessionByActorId_(client->GetController()->GetObject()->GetObjectId());
	auto target = simulation_service_->GetObjectById<Creature>(trade_session.target_id);

	SendRemoveItemMessage_(target->GetController()->GetRemoteClient(), message.item_id);

	// Remove the item's ID from the list of item IDs of the actor's trade window contents
	trade_session.actor_trade_items.remove(message.item_id);

	if (trade_session.actor_accepted)
	{
		// The trade contents have changed, send an unaccept to the trade partner to prevent a faulty trade
		SendUnAcceptTransactionMessage_(target->GetController()->GetRemoteClient());

		// Set the TradeSession to reflect that the actor has modified, and therefore unaccepted the trade
		trade_session.actor_accepted = false;
	}
}

void TradeService::HandleUnAcceptTransactionMessage_(
	const std::shared_ptr<ConnectionClient>& client,
	const UnAcceptTransactionMessage& message)
{
	auto trade_session = GetTradeSessionByActorId_(client->GetController()->GetObject()->GetObjectId());
	auto target = simulation_service_->GetObjectById<Creature>(trade_session.target_id);

	SendUnAcceptTransactionMessage_(target->GetController()->GetRemoteClient());

	// Set the TradeSession to reflect that the actor has unaccepted the trade
	trade_session.actor_accepted = false;
}

void TradeService::HandleVerifyTradeMessage_(
	const std::shared_ptr<ConnectionClient>& client,
	const VerifyTradeMessage& message)
{
	auto trade_session = GetTradeSessionByActorId_(client->GetController()->GetObject()->GetObjectId());
	auto target = simulation_service_->GetObjectById<Creature>(trade_session.target_id);

	// Determine which party is verifying
	if (client->GetController()->GetObject()->GetObjectId() == trade_session.actor_id)
		trade_session.actor_accepted = true;
	else if (client->GetController()->GetObject()->GetObjectId() == trade_session.target_id)
		trade_session.target_accepted = true;

	if (trade_session.actor_accepted == true && trade_session.target_accepted == true)
	{
		// Do everything necessary to complete the trade. (Needs item management)

		SendTradeCompleteMessage_(client);
		SendTradeCompleteMessage_(target->GetController()->GetRemoteClient());
		client->GetController()->SendSystemMessage(OutOfBand("ui_trade", "complete"));
		target->GetController()->SendSystemMessage(OutOfBand("ui_trade", "complete"));
	}
	else
	{
		SendDenyTradeMessage_(client);
		SendDenyTradeMessage_(target->GetController()->GetRemoteClient());
	}
}

// Sender for ObjController SecureTrade
void TradeService::SendSecureTrade_(
	const std::shared_ptr<ConnectionClient>& client,
	uint64_t trader_id,
	uint64_t target_id)
{
	SecureTrade secure_trade;
	secure_trade.trader_id = trader_id;
	secure_trade.target_id = target_id;
	client->SendMessage(ObjControllerMessage(0x0000000B, secure_trade));
}

// Senders
void TradeService::SendAbortTradeMessage_(
	const std::shared_ptr<ConnectionClient>& client)
{
	AbortTradeMessage abort_trade_message;
	client->SendMessage(abort_trade_message);
}

void TradeService::SendAcceptTransactionMessage_(
	const std::shared_ptr<ConnectionClient>& client)
{
	AcceptTransactionMessage accept_transaction_message;
	client->SendMessage(accept_transaction_message);
}

void TradeService::SendAddItemFailedMessage_(
	const std::shared_ptr<ConnectionClient>& client,
	uint64_t item_id)
{
	AddItemFailedMessage add_item_failed_message;
	add_item_failed_message.item_id = item_id;
	client->SendMessage(add_item_failed_message);
}

void TradeService::SendAddItemMessage_(
	const std::shared_ptr<ConnectionClient>& client,
	uint64_t item_id)
{
	AddItemMessage add_item_message;
	add_item_message.item_id = item_id;
	client->SendMessage(add_item_message);
}

void TradeService::SendBeginTradeMessage_(
	const std::shared_ptr<ConnectionClient>& client,
	uint64_t target_id)
{
	BeginTradeMessage begin_trade_message;
	begin_trade_message.target_id = target_id;
	client->SendMessage(begin_trade_message);
}

void TradeService::SendDenyTradeMessage_(
	const std::shared_ptr<ConnectionClient>& client)
{
	DenyTradeMessage deny_trade_message;
	client->SendMessage(deny_trade_message);
}

void TradeService::SendGiveMoneyMessage_(
	const std::shared_ptr<ConnectionClient>& client,
	uint32_t credit_amount)
{
	GiveMoneyMessage give_money_message;
	give_money_message.credit_amount = credit_amount;
	client->SendMessage(give_money_message);
}

void TradeService::SendRemoveItemMessage_(
	const std::shared_ptr<ConnectionClient>& client,
	uint64_t item_id)
{
	RemoveItemMessage remove_item_message;
	remove_item_message.item_id = item_id;
	client->SendMessage(remove_item_message);
}

void TradeService::SendTradeCompleteMessage_(
	const std::shared_ptr<ConnectionClient>& client)
{
	TradeCompleteMessage trade_complete_message;
	client->SendMessage(trade_complete_message);
}

void TradeService::SendUnAcceptTransactionMessage_(
	const std::shared_ptr<ConnectionClient>& client)
{
	UnAcceptTransactionMessage unaccept_transaction_message;
	client->SendMessage(unaccept_transaction_message);
}

void TradeService::SendVerifyTradeMessage_(
	const std::shared_ptr<ConnectionClient>& client)
{
	VerifyTradeMessage verify_trade_message;
	client->SendMessage(verify_trade_message);
}

void TradeService::onStart()
{
	// Register ObjController Handlers with the SimulationService
	simulation_service_ = std::static_pointer_cast<SimulationService>(kernel()->GetServiceManager()->GetService("SimulationService"));

	simulation_service_->RegisterControllerHandler(0x00000115,
		[this] (
		const std::shared_ptr<ObjectController>& controller,
		const ObjControllerMessage& message)
	{
		HandleSecureTrade_(controller, message);
	});

	// Register Message Handlers with the ConnectionService
	auto connection_service = std::static_pointer_cast<ConnectionService>(kernel()->GetServiceManager()->GetService("ConnectionService"));

	connection_service->RegisterMessageHandler(&TradeService::HandleAbortTradeMessage_, this);
	connection_service->RegisterMessageHandler(&TradeService::HandleAcceptTransactionMessage_, this);
	connection_service->RegisterMessageHandler(&TradeService::HandleAddItemMessage_, this);
	connection_service->RegisterMessageHandler(&TradeService::HandleDenyTradeMessage_, this);
	connection_service->RegisterMessageHandler(&TradeService::HandleGiveMoneyMessage_, this);
	connection_service->RegisterMessageHandler(&TradeService::HandleRemoveItemMessage_, this);
	connection_service->RegisterMessageHandler(&TradeService::HandleUnAcceptTransactionMessage_, this);
	connection_service->RegisterMessageHandler(&TradeService::HandleVerifyTradeMessage_, this);
}

void TradeService::StartTradeSession_(
	uint64_t actor_id,
	uint64_t target_id)
{
	TradeSession trade_session;
	trade_session.actor_id = actor_id;
	trade_session.target_id = target_id;

	TradeSessionList.push_back(trade_session);
}

void TradeService::EndTradeSession_(
	uint64_t actor_id)
{
	auto session_ = std::find_if(TradeSessionList.begin(), TradeSessionList.end(), [=](TradeSession& session)
	{
		if (session.actor_id == actor_id)
			return true;
		else
			return false;
	});

	TradeSessionList.erase(session_);
}

TradeSession TradeService::GetTradeSessionByActorId_(
	uint64_t actor_id)
{
	auto session_ = std::find_if(TradeSessionList.begin(), TradeSessionList.end(), [=](TradeSession& session)
	{
		if (session.actor_id == actor_id)
			return true;
		else
			return false;
	});

	return *session_;
}