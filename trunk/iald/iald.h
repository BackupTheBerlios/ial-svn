#include <dbus/dbus.h>
#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-lowlevel.h>

#include "../libial/libial.h"

/** defined in iald.c */
void opt_header(void);
void opt_usage(void);
void opt_list(void);
void opt_list_verbose(void);
void opt_modules_opts(char *);
void opt_version(void);
void opt_parse(int, char *[]);


int main(int, char *[]);
