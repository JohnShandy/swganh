#ifndef SWGANH_WEATHER_WEATHER_SERVICE_BINDING_H_
#define SWGANH_WEATHER_WEATHER_SERVICE_BINDING_H_

#include <string>
#include <vector>
#include <glm/glm.hpp>

#include "anh/python_shared_ptr.h" // must be above any boost headers
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include "swganh/weather/weather_service.h"

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

    class_<WeatherEvent>("weather_event", "Contains the duration, weather type, and cloud vector for a weaather event.", init<float, Weather, glm::vec3>())
        .def("getDuration", &WeatherEvent::GetDuration, "Gets the duration.")
        .def("setDuration", &WeatherEvent::SetDuration, "Sets the duration.")
        .def("getWeatherType", &WeatherEvent::GetWeatherType, "Gets the weather type.")
        .def("setWeatherType", &WeatherEvent::SetWeatherType, "Sets the weather type.")
        .def("getCloudVector", &WeatherEvent::GetCloudVector, "Gets the cloud vector.")
        .def("setCloudVector", &WeatherEvent::SetCloudVector, "Sets the cloud vector.")
        .add_property("duration", &WeatherEvent::GetDuration, &WeatherEvent::SetDuration, "Gets and sets the duration.")
        .add_property("weather_type", &WeatherEvent::GetWeatherType, &WeatherEvent::SetWeatherType, "Gets and sets the weather type.")
        .add_property("cloud_vector", &WeatherEvent::GetCloudVector, &WeatherEvent::SetCloudVector, "Gets and sets the cloud vector.")
        ;

    class_<std::vector<WeatherEvent> >("weather_sequence", "A vector for WeatherEvent objects which contain duration, weather type, and cloud vector.")
        .def(vector_indexing_suite<std::vector<WeatherEvent>>())
        ;

    class_<WeatherService, shared_ptr<WeatherService>, boost::noncopyable>("WeatherService", "The weather service processes in-game weather features.", no_init)
        .def("get_scene_weather", &WeatherService::GetSceneWeather, "Returns the current weather ID from a specified scene.")
        .def("set_scene_weather", &WeatherService::SetSceneWeather, "Sets the weather ID on a specified scene and broadcasts a ServerWeatherMessage to all players on that scene.")
        .def("run_weather_sequence", &WeatherService::RunWeatherSequence, "Runs a weather_sequence on a specified scene.")
        ;
}

#endif // SWGANH_WEATHER_WEATHER_SERVICE_BINDING_H_