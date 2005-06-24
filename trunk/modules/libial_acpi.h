#define MODULE_NAME "ACPI Input Abstraction Layer Module"
#define MODULE_TOKEN "acpi"
#define MODULE_VERSION "0.0.1"
#define MODULE_AUTHOR "Timo Hoenig <thoenig@nouse.net>"
#define MODULE_DESCR "This module reports ACPI button events."

/** ACPI interface */
#define ACPI_EVENT              "/proc/acpi/event"
#define ACPI_LID                "/proc/acpi/button/lid/LID/state"
#define ACPID_SOCKET            "/var/run/acpid.socket"

/** Lid states */
#define ACPI_LID_STATE_OPEN     1
#define ACPI_LID_STATE_CLOSED   0
#define ACPI_LID_STATE_ERROR    -1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libial/libial.h>

/** libial_acpi_main.c */
void acpi_event_send (char *);
gboolean acpi_event_fd_init (void);
int acpi_lid_state (void);
void acpi_event_handle (GString *);
gboolean acpi_event_callback (GIOChannel *, GIOCondition, gpointer);
gboolean acpi_socket (void);
gboolean acpi_raw (void);
gboolean libial_acpi_start (void);

/** libial_acpi.c */
gboolean mod_load (void);
gboolean mod_unload (void);

struct Acpi_s {
    const char *event;
    GIOChannel *io_channel;

    int event_fd;
};
