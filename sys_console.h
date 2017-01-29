#ifndef SYS_CONSOLE_H
#define SYS_CONSOLE_H

#include "entity.h"
#include <stdbool.h>
#include <time.h>

#include "sys_gui.h"

#define CONSOLE_BLINK_INTERVAL 0.10 /* seconds */
#define CONSOLE_PRINT_INTERVAL 0.05 /* seconds */

enum { MAX_CONSOLES = 1,
};

enum { CONSOLE_START_X = 0,
       CONSOLE_START_Y = 120,
       CONSOLE_WIDTH = GUI_WIDTH - CONSOLE_START_X,
       CONSOLE_HEIGHT = GUI_HEIGHT - CONSOLE_START_Y,
       CONSOLE_COLOR = 0x000000ff,

       CONSOLE_FONT_WIDTH = 8,
       CONSOLE_FONT_HEIGHT = 8,

       CONSOLE_NUM_ROWS = CONSOLE_HEIGHT / CONSOLE_FONT_HEIGHT,
       CONSOLE_CURSOR = '$',

       CONSOLE_MAX_LINES = 256,
       CONSOLE_HISTORY_SIZE = 10000,
};

struct Console {
	Entity e;
	unsigned col, row;
	unsigned numLines;
	unsigned scroll;

	Entity room;

	bool blink;
	double blinktmr;

	bool acceptInput;
	double addtmr_interval;
	double addtmr_start;
	double addtmr;
	char addBuff[256];

	int lines[CONSOLE_MAX_LINES];
	char text[CONSOLE_HISTORY_SIZE];
};

struct ConsoleUpdate {
	Entity e;
};

void InitConsoleSystem();
void UpdateConsoleSystem();
void AddConsole(Entity);

#endif
