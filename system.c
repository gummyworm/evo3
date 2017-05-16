#include "system.h"
#include "systems.h"

/* uncomment to log system update's durations */
#define PROFILE

/* uncomment to log all system update's durations */
/* #define PROFILE_LEVEL_ALL */

/* the time an update must take before it is logged as a warning. */
#define PROFILE_WARN_THRESHOLD .01f

#ifndef PROFILE
#define update(sys) sys()
#else
#ifdef PROFILE_LEVEL_ALL
#define update(sys)                                                            \
	{                                                                      \
		float dt;                                                      \
		float t = glfwGetTime();                                       \
		sys();                                                         \
		dt = glfwGetTime() - t;                                        \
		if (dt > PROFILE_WARN_THRESHOLD)                               \
			dwarnf("%s: %f\n", #sys, glfwGetTime() - t);           \
		else                                                           \
			dinfof("%s: %f\n", #sys, glfwGetTime() - t);           \
	}
#else
#define update(sys)                                                            \
	{                                                                      \
		float dt;                                                      \
		float t = glfwGetTime();                                       \
		sys();                                                         \
		dt = glfwGetTime() - t;                                        \
		if (dt > PROFILE_WARN_THRESHOLD)                               \
			dwarnf("%s: %f\n", #sys, glfwGetTime() - t);           \
	}
#endif
#endif

/* InitSystems initializes all the systems. */
void InitSystems(struct GLFWwindow *win) {
	InitInput(win);
	InitAudioSystem();

	InitDamageSystem();
	InitConsoleSystem(win);
	InitUnitSystem();
	InitSpriteSystem();
	InitPartySystem();
	InitTimeSystem();
	InitTransformSystem();
	InitCommanderSystem();
	InitFPSControllerSystem();
	InitWeatherSystem();
	InitTileMapSystem();
	InitGunSystem();
	InitChildSystem();
	InitGunControllerSystem();

	InitWidgetSystem(win);
	InitCameraSystem(win);
}

/* UpdateSystems updates all the systems. */
void UpdateSystems() {
	update(UpdateTimeSystem);

	/* pre render updates */
	update(UpdateChildSystem);
	update(UpdateMovementSystem);
	update(UpdateTransformSystem);
	update(UpdateInputSystem);
	update(UpdateFPSControllerSystem);
	update(UpdateGunSystem);
	update(UpdateDamageSystem);
	update(UpdateUnitSystem);

	/* render */
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	update(UpdateCameraSystem);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	// update(UpdateSpriteSystem);
	update(UpdateWeatherSystem);
	update(UpdateGunSystem);

	/* post main render updates */
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	update(UpdateCommanderSystem);
	update(UpdateConsoleSystem);
	update(UpdateWidgetSystem);
	// update(UpdateLabelSystem);
}
