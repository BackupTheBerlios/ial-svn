/***************************************************************************
 * Copyright (C) 2003 David Zeuthen, <david@fubar.dk>
 * Licensed under the Academic Free License version 2.0
 *************************************************************************/

/* Log prios */
enum {
    LOGPRI_ERROR = 0,    /**< error */
    LOGPRI_WARNING = 1,  /**< warnings */
    LOGPRI_INFO = 2,     /**< informational level */
    LOGPRI_DEBUG = 3     /**< debug statements in code */
};

void log_setup(int priority, const char *file, int line,
               const char *function);

void log_output(const char *format, ...);
void log_level_set(int);
int log_level_get(void);

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

/** Error leve logging macro */
#define ERROR(expr) \
        do { \
            log_setup(LOGPRI_ERROR, __FILE__, __LINE__, __FUNCTION__); \
            log_output expr; \
        } while(0)

/** Macro for terminating the program on an unrecoverable error */
//#define DIE(expr) do {printf("*** [TERMINATING] %s:%s():%d : ", __FILE__, __FUNCTION__, __LINE__); printf expr; printf("\n"); exit(1); } while(0)
