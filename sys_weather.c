#include "draw.h"
#include "systems.h"
#include "third-party/include/uthash.h"

struct entityToWeather {
	Entity e;
	struct Weather *weather;
	UT_hash_handle hh;
};

static struct entityToWeather *entitiesToWeathers;
static struct Weather weathers[MAX_WEATHERS];
static int numWeathers;

static int numUpdates;
static struct WeatherUpdate updates[MAX_WEATHERS];

/* getWeather returns the weather attached to entity e (if there is one). */
static struct Weather *getWeather(Entity e) {
	struct entityToWeather *m;

	if (entitiesToWeathers == NULL)
		return NULL;

	HASH_FIND_INT(entitiesToWeathers, &e, m);
	if (m == NULL)
		return NULL;

	return m->weather;
}

/* addUpdate adds a new update for this frame. */
static void addUpdate(struct WeatherUpdate *u) { updates[numUpdates++] = *u; }

/* drawWeatherUI renders the GUI that displays weather information. */
static void drawWeatherUI(struct Weather *w) {
	static GLuint vane;
	mat4x4 proj;

	if (vane == 0) {
		if ((vane = GetTexture("res/vane.png")) == 0)
			return;
	}

	GuiProjection(proj);
	TexRectZRot(proj, getTextureProgram(), VANE_X, VANE_Y, 0.f, VANE_W,
	            VANE_H, 0.f, 0.f, 1.f, 1.f, w->windAngle, vane);
	dinfof("%f", w->windAngle);
}

/* InitWeatherSystem initializes the weather system. */
void InitWeatherSystem() {}

/* UpdateWeatherSystem updates all weathers that have been created. */
void UpdateWeatherSystem() {
	if (numWeathers <= 0)
		return;

	drawWeatherUI(weathers + 0);
	numUpdates = 0;
}

/* AddWeather adds a weather component to the entity e. */
void AddWeather(Entity e, float temp, float speed) {
	struct entityToWeather *item;

	if (getWeather(e) != NULL)
		return;

	item = malloc(sizeof(struct entityToWeather));
	item->weather = weathers + numWeathers;
	item->e = e;

	weathers[numWeathers].e = e;
	weathers[numWeathers].windSpeed = speed;
	weathers[numWeathers].windAngle = 10.f / 180.f * M_PI;
	weathers[numWeathers].temp = temp;
	weathers[numWeathers].uv = 5.5f;

	HASH_ADD_INT(entitiesToWeathers, e, item);
	numWeathers++;
}

/* RemoveWeather removes the weather attached to e from the Weather system.
 */
void RemoveWeather(Entity e) {
	struct entityToWeather *c;

	if (entitiesToWeathers == NULL)
		return;

	HASH_FIND_INT(entitiesToWeathers, &e, c);
	if (c != NULL) {
		struct Weather *sys = c->weather;
		int sz = (weathers + numWeathers) - sys;
		memmove(sys, sys + 1, sz);
		HASH_DEL(entitiesToWeathers, c);
		free(c);
	}
}

/* GetWeatherUpdates returns the weather updates this frame. */
struct WeatherUpdate *GetWeatherUpdates(int *num) {
	*num = numUpdates;
	return updates;
}