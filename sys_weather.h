#ifndef SYS_WEATHER_H
#define SYS_WEATHER_H

#include "entity.h"

enum { MAX_WEATHERS = 8192,
};

enum { VANE_X = 30,
       VANE_Y = 0,
       VANE_W = 20,
       VANE_H = 20,
};

/* Weather is a component that represents the weather of a region. */
struct Weather {
	Entity e;
	struct {
		float w, h, d;
	} volume;

	float windSpeed;
	float windAngle;
	float temp;
	float uv;
};

void InitWeatherSystem();
void UpdateWeatherSystem();
void AddWeather(Entity, float, float);
void RemoveWeather(Entity);

#endif
