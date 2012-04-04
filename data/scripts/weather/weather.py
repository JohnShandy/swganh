# swgpy
from swgpy import app, utility, weather

# modules
import random

service_mgr = kernel.service_manager()
weather_svc = service_mgr.weather_service()

# The durations are in seconds for quick testing with the swganh.exe console,
# but in normal use one would of course sepcify enough seconds such that,
# storm events last for minutes or hours of time.
lightStormSequence = weather.weather_sequence()
lightStormSequence[:] = [
	weather.weather_event(5.0, weather.WEATHER.CLOUDY, utility.vector3(0.0, 0.0, 0.0)),
	weather.weather_event(20.0, weather.WEATHER.LIGHTSTORM, utility.vector3(0.0, 0.0, 0.0)),
	weather.weather_event(5.0, weather.WEATHER.CLOUDY, utility.vector3(0.0, 0.0, 0.0)),
	weather.weather_event(0.0, weather.WEATHER.NOSTORM, utility.vector3(0.0, 0.0, 0.0))
]

mediumStormSequence = weather.weather_sequence()
mediumStormSequence[:] = [
	weather.weather_event(5.0, weather.WEATHER.CLOUDY, utility.vector3(0.0, 0.0, 0.0)),
	weather.weather_event(10.0, weather.WEATHER.LIGHTSTORM, utility.vector3(0.0, 0.0, 0.0)),
	weather.weather_event(20.0, weather.WEATHER.MEDIUMSTORM, utility.vector3(0.0, 0.0, 0.0)),
	weather.weather_event(10.0, weather.WEATHER.LIGHTSTORM, utility.vector3(0.0, 0.0, 0.0)),
	weather.weather_event(5.0, weather.WEATHER.CLOUDY, utility.vector3(0.0, 0.0, 0.0)),
	weather.weather_event(0.0, weather.WEATHER.NOSTORM, utility.vector3(0.0, 0.0, 0.0))
]

availableSequences = [
	lightStormSequence,
	mediumStormSequence
]

weather_svc.run_weather_sequence(weather.SCENE.CORELLIA, random.choice(availableSequences))