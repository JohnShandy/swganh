import swgpy.app
import swgpy.utility
import swgpy.weather

service_mgr = kernel.service_manager()
weather_svc = service_mgr.weather_service()

weather_svc.set_scene_weather(swgpy.SCENE.CORELLIA, swgpy.WEATHER.HEAVYSTORM, swgpy.vector3(0.0, 0.0, 0.0))