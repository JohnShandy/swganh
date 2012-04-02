#include "swganh/weather/weather_service_binding.h"

BOOST_PYTHON_MODULE(py_weather)
{
    docstring_options local_docstring_options(true, true, false);

    exportWeatherService();
}