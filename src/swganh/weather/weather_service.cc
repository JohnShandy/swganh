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

#include <algorithm>
#include <ctime>

#include <cppconn/exception.h>
#include <cppconn/connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/sqlstring.h>

#include <anh/logger.h>

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

using swganh::app::SwganhKernel;

WeatherEvent::WeatherEvent()
{
}

float WeatherEvent::GetDuration() { return duration; }
void WeatherEvent::SetDuration(float seconds)
{
    duration = seconds;
}

Weather WeatherEvent::GetWeatherType() { return weather_type; }
void WeatherEvent::SetWeatherType(Weather weather)
{
    weather_type = weather;
}

glm::vec3 WeatherEvent::GetCloudVector() { return cloud_vector; }
void WeatherEvent::SetCloudVector(glm::vec3 vector)
{
    cloud_vector.x = vector.x;
    cloud_vector.y = vector.y;
    cloud_vector.z = vector.z;
}

WeatherService::WeatherService(SwganhKernel* kernel) : BaseService(kernel), script_("scripts/weather/weather.py")
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

		LOG(info) << "Retrieved (" << weather_type << ") from " << scene_name;
	}
	catch (sql::SQLException& e)
	{
		LOG(error) << "SQLException at " << __FILE__ << " (" << __LINE__ << ": " << __FUNCTION__ << ")";
		LOG(error) << "MySQL Error: (" << e.getErrorCode() << ": " << e.getSQLState() << ") " << e.what();
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
		
		auto statement = unique_ptr<sql::PreparedStatement>(conn->prepareStatement("UPDATE scene SET weather_id = ?, cloud_vector_x = ?, cloud_vector_y = ?, cloud_vector_z = ? WHERE id = ?"));
        statement->setUInt(1, weather_type);
        statement->setDouble(2, cloud_vector.x);
        statement->setDouble(3, cloud_vector.y);
        statement->setDouble(4, cloud_vector.z);
		statement->setUInt(5, scene_id);
		statement->executeUpdate();

        SendServerWeatherMessage_(weather_type, cloud_vector, scene_id);

		LOG(info) << "Set weather on scene " << scene_id << " to " << weather_type;
	}
	catch (sql::SQLException& e)
	{
		LOG(error) << "SQLException at " << __FILE__ << " (" << __LINE__ << ": " << __FUNCTION__ << ")";
		LOG(error) << "MySQL Error: (" << e.getErrorCode() << ": " << e.getSQLState() << ") " << e.what();
	}
}

void WeatherService::RunWeatherSequence(
    uint32_t scene_id,
    std::vector<WeatherEvent> weather_sequence)
{
    for_each(weather_sequence.begin(), weather_sequence.end(), [=] (WeatherEvent weather_event) {
        auto start = time(NULL);
        while (start)
        {
            auto elapsed = time(NULL) - start;
            if (elapsed == weather_event.GetDuration())
            {
                SetSceneWeather(scene_id, weather_event.GetWeatherType(), weather_event.GetCloudVector());
                break;
            }
        }
    });
}

void WeatherService::SendServerWeatherMessage_(
	Weather weather_type,
	glm::vec3 cloud_vector,
    uint32_t scene_id)
{
	ServerWeatherMessage server_weather_message;
	server_weather_message.weather_id = weather_type;
	server_weather_message.cloud_vector = cloud_vector;

	auto simulation_service_ = kernel()->GetServiceManager()->GetService<SimulationService>("SimulationService");
    simulation_service_->SendToAllInScene(server_weather_message, scene_id);
}

void WeatherService::onStart()
{
    try
    {
        script_.SetContext("kernel", boost::python::ptr(kernel()));
        script_.Run();
    }
    catch (std::exception& e)
    {
        LOG(fatal) << e.what();
    }
}