#define MODULE_NAME "ACPI Input Abstraction Layer Module"
#define MODULE_TOKEN "acpi"
#define MODULE_VERSION "0.0.1"
#define MODULE_AUTHOR "Timo Hoenig <thoenig@nouse.net>"
#define MODULE_DESCR "This module reports ACPI button events."

#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* TODO clean this include */
#include "../libial/libial.h"

/** libial_acpi_main.c */
gboolean acpi_event_fd_init(void);
void acpi_event_handle(GString *);
gboolean acpi_event_callback(GIOChannel *, GIOCondition, gpointer);
gboolean acpi_socket(void);
gboolean acpi_raw(void);
gboolean libial_acpi_start(void);

/** libial_acpi.c */
gboolean mod_load(void);
gboolean mod_unload(void);

/** ACPI interface */
#define ACPI_EVENT              "/proc/acpi/event"
#define ACPID_SOCKET            "/var/run/acpid.socket"

struct acpi_s
{
    const char *event;
    GIOChannel *io_channel;

    int event_fd;
};
