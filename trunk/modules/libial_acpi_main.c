#include "libial_acpi.h"

/** Socket operations */
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

extern ModuleOption mod_options[];
extern ModuleData mod_data;

struct acpi_s acpi;
int acpi_event_watch;

void acpi_event_send(char *button)
{
    IalEvent event;

    event.sender = mod_data.token;
    event.source = ACPI_EVENT;
    event.name = button;
    event.raw = -1;

    event_send(&event);
}

gboolean acpi_event_fd_init()
{
    /* raw */
    acpi.event_fd = open(ACPI_EVENT, 0);
    if (acpi.event_fd >= 0) {
        acpi.io_channel = g_io_channel_unix_new(acpi.event_fd);
        return TRUE;
    }

    /* acpid */
    acpi.event_fd = socket(PF_UNIX, SOCK_STREAM, 0);

    if (acpi.event_fd >= 0) {
        struct sockaddr_un addr;
        addr.sun_family = AF_UNIX;
        strcpy(addr.sun_path, ACPID_SOCKET);
        if (connect(acpi.event_fd, (struct sockaddr *) &addr, sizeof(addr)) ==
            0) {
            acpi.io_channel = g_io_channel_unix_new(acpi.event_fd);
            return TRUE;
        }
    }

    acpi.event_fd = -1;

    return FALSE;
}

int acpi_lid_state()
{
    FILE *fd;
    char buffer[MAX_BUF];

    fd = fopen(ACPI_LID, "r");

    if (fd) {
        fscanf(fd, "%[^\"]", buffer);
        fclose(fd);

        if (strstr(buffer, "open")) {
            return ACPI_LID_STATE_OPEN;
        }
        else if (strstr(buffer, "closed")) {
            return ACPI_LID_STATE_CLOSED;
        }
        else {
            return ACPI_LID_STATE_ERROR;
        }
    }
    else {
        WARNING(("Could not open %s.", ACPI_LID));
        return ACPI_LID_STATE_ERROR;
    }
}

void acpi_event_handle(GString * acpi_event)
{
    if (strstr(acpi_event->str, "button")) {

        if (strstr(acpi_event->str, "PWRF")) {
            /* Power Button */
            acpi_event_send("Power Button");
        }
        else if (strstr(acpi_event->str, "SLPF")) {
            /* Sleep Button */
            acpi_event_send("Sleep Button");
        }
        else if (strstr(acpi_event->str, "LID")) {
            /* Lid Button */

            /** Determine lid state and send event */
            switch (acpi_lid_state()) {
            case ACPI_LID_STATE_OPEN:
                acpi_event_send("Lid Switch (open)");
                break;

            case ACPI_LID_STATE_CLOSED:
                acpi_event_send("Lid Switch (close)");
                break;

            case ACPI_LID_STATE_ERROR:
                break;

            default:
                break;
            }
        }
        else {
            WARNING(("Unknown button event received."));
            acpi_event_send(acpi_event->str);
        }
    }

}

gboolean acpi_event_callback(GIOChannel * chan, GIOCondition cond,
                             gpointer data)
{
    if (cond & (G_IO_ERR | G_IO_HUP)) {
        /* some exception handling TODO */

        /* does this lead to some race? 
         *
         * it is nice to restart the module since:
         * - if we were listening on the socket of acpid and acpid shutsdown
         *   we are reinitializing the module and get the events from the
         *   /proc/acpi/event.
         */

        libial_acpi_start();
        return FALSE;
    }
    else {
        gsize i;
        GString *acpi_event;
        GError *gerror = NULL;
        acpi_event = g_string_new(NULL);

        if (g_io_channel_read_line_string
            (acpi.io_channel, acpi_event, &i, &gerror)
            == G_IO_STATUS_NORMAL) {

            acpi_event_handle(acpi_event);
        }

        g_string_free(acpi_event, TRUE);

    }

    return TRUE;
}

gboolean libial_acpi_start()
{
    if (acpi_event_fd_init() == TRUE) {
        acpi_event_watch =
            g_io_add_watch(acpi.io_channel, G_IO_IN | G_IO_ERR | G_IO_HUP,
                           acpi_event_callback, NULL);
        return TRUE;
    }

    return FALSE;
}
