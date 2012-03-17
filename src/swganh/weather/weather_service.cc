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

#include "anh/app/kernel_interface.h"
#include "anh/service/service_manager.h"

/// CHECK THIS
#include "swganh/messages/server_weather_message.h"

using namespace anh::app;
using namespace anh::service;
using namespace std;
using namespace swganh::weather;
using namespace swganh::messages;

WeatherService::WeatherService(KernelInterface* kernel) : BaseService(kernel)
{}

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

void WeatherService::SendServerWeatherMessage_()
{
}

void WeatherService::onStart()
{
}