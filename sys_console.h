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

enum { CONSOLE_START_X = 8,
       CONSOLE_START_Y = 168,
       CONSOLE_WIDTH = GUI_WIDTH - CONSOLE_START_X - 8,
       CONSOLE_HEIGHT = GUI_HEIGHT - CONSOLE_START_Y - 8,

       CONSOLE_BSTART_X = 0,
       CONSOLE_BSTART_Y = 160,
       CONSOLE_BWIDTH = GUI_WIDTH - CONSOLE_BSTART_X,
       CONSOLE_BHEIGHT = GUI_HEIGHT - CONSOLE_BSTART_Y,
       CONSOLE_COLOR = 0x000000ff,

       CONSOLE_FONT_WIDTH = 4,
       CONSOLE_FONT_HEIGHT = 4,

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

	GLuint overlayTex, backgroundTex;

	int lines[CONSOLE_MAX_LINES];
	char text[CONSOLE_HISTORY_SIZE];
	char addBuff[1024];
};

struct ConsoleUpdate {
	Entity e;
};

void InitConsoleSystem(GLFWwindow *);
void UpdateConsoleSystem();
void AddConsole(Entity);
void RemoveConsole(Entity);

#endif
