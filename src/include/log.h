#ifndef _LOG_H
#define _LOG_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>

extern int restart_gl_log();
extern int gl_log(const char* message, ...);
extern int gl_log_err (const char* message, ...);

#endif