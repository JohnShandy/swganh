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

#ifndef SWGANH_TRADE_TRADE_SERVICE_H_
#define SWGANH_TRADE_TRADE_SERVICE_H_

#include <cstdint>
#include <memory>
#include <string>

#include "swganh/base/base_service.h"

#include "swganh/connection/connection_client.h"

#include "swganh/object/object_controller.h"
#include "swganh/messages/controllers/command_queue_enqueue.h"
#include "swganh/messages/controllers/secure_trade.h"

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

namespace swganh {
namespace object {
	namespace creature { class Creature; }
	namespace tangible { class Tangible; }
}
namespace simulation {
	class SimulationService;
}} // namespace swganh::object

namespace swganh {
namespace trade {

	class TradeService: public swganh::base::BaseService
	{
	public:
		explicit TradeService(anh::app::KernelInterface* kernel);

		anh::service::ServiceDescription GetServiceDescription();

		void RequestTrade(
			const std::shared_ptr<swganh::object::creature::Creature>& actor,
			const std::shared_ptr<swganh::object::tangible::Tangible>& target);
		
		void BeginTrade(
			const std::shared_ptr<swganh::object::creature::Creature>& actor,
			const std::shared_ptr<swganh::object::tangible::Tangible>& target);
		
	private:
		// Handlers
		/*void HandleTrade_(
			const std::shared_ptr<swganh::object::creature::Creature>& actor, // creature object
			const std::shared_ptr<swganh::object::tangible::Tangible>& target, // target object
			const swganh::messages::controllers::CommandQueueEnqueue& command);*/

		void HandleSecureTrade_(
			const std::shared_ptr<swganh::object::ObjectController>& controller,
			const swganh::messages::ObjControllerMessage& message);

		/*void HandleTradeAccept_(
			const std::shared_ptr<swganh::object::creature::Creature>& actor, // creature object
			const std::shared_ptr<swganh::object::tangible::Tangible>& target, // target object
			const swganh::messages::controllers::CommandQueueEnqueue& command);*/
		
		void HandleAbortTradeMessage_(
			const std::shared_ptr<swganh::connection::ConnectionClient>& client,
			const swganh::messages::AbortTradeMessage& message);
		
		void HandleAcceptTransactionMessage_(
			const std::shared_ptr<swganh::connection::ConnectionClient>& client,
			const swganh::messages::AcceptTransactionMessage& message);
		
		void HandleAddItemMessage_(
			const std::shared_ptr<swganh::connection::ConnectionClient>& client,
			const swganh::messages::AddItemMessage& message);
			
		void HandleDenyTradeMessage_(
			const std::shared_ptr<swganh::connection::ConnectionClient>& client,
			const swganh::messages::DenyTradeMessage& message);
		
		void HandleGiveMoneyMessage_(
			const std::shared_ptr<swganh::connection::ConnectionClient>& client,
			const swganh::messages::GiveMoneyMessage& message);
		
		void HandleRemoveItemMessage_(
			const std::shared_ptr<swganh::connection::ConnectionClient>& client,
			const swganh::messages::RemoveItemMessage& message);
		
		void HandleUnAcceptTransactionMessage_(
			const std::shared_ptr<swganh::connection::ConnectionClient>& client,
			const swganh::messages::UnAcceptTransactionMessage& message);
		
		void HandleVerifyTradeMessage_(
			const std::shared_ptr<swganh::connection::ConnectionClient>& client,
			const swganh::messages::VerifyTradeMessage& message);
		
		// Sender for ObjController SecureTrade
		void SendSecureTrade_(
			const std::shared_ptr<swganh::connection::ConnectionClient>& client,
			uint64_t trader_id,
			uint64_t target_id);
		
		// Senders
		void SendAbortTradeMessage_(
			const std::shared_ptr<swganh::connection::ConnectionClient>& client);
			
		void SendAcceptTransactionMessage_(
			const std::shared_ptr<swganh::connection::ConnectionClient>& client);

		void SendAddItemFailedMessage_(
			const std::shared_ptr<swganh::connection::ConnectionClient>& client,
			uint64_t item_id);

		void SendAddItemMessage_(
			const std::shared_ptr<swganh::connection::ConnectionClient>& client,
			uint64_t item_id);

		void SendBeginTradeMessage_(
			const std::shared_ptr<swganh::connection::ConnectionClient>& client,
			uint64_t target_id);

		void SendDenyTradeMessage_(
			const std::shared_ptr<swganh::connection::ConnectionClient>& client);
		
		void SendGiveMoneyMessage_(
			const std::shared_ptr<swganh::connection::ConnectionClient>& client,
			uint32_t credit_amount);

		void SendRemoveItemMessage_(
			const std::shared_ptr<swganh::connection::ConnectionClient>& client,
			uint64_t item_id);

		void SendTradeCompleteMessage_(
			const std::shared_ptr<swganh::connection::ConnectionClient>& client);

		void SendUnAcceptTransactionMessage_(
			const std::shared_ptr<swganh::connection::ConnectionClient>& client);
		
		void SendVerifyTradeMessage_(
			const std::shared_ptr<swganh::connection::ConnectionClient>& client);
			
		void onStart();

		std::shared_ptr<swganh::simulation::SimulationService> simulation_service_;
	};

}} // namespace swganh::trade

#endif // SWGANH_TRADE_TRADE_SERVICE_H_