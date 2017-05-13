#include "sys_time.h"
#include "base.h"

static float dt;
static float t;
static float lastUpdate;

/* InitTimeSystem begins the time system. */
void InitTimeSystem() {
	glfwSetTime(0.0);
	t = 0.f;
	dt = 0.f;
	lastUpdate = 0.f;
}

/* UpdateTimeSystem updates the time system. */
void UpdateTimeSystem() { SetTime(glfwGetTime()); }

/* GetTime returns the current time since the Time system was initialized. */
float GetTime() { return t; }

/* SetTime sets the game time to t. */
void SetTime(float set) {
	lastUpdate = t;
	t = set;
	dt = t - lastUpdate;
}

/* GetTimeDelta returns the time difference since the last update to the time
 * system. */
float GetTimeDelta() { return dt; }
