#ifndef SWGANH_WEATHER_WEATHER_SERVICE_BINDING_H_
#define SWGANH_WEATHER_WEATHER_SERVICE_BINDING_H_

#include "anh/python_shared_ptr.h"
#include "weather_service.h"

using namespace swganh::weather;
using namespace boost::python;
using namespace std;

void exportWeatherService()
{
	enum_<Weather>("WEATHER", "Weather types available in :class:`.WeatherService`")
		.value("NOSTORM", NOSTORM)
        .value("CLOUDY", CLOUDY)
        .value("LIGHTSTORM", LIGHTSTORM)
        .value("MEDIUMSTORM", MEDIUMSTORM)
        .value("HEAVYSTORM", HEAVYSTORM)
		;

    enum_<Scene>("SCENE", "Scenes available in :class:`.WeatherService`")
        .value("CORELLIA", CORELLIA)
        .value("DANTOOINE", DANTOOINE)
        .value("DATHOMIR", DATHOMIR)
        .value("ENDOR", ENDOR)
        .value("LOK", LOK)
        .value("NABOO", NABOO)
        .value("RORI", RORI)
        .value("TALUS", TALUS)
        .value("TATOOINE", TATOOINE)
        .value("YAVIN4", YAVIN4)
        ;

    class_<WeatherService, shared_ptr<WeatherService>, boost::noncopyable>("WeatherService", "The weather service processes in-game weather features.", no_init)
        .def("get_scene_weather", &WeatherService::GetSceneWeather, "Returns the current weather ID from a specified scene.")
        .def("set_scene_weather", &WeatherService::SetSceneWeather, "Sets the weather ID on a specified scene and broadcasts a ServerWeatherMessage to all players on that scene.")
        ;
}

#endif // SWGANH_WEATHER_WEATHER_SERVICE_BINDING_H_