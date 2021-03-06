// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#ifndef SWGANH_KERNEL_H_
#define SWGANH_KERNEL_H_

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>

#include "anh/app/kernel_interface.h"

namespace swganh {
namespace tre {
    class TreArchive;
}}  // namespace swganh::tre

namespace swganh {
namespace app {

/*!
 * @Brief Contains information about the app config"
 */
struct AppConfig {
	std::string server_mode;
    std::vector<std::string> plugins;
    std::string plugin_directory;
    std::string script_directory;
    std::string galaxy_name;
    std::string tre_config;
    uint32_t resource_cache_size;

    /*!
    * @Brief Contains information about the database config"
    */
    struct DatabaseConfig {
        std::string host;
        std::string schema;
        std::string username;
        std::string password;
    } galaxy_manager_db, galaxy_db;

    /*!
    * @Brief Contains information about the Login config"
     */
    struct LoginConfig {
        std::string listen_address;
        uint16_t listen_port;
        int galaxy_status_check_duration_secs;
        int login_error_timeout_secs;
        bool login_auto_registration;
    } login_config;
    /*!
    * @Brief Contains information about the app config"
    */
    struct ConnectionConfig {
        std::string listen_address;
        uint16_t listen_port;
        uint16_t ping_port;
    } connection_config;

    boost::program_options::options_description BuildConfigDescription();
};
    
class SwganhKernel : public anh::app::KernelInterface {
public:
    explicit SwganhKernel(boost::asio::io_service& io_service);
    virtual ~SwganhKernel();

    const anh::app::Version& GetVersion();

    AppConfig& GetAppConfig();

    anh::database::DatabaseManagerInterface* GetDatabaseManager();
    
    anh::EventDispatcher* GetEventDispatcher();

    anh::plugin::PluginManager* GetPluginManager();

    anh::service::ServiceManager* GetServiceManager();
    
    anh::service::ServiceDirectoryInterface* GetServiceDirectory();
    
    boost::asio::io_service& GetIoService();

    anh::resource::ResourceManager* GetResourceManager();

private:
    SwganhKernel();
    anh::app::Version version_;
    swganh::app::AppConfig app_config_;
    
    std::unique_ptr<anh::database::DatabaseManagerInterface> database_manager_;
    std::unique_ptr<anh::EventDispatcher> event_dispatcher_;
    std::unique_ptr<anh::plugin::PluginManager> plugin_manager_;
    std::unique_ptr<anh::service::ServiceManager> service_manager_;
    std::unique_ptr<anh::service::ServiceDirectoryInterface> service_directory_;
    std::unique_ptr<anh::resource::ResourceManager> resource_manager_;

    boost::asio::io_service& io_service_;
};

}}  // namespace anh::app

#endif  // SWGANH_KERNEL_H_