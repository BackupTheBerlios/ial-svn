/* Copyright (C) 2003 David Zeuthen, <david@fubar.dk>
 * Licensed under the Academic Free License version 2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "libial_log.h"

/**
 * @defgroup HalDaemonLogging Logging system
 * @ingroup HalDaemon
 * @brief Logging system for the HAL daemon
 * @{
 */

static int priority;
static const char *file;
static int line;
static const char *function;
static int level;

/** Setup logging entry
 *
 *  @param  priority            Logging priority, one of HAL_LOGPRI_*
 *  @param  file                Name of file where the log entry originated
 *  @param  line                Line number of file
 *  @param  function            Name of function
 */
void log_setup(int _priority, const char *_file, int _line,
               const char *_function)
{
    priority = _priority;
    file = _file;
    line = _line;
    function = _function;
}

void log_level_set(int _level)
{
    switch (_level)
    {
    case LOGPRI_DEBUG:
    case LOGPRI_INFO:
    case LOGPRI_WARNING:
    case LOGPRI_ERROR:
        level = _level;
        break;

    default:
        ERROR(("Log level violation (must be in range of 0..3). Setting default log level (0)."));
        level = 0;
    }
}

int log_level_get()
{
    return level;
}

/** Emit logging entry
 *
 *  @param  format              Message format string, printf style
 *  @param  ...                 Parameters for message, printf style
 */
void log_output(const char *format, ...)
{
    va_list args;
    char buf[512];
    char *pri;

    if (priority > level)
        return;

    va_start(args, format);
    vsnprintf(buf, 512, format, args);

    switch (priority)
    {
    case LOGPRI_DEBUG:
        pri = "Debug:  ";
        break;
    case LOGPRI_INFO:
        pri = "Info:   ";
        break;
    case LOGPRI_WARNING:
        pri = "Warning:";
        break;
    default:                   /* explicit fallthrough */
    case LOGPRI_ERROR:
        pri = "Error:  ";
        break;
    }

    /** @todo Make programmatic interface to logging */
    fprintf(stderr, "%s %s:%d %s() : %s\n", pri, file, line, function, buf);

    va_end(args);
}

/** @} */
