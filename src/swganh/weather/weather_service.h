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
#include <glm/glm.hpp>
#include <memory>

#include "anh/app/kernel_interface.h"

#include "swganh/base/base_service.h"
#include "swganh/messages/server_weather_message.h"
#include "swganh/scripting/python_script.h"

namespace swganh {
namespace simulation {
	class SimulationService;
}} // namespace swganh::simulation

namespace swganh {
namespace weather {

	enum Weather : uint32_t
	{
		NOSTORM,
		CLOUDY,
		LIGHTSTORM,
		MEDIUMSTORM,
		HEAVYSTORM
	};

	class WeatherService: public swganh::base::BaseService
	{
	public:
		explicit WeatherService(anh::app::KernelInterface* kernel);

		anh::service::ServiceDescription GetServiceDescription();

		Weather GetSceneWeather(
			uint32_t scene_id);
		
		void SetSceneWeather(
			uint32_t scene_id,
			Weather weather_type,
			glm::vec3 cloud_vector);

		boost::python::object operator()(
			anh::app::KernelInterface* kernel);

	private:
		void SendServerWeatherMessage_(
			Weather weather_type,
			glm::vec3 cloud_vector);

		swganh::scripting::PythonScript script_;

		void onStart();

		std::shared_ptr<swganh::simulation::SimulationService> simulation_service_;
	};

}} // namespace swganh::weather

#endif // SWGANH_WEATHER_WEATHER_SERVICE_H_