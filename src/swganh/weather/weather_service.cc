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

using namespace anh::app;
using namespace anh::service;
using namespace std;
using namespace swganh::messages;
using namespace swganh::scripting;
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
}

void WeatherService::SetSceneWeather(
	uint32_t scene_id,
	Weather weather,
	glm::vec3 cloud_vector)
{
}

boost::python::object WeatherService::operator()(
	anh::app::KernelInterface* kernel)
{
	script_.SetContext("kernel", boost::python::ptr(kernel));

	script_.Run();

	return script_.GetGlobals();
}

void WeatherService::SendServerWeatherMessage_(
	Weather weather,
	glm::vec3 cloud_vector)
{
}

void WeatherService::onStart()
{
}