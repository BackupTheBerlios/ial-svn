/* libial_log.c - Input Abstraction Layer Logging
 *
 * Copyright (C) 2003       David Zeuthen, <david@fubar.dk>
 * Copyright (C) 2004, 2005 Timo Hoenig <thoenig@nouse.net>
 *                          All rights reserved
 *
 * Licensed under the Academic Free License version 2.1
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>
#include <unistd.h>

#include "libial_log.h"

/**
 * @defgroup IALLIBLOG Debug and Logging
 * @ingroup IALLIB 
 * @brief   Debug and logging facilities used by the IAL daemon and IAL modules.
 * @{
 */

static int priority;
static const char *file;
static int line;
static const char *function;
static int level;


/** Setup logging
 *
 * @param _priority     Log priority (LOGPRI_DEBUG, LOGPRI_INFO, LOGPRI_WARNING or LOGPRI_ERROR).
 * @param _file         File which invoked logging.
 * @param _line         Line number of `file`.
 * @param _function     Name of function which invoked logging.
 */
void log_setup(int _priority, const char *_file, int _line,
               const char *_function)
{
    priority = _priority;
    file = _file;
    line = _line;
    function = _function;
}

/** Set log level
 *
 * @param _level          Log priority
 */
void log_level_set(int _level)
{
    switch (_level) {
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

void log_logfile_set(const char *logfile)
{
    int logfd;

    if (!strcmp(logfile, "stdout")) {
        INFO(("Logging to \"standard out\"."));
        return;
    }
    else {

    }

    logfd =  open(logfile, O_WRONLY|O_CREAT|O_APPEND); 

    if (logfd < 0) {
        WARNING(("Could not write to logfile \"%s\". Logging output will be on \"stderr\".", logfile));
        return;
    }

    if (dup2(logfd, STDIN_FILENO) == -1) {
        WARNING(("Can not redirect stdin."));
    }
        
    if (dup2(logfd, STDOUT_FILENO) == -1) {
        WARNING(("Can not redirect stdout."));
    }
            
    if (dup2(logfd, STDERR_FILENO) == -1) {
        WARNING(("Can not redirect stderr."));
    }
}

/** Get log level
 *
 * @returns Current log level.
 */
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

    int logfd;

    if (priority > level)
        return;

    va_start(args, format);
    vsnprintf(buf, 512, format, args);

    switch (priority) {
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
        pri = "Error:  ";
    }

    fprintf(stderr, "%s %s:%d %s(): %s\n", pri, file, line, function,
            buf);

    va_end(args);
}

/** @} */
