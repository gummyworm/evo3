#include "draw.h"
#include "third-party/include/uthash.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <string.h>

#include "debug.h"
#include "systems.h"

/* err prints the error msg to the console. */
#define err(msg, ...)                                                          \
	{                                                                      \
		char buff[128];                                                \
		if (console != NULL) {                                         \
			sprintf(buff, msg, __VA_ARGS__);                       \
			ConsoleAddLine(console, buff);                         \
		}                                                              \
	}

/* msg prints msg to the console. */
#define msg(msg, ...)                                                          \
	{                                                                      \
		char buff[128];                                                \
		if (console != NULL) {                                         \
			sprintf(buff, msg, __VA_ARGS__);                       \
			ConsoleAddLine(console, buff);                         \
		}                                                              \
	}

/* error messages */
#define ERR_UNKNOWN_COMMAND "unknown command: %s"
#define ERR_NUM_ARGS "expected %d argument(s); %d given"
#define ERR_ITEM_NOT_FOUND "I don't have a %s"
#define ERR_ITEM_NOT_VISIBLE "I don't see a %s"
#define ERR_UNKNOWN_DIRECTION "You can't go %s"
#define ERR_NO_EXIT "There is no exit %s"
#define ERR_NO_TARGET "There is no target named %s"

/* info messages */
#define MSG_ITEM_DROPPED "Dropped %s"
#define MSG_ITEM_TAKEN "The %s is in hand"
#define MSG_ATTACK_TARGET "You attack %s"
#define MSG_WIELD "You wield the %s"
#define MSG_WEAR "You don the the %s"

/* command names */
const char CMD_LS[] = "LS";
const char CMD_DROP[] = "RM";
const char CMD_LOOK[] = "LOOK";
const char CMD_TAKE[] = "TAKE";
const char CMD_MOVE[] = "GO";
const char CMD_KILL[] = "KILL";
const char CMD_WIELD[] = "WIELD";
const char CMD_WEAR[] = "WEAR";

const char DIRECTION_NORTH[] = "NORTH";
const char DIRECTION_SOUTH[] = "SOUTH";

static time_t tmrstart;

struct entityToConsole {
	Entity e;
	struct Console *console;
	UT_hash_handle hh;
};

static struct entityToConsole *entitiesToConsoles;
static struct Console consoles[MAX_COMMANDERS];
static int numConsoles;

static int numUpdates;
static struct ConsoleUpdate updates[MAX_CONSOLES];

/* addChar adds the character ch to the console and updates the col/row etc.
 * accordingly. */
static void addChar(struct Console *console, int ch) {
	int r, c, l;

	c = console->col;
	r = console->row;
	l = console->numLines;

	switch (ch) {
	case GLFW_KEY_UP:
		if (console->scroll > 0)
			console->scroll--;
		break;

	case GLFW_KEY_DOWN:
		if (console->scroll <= (console->numLines - CONSOLE_NUM_ROWS))
			console->scroll++;
		break;

	case '\n':
	case GLFW_KEY_ENTER:
		console->lines[l + 1] = console->lines[l] + c;
		console->numLines++;
		console->col = 0;
		if (r < (CONSOLE_NUM_ROWS - 1))
			console->row++;
		else
			console->scroll++;
		break;

	case GLFW_KEY_BACKSPACE:
		if (c > 0) {
			*(console->text + console->lines[l] + c - 1) = '\0';
			console->col--;
		}
		break;

	default:
		if (c < (CONSOLE_WIDTH / CONSOLE_FONT_WIDTH)) {
			*(console->text + console->lines[l] + c) = ch;
			*(console->text + console->lines[l] + c + 1) = '\0';
			console->col++;
		}
		break;
	}
}

/* addLine adds text to the console. */
static void addLine(struct Console *console, char *text) {
	char c, *pch;
	int len;

	len = strlen(text);

	pch = text;
	while ((c = *pch++))
		addChar(console, *pch);
	addChar(console, '\n');
}

/* getargs parses line, sets all the arguments found, and returns the number. */
static int getargs(char *line, char **args) {
	char *p;
	int count;

	count = 0;
	for (p = strtok(line, " \t"); p != NULL; p = strtok(NULL, " \t")) {
		args[count] = p;
		count++;
	}

	return count;
}

/* look examines target and displays its description if it has one. */
static void look(char *name) {}

/* take adds name to the inventory (if it exists can be added). */
static void take(char *name) {}

/* inventory displays the contents of the inventory in console. */
static void inventory() {}

/* move moves the player to the exit matching dir. */
static void move(char *dir) {
	if (strcmp(dir, DIRECTION_NORTH) == 0) {
	} else if ((strcmp(dir, DIRECTION_SOUTH) == 0)) {
	} else {
	}
}

/* drop removes the item name from the inventory. */
static void drop(char *name) {}

/* kill engages target in combat and attempts to vanquish it. */
static void kill(char *target) {}

/* wield equips weapon. */
static void wield(char *weapon) {}

/* wear puts on attire. */
static void wear(char *attire) {}

/* exec executes line as a console command. */
static void exec(char *line) {
	int argc;
	char *argv[256];
	char l[256];

	strncpy(l, line, sizeof(l));
	argc = getargs(l, argv);

	if (strncmp(argv[0], CMD_LS, sizeof(CMD_LS)) == 0) {
		inventory();
	} else if (strncmp(argv[0], CMD_DROP, sizeof(CMD_DROP)) == 0) {
		if (argc != 2)
			;
		else
			drop(argv[1]);
	} else if (strncmp(argv[0], CMD_LOOK, sizeof(CMD_LOOK)) == 0) {
		if (argc != 2)
			;
		else
			look(argv[1]);
	} else if (strncmp(argv[0], CMD_TAKE, sizeof(CMD_TAKE)) == 0) {
		if (argc != 2)
			;
		else
			take(argv[1]);
	} else if (strncmp(argv[0], CMD_MOVE, sizeof(CMD_MOVE)) == 0) {
		if (argc != 2)
			;
		else
			move(argv[1]);
	} else if (strncmp(argv[0], CMD_KILL, sizeof(CMD_KILL)) == 0) {
		if (argc != 2)
			;
		else
			kill(argv[1]);
	} else if (strncmp(argv[0], CMD_WIELD, sizeof(CMD_WIELD)) == 0) {
		if (argc != 2)
			;
		else
			wield(argv[1]);
	} else if (strncmp(argv[0], CMD_WEAR, sizeof(CMD_WEAR)) == 0) {
		if (argc != 2)
			;
		else
			wear(argv[1]);
	} else {
		;
	}
}

/* button is called when a button is pressed. */
static void key(int key, int button, int action, int mods) {
	UNUSED(mods);

	struct Console *console;

	if (action != GLFW_PRESS)
		return;

	if (numConsoles < 1)
		return;
	console = consoles + 0;

	if (!console->acceptInput)
		return;

	tmrstart = time(NULL);
	console->blink = true;

	addChar(console, key);
	if (key == GLFW_KEY_DOWN)
		exec(console->text + console->lines[console->numLines]);
}

/* update updates and draws the console window. */
static void update(struct Console *console) {
	unsigned i, y, scroll;
	char buff[CONSOLE_WIDTH / CONSOLE_FONT_WIDTH + 2];
	mat4x4 mvp;

	if (console == NULL)
		return;

	/* blink the cursor */
	console->blinktmr = time(NULL);
	if (difftime(console->blinktmr, tmrstart) > CONSOLE_BLINK_INTERVAL) {
		console->blinktmr -= tmrstart;
		tmrstart = time(NULL);
		console->blink = !console->blink;
	}

	/* if we've moved to a new room, display its description */
	if (GetTransformUpdate(console->e) != NULL) {
		Entity room = GetRoom(console->e);
		if (room != console->room) {
			char *desc = (char *)GetDescription(room);
			if (desc != NULL) {
				addLine(console, desc);
			}
			console->room = room;
		}
	}

	/* draw the console */
	GuiProjection(&mvp);
	Rect(mvp, CONSOLE_START_X, CONSOLE_START_Y, CONSOLE_WIDTH,
	     CONSOLE_HEIGHT, CONSOLE_COLOR);

	y = CONSOLE_START_Y;
	scroll = console->scroll;
	for (i = 0; i <= (scroll + console->numLines); ++i) {
		int len, offset;

		if (y > (CONSOLE_START_Y + CONSOLE_HEIGHT))
			break;

		offset = console->lines[i + scroll];
		if (i + scroll == console->numLines)
			len = console->col;
		else
			len = console->lines[i + scroll + 1] - offset;

		memcpy(buff, console->text + offset, len);

		if (console->row == i && console->blink) {
			buff[len] = CONSOLE_CURSOR;
		} else {
			buff[len] = '\0';
		}
		buff[len + 1] = '\0';

		Text(mvp, CONSOLE_START_X, y, CONSOLE_FONT_WIDTH, buff);
		y += CONSOLE_FONT_HEIGHT;
	}
}

/* getConsole returns the console attached to entity e (if there is one). */
static struct Console *getConsole(Entity e) {
	struct entityToConsole *c;

	if (entitiesToConsoles == NULL)
		return NULL;

	HASH_FIND_INT(entitiesToConsoles, &e, c);
	if (c == NULL)
		return NULL;

	return c->console;
}

/* addUpdate adds a new update for this frame. */
static void addUpdate(struct ConsoleUpdate *u) { updates[numUpdates++] = *u; }

/* AddConsole adds a console component to the entity e. */
void AddConsole(Entity e) {
	struct entityToConsole *item;

	if (getConsole(e) != NULL)
		return;

	item = malloc(sizeof(struct entityToConsole));
	item->console = consoles + numConsoles;
	item->e = e;

	consoles[numConsoles].e = e;
	consoles[numConsoles].room = -1;
	consoles[numConsoles].numLines = 0;
	memset(consoles[numConsoles].text, '\0',
	       sizeof(consoles[numConsoles].text));
	memset(consoles[numConsoles].lines, 0,
	       sizeof(consoles[numConsoles].lines));
	consoles[numConsoles].acceptInput = true;

	HASH_ADD_INT(entitiesToConsoles, e, item);
	numConsoles++;
}

/* Initconsolesystem initializes the transform system. */
void InitConsoleSystem() { InputRegisterKeyEvent(INPUT_LAYER_CONSOLE, key); }

/* Updateconsolesystem updates all consoles that have been created. */
void UpdateConsoleSystem() {
	int i;

	for (i = 0; i < numConsoles; ++i)
		update(consoles + i);

	numUpdates = 0;
}
