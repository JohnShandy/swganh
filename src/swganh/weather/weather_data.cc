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

#include "swganh/weather/weather_data.h"

using namespace swganh::weather;

WeatherEvent::WeatherEvent(float seconds, Weather weather, glm::vec3 vector)
{
    duration = seconds;
    weather_type = weather;
    cloud_vector.x = vector.x;
    cloud_vector.y = vector.y;
    cloud_vector.z = vector.z;
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