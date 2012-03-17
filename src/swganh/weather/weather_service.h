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

#ifndef SWGANH_WEATHER_WEATHER_SERVICE_H_
#define SWGANH_WEATHER_WEATHER_SERVICE_H_

#include <cstdint>
#include <memory>

#include "swganh/base/base_service.h"
#include "swganh/messages/server_weather_message.h"

namespace swganh {
namespace weather {

	class WeatherService: public swganh::base::BaseService
	{
	public:
		explicit WeatherService(anh::app::KernelInterface* kernel);

		anh::service::ServiceDescription GetServiceDescription();

		void SendServerWeatherMessage();

	private:
		void onStart();
	};

}} // namespace swganh::weather

#endif // SWGANH_WEATHER_WEATHER_SERVICE_H_