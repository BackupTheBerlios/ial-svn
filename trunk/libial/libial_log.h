/***************************************************************************
 *
 * libial_log.h - Input Abstraction Layer Logging Facilities
 *
 * SVN ID: $Id$
 *
 * Copyright (C) 2003        David Zeuthen, <david@fubar.dk>
 * Copyright (C) 2004, 2005  Timo Hoenig <thoenig@nouse.net>
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
 **************************************************************************/

/* Log prios */
enum {
    LOGPRI_ERROR = 0,    /**< error */
    LOGPRI_WARNING = 1,  /**< warnings */
    LOGPRI_INFO = 2,     /**< informational level */
    LOGPRI_DEBUG = 3     /**< debug statements in code */
};

void log_setup (int priority, const char *file, int line, const char *function);

void log_logfile_set (const char *);
void log_level_set (int);
int log_level_get (void);

void log_output (const char *format, ...) __attribute__((__format__(__printf__,1,2)));

/** Debug information logging macro */
#define DEBUG(expr) \
        do { \
            log_setup(LOGPRI_DEBUG, __FILE__, __LINE__, __FUNCTION__); \
            log_output expr; \
        } while(0)

/** Information level logging macro */
#define INFO(expr) \
        do { \
            log_setup(LOGPRI_INFO, __FILE__, __LINE__, __FUNCTION__); \
            log_output expr; \
        } while(0)

/** Warning level logging macro */
#define WARNING(expr) \
        do { \
            log_setup(LOGPRI_WARNING, __FILE__, __LINE__, __FUNCTION__); \
            log_output expr; \
        } while(0)

/** Error level logging macro */
#define ERROR(expr) \
        do { \
            log_setup(LOGPRI_ERROR, __FILE__, __LINE__, __FUNCTION__); \
            log_output expr; \
        } while(0)

/** Macro for terminating the program on an unrecoverable error */
//#define DIE(expr) do {printf("*** [TERMINATING] %s:%s():%d : ", __FILE__, __FUNCTION__, __LINE__); printf expr; printf("\n"); exit(1); } while(0)
