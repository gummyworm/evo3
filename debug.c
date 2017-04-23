#ifdef DEBUG

#include "debug.h"
#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void exceptionhandler(int sig) {
	void *array[10];
	size_t size;

	size = backtrace(array, 10);
	derrorf("signal %d:", sig);
	backtrace_symbols_fd(array, size, STDERR_FILENO);
	exit(1);
}

void dinstallhandlers() {
	dinfof("exception handler installed");
	signal(SIGSEGV, exceptionhandler);
	signal(SIGFPE, exceptionhandler);
}

#endif
