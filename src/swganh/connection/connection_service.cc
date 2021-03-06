// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#include "swganh/connection/connection_service.h"

#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lexical_cast.hpp>
#include "anh/logger.h"

#include "anh/crc.h"
#include "anh/event_dispatcher.h"
#include "anh/network/soe/server.h"
#include "anh/plugin/plugin_manager.h"
#include "anh/service/service_directory_interface.h"
#include "anh/service/service_manager.h"

#include "swganh/app/swganh_kernel.h"

#include "swganh/character/character_service_interface.h"
#include "swganh/character/character_provider_interface.h"
#include "swganh/connection/ping_server.h"
#include "swganh/connection/connection_client.h"
#include "swganh/connection/providers/session_provider_interface.h"

#include "swganh/object/object.h"
#include "swganh/object/player/player.h"

#include "swganh/messages/logout_message.h"

using namespace anh::app;
using namespace anh::event_dispatcher;
using namespace anh::network::soe;
using namespace anh::service;
using namespace swganh::character;
using namespace swganh::connection;
using namespace swganh::login;
using namespace swganh::messages;
using namespace swganh::object;
using namespace swganh::simulation;

using namespace std;

using anh::ValueEvent;

using boost::asio::ip::udp;
using swganh::app::SwganhKernel;

ConnectionService::ConnectionService(
        string listen_address,
        uint16_t listen_port,
        uint16_t ping_port,
        SwganhKernel* kernel)
    : swganh::network::BaseSwgServer(kernel->GetIoService())
    , kernel_(kernel)
    , ping_server_(nullptr)
    , active_(kernel->GetIoService())
    , listen_address_(listen_address)
    , listen_port_(listen_port)
    , ping_port_(ping_port)
{

    session_provider_ = kernel_->GetPluginManager()->CreateObject<providers::SessionProviderInterface>("Login::SessionProvider");

    character_provider_ = kernel_->GetPluginManager()->CreateObject<CharacterProviderInterface>("Character::CharacterProvider");
}

ConnectionService::~ConnectionService()
{
    session_timer_->cancel();
}

ServiceDescription ConnectionService::GetServiceDescription() {
    auto listen_address = Resolve(listen_address_);

    ServiceDescription service_description(
        "ANH Connection Service",
        "connection",
        "0.1",
        listen_address,
        0,
        listen_port(),
        ping_port_);

    return service_description;
}

void ConnectionService::Startup() {
    ping_server_ = make_shared<PingServer>(kernel_->GetIoService(), ping_port_);

    character_service_ = kernel_->GetServiceManager()->GetService<CharacterServiceInterface>("CharacterService");
    login_service_ = kernel_->GetServiceManager()->GetService<LoginService>("LoginService");
    simulation_service_ = kernel_->GetServiceManager()->GetService<SimulationServiceInterface>("SimulationService");
    
    RegisterMessageHandler(&ConnectionService::HandleClientIdMsg_, this);
    RegisterMessageHandler(&ConnectionService::HandleCmdSceneReady_, this);

    Server::Startup(listen_port_);

    session_timer_ = active_.AsyncRepeated(boost::posix_time::milliseconds(5), [this] () {
        boost::lock_guard<boost::mutex> lg(session_map_mutex_);
        for_each(
            begin(session_map_),
            end(session_map_),
            [=] (SessionMap::value_type& type)
        {
            type.second->Update();
        });
    });
}

void ConnectionService::Shutdown() {
    BaseSwgServer::Shutdown();
}

const string& ConnectionService::listen_address() {
    return listen_address_;
}

uint16_t ConnectionService::listen_port() {
    return listen_port_;
}

shared_ptr<Session> ConnectionService::CreateSession(const udp::endpoint& endpoint)
{
    shared_ptr<ConnectionClient> session = nullptr;

    {
        boost::lock_guard<boost::mutex> lg(session_map_mutex_);
        if (session_map_.find(endpoint) == session_map_.end())
        {
            session = make_shared<ConnectionClient>(this, kernel_->GetIoService(), endpoint);
            session_map_.insert(make_pair(endpoint, session));
        }
    }

    return session;
}

bool ConnectionService::RemoveSession(std::shared_ptr<Session> session) {
    {
        boost::lock_guard<boost::mutex> lg(session_map_mutex_);
        session_map_.erase(session->remote_endpoint());
    }

    auto connection_client = static_pointer_cast<ConnectionClient>(session);

    auto controller = connection_client->GetController();
    if (controller)
    {
		auto player = simulation_service_->GetObjectById<swganh::object::player::Player>(controller->GetObject()->GetObjectId() + 1);
		kernel_->GetEventDispatcher()->Dispatch(make_shared<anh::ValueEvent<shared_ptr<swganh::object::player::Player>>>("Connection::PlayerRemoved", player));
        
        simulation_service_->PersistRelatedObjects(controller->GetObject()->GetObjectId());
    }

    LOG(info) << "Removing disconnected client";
    session_provider_->EndGameSession(connection_client->GetPlayerId());

    return true;
}

shared_ptr<Session> ConnectionService::GetSession(const udp::endpoint& endpoint) {
    {
        boost::lock_guard<boost::mutex> lg(session_map_mutex_);

        auto find_iter = session_map_.find(endpoint);
        if (find_iter != session_map_.end())
        {
            return find_iter->second;
        }
    }

    return CreateSession(endpoint);
}

std::shared_ptr<ConnectionClient> ConnectionService::FindConnectionByPlayerId(uint64_t player_id)
{
    shared_ptr<ConnectionClient> connection = nullptr;

    {
        boost::lock_guard<boost::mutex> lg(session_map_mutex_);

        auto find_iter = find_if(
            begin(session_map_),
            end(session_map_),
            [player_id] (SessionMap::value_type& item)
        {
            return item.second->GetPlayerId() == player_id;
        });

        if (find_iter != end(session_map_))
        {
            connection = find_iter->second;
        }
    }

    return connection;
}

void ConnectionService::HandleCmdSceneReady_(
    const shared_ptr<ConnectionClient>& client, 
    CmdSceneReady message)
{
    LOG(warning) << "Handling CmdSceneReady";

    client->SendTo(CmdSceneReady());

    auto object = client->GetController()->GetObject();

    kernel_->GetEventDispatcher()->Dispatch(
        make_shared<ValueEvent<shared_ptr<Object>>>("ObjectReadyEvent", object));
}

void ConnectionService::HandleClientIdMsg_(
    const shared_ptr<ConnectionClient>& client, 
    ClientIdMsg message)
{
    LOG(warning) << "Handling ClientIdMsg";

    // get session key from login service
    uint32_t account_id = login_service_->GetAccountBySessionKey(message.session_hash);

    // authorized
    if (! account_id) {
        LOG(warning) << "Account_id not found from session key, unauthorized access.";
        return;
    }

    // gets player from account
    uint64_t player_id = session_provider_->GetPlayerId(account_id);

    // authorized
    if (! player_id) {
        LOG(warning) << "No player found for the requested account, unauthorized access.";
        return;
    }

    auto existing_session_connection = FindConnectionByPlayerId(player_id);
    if (existing_session_connection)
    {
        existing_session_connection->Close();
    }

    // creates a new session and stores it for later use
    if (!session_provider_->CreateGameSession(player_id, client->connection_id())) {
        LOG(warning) << "Player Not Inserted into Session Map because No Game Session Created!";
    }

    client->Connect(account_id, player_id);

    ClientPermissionsMessage client_permissions;
    client_permissions.galaxy_available = kernel_->GetServiceDirectory()->galaxy().status();
    client_permissions.available_character_slots = static_cast<uint8_t>(character_provider_->GetMaxCharacters(account_id));
    /// @TODO: Replace with configurable value
    client_permissions.unlimited_characters = 0;

    client->SendTo(client_permissions);
}
