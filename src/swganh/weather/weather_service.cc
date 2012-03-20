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

#include "swganh/weather/weather_service.h"

#include <cppconn/exception.h>
#include <cppconn/connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/sqlstring.h>
#include <boost/log/trivial.hpp>

#include "anh/app/kernel_interface.h"
#include "anh/database/database_manager_interface.h"
#include "anh/service/service_manager.h"

#include "swganh/messages/server_weather_message.h"
#include "swganh/simulation/simulation_service.h"

using namespace anh::app;
using namespace anh::service;
using namespace std;
using namespace swganh::messages;
using namespace swganh::scripting;
using namespace swganh::simulation;
using namespace swganh::weather;

WeatherService::WeatherService(KernelInterface* kernel) : BaseService(kernel), script_("scripts/weather/weather.py")
{
}

ServiceDescription WeatherService::GetServiceDescription()
{
	ServiceDescription service_description(
		"WeatherService",
		"weather",
		"0.1",
		"127.0.0.1",
		0,
		0,
		0);

	return service_description;
}

Weather WeatherService::GetSceneWeather(
	uint32_t scene_id)
{
	Weather weather_type;
	string scene_name;

	try
	{
		auto db_manager = kernel()->GetDatabaseManager();
		auto conn = db_manager->getConnection("galaxy");
		
		auto statement = unique_ptr<sql::PreparedStatement>(conn->prepareStatement("SELECT weather_id FROM scene WHERE id = ?"));
		statement->setUInt(1, scene_id);

		auto result = unique_ptr<sql::ResultSet>(statement->executeQuery());

		while (result->next())
		{
			weather_type = Weather(result->getUInt("weather_id"));
			scene_name = result->getString("name");
		}

		BOOST_LOG_TRIVIAL(info) << "Retrieved (" << weather_type << ") from " << scene_name;
	}
	catch (sql::SQLException &e)
	{
		BOOST_LOG_TRIVIAL(error) << "SQLException at " << __FILE__ << " (" << __LINE__ << ": " << __FUNCTION__ << ")";
		BOOST_LOG_TRIVIAL(error) << "MySQL Error: (" << e.getErrorCode() << ": " << e.getSQLState() << ") " << e.what();
	}
	
	return weather_type;
}

void WeatherService::SetSceneWeather(
	uint32_t scene_id,
	Weather weather_type,
	glm::vec3 cloud_vector)
{
	try
	{
		auto db_manager = kernel()->GetDatabaseManager();
		auto conn = db_manager->getConnection("galaxy");
		
		auto statement = unique_ptr<sql::PreparedStatement>(conn->prepareStatement("UPDATE scene SET weather_id = ? WHERE id = ?"));
		statement->setUInt(1, Weather(weather_type));
		statement->setUInt(2, scene_id);
		statement->executeUpdate();

		BOOST_LOG_TRIVIAL(info) << "Set (" << weather_type << ") on " << " scene " << scene_id;
	}
	catch (sql::SQLException &e)
	{
		BOOST_LOG_TRIVIAL(error) << "SQLException at " << __FILE__ << " (" << __LINE__ << ": " << __FUNCTION__ << ")";
		BOOST_LOG_TRIVIAL(error) << "MySQL Error: (" << e.getErrorCode() << ": " << e.getSQLState() << ") " << e.what();
	}
}

boost::python::object WeatherService::operator()(
	anh::app::KernelInterface* kernel)
{
	script_.SetContext("kernel", boost::python::ptr(kernel));

	script_.Run();

	return script_.GetGlobals();
}

void WeatherService::SendServerWeatherMessage_(
	Weather weather_type,
	glm::vec3 cloud_vector)
{
	ServerWeatherMessage server_weather_message;
	server_weather_message.weather_id = Weather(weather_type);
	server_weather_message.cloud_vector = cloud_vector;

	simulation_service_ = std::static_pointer_cast<SimulationService>(kernel()->GetServiceManager()->GetService("SimulationService"));

	//
}

void WeatherService::onStart()
{
}