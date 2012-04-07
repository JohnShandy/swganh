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

#ifndef SWGANH_WEATHER_WEATHER_DATA_H_
#define SWGANH_WEATHER_WEATHER_DATA_H_

#include <cstdint>
#include <glm/glm.hpp>

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

    enum Scene : uint32_t
    {
        CORELLIA = 1,
        DANTOOINE,
        DATHOMIR,
        ENDOR,
        LOK,
        NABOO,
        RORI,
        TALUS,
        TATOOINE,
        YAVIN4
    };

    class WeatherEvent
    {
    public:
        WeatherEvent(float seconds, Weather weather, glm::vec3 vector);

        float GetDuration();
        void SetDuration(float seconds);

        Weather GetWeatherType();
        void SetWeatherType(Weather weather);

        glm::vec3 GetCloudVector();
        void SetCloudVector(glm::vec3 vector);

        bool operator==(WeatherEvent other)
        {
            return weather_type == other.weather_type;
        }

    private:
        float duration;
        Weather weather_type;
        glm::vec3 cloud_vector;
    };

}} // namespace swganh::weather

#endif // SWGANH_WEATHER_WEATHER_DATA_H_