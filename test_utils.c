#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>

#include "test_utils.h"

_Bool verbose = 0;

void verbose_start(char const * func_name) {
	if (verbose) {
		printf("%s...", func_name);
	}
}

void verbose_end(int nerrors) {
	if (verbose) {
		printf("%s with %d errors.\n", nerrors ? "failed" : "passed", nerrors);
	}
}

int check(_Bool result, char const * failmsg, ...) {
    if (!result) {
        va_list args;
        va_start(args, failmsg);
        vprintf(failmsg, args);
        va_end(args);
        return 1;
    }
    return 0;
}

