import swgpy.app
import swgpy.weather

service_mgr = kernel.service_manager()
weather_svc = service_mgr.weather_service()

def enum(**enums):
	return type('Enum', (), enums)
	
SCENE = enum(
			CORELLIA = 1,
			DANTOOINE = 2,
			DATHOMIR = 3,
			ENDOR = 4,
			LOK = 5,
			NABOO = 6,
			RORI = 7,
			TALUS = 8,
			TATOOINE = 9,
			YAVIN4 = 10
		)

weather_svc.set_scene_weather(SCENE.CORELLIA, WEATHER.HEAVYSTORM, (0.0, 0.0, 0.0))