#include "libial_acpi.h"

/** Socket operations */
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

struct acpi_s acpi;
int acpi_event_watch;

gboolean acpi_event_fd_init()
{
    /* raw */
    acpi.event_fd = open(ACPI_EVENT, 0);
    if (acpi.event_fd >= 0)
    {
        acpi.io_channel = g_io_channel_unix_new(acpi.event_fd);
        return TRUE;
    }

    /* acpid */
    acpi.event_fd = socket(PF_UNIX, SOCK_STREAM, 0);

    if (acpi.event_fd >= 0)
    {
        struct sockaddr_un addr;
        addr.sun_family = AF_UNIX;
        strcpy(addr.sun_path, ACPID_SOCKET);
        if (connect(acpi.event_fd, (struct sockaddr *) &addr, sizeof(addr)) ==
            0)
        {
            acpi.io_channel = g_io_channel_unix_new(acpi.event_fd);
            return TRUE;
        }
    }

    acpi.event_fd = -1;

    return FALSE;
}

void acpi_event_handle(GString * event)
{
    if (strstr(event->str, "button"))
    {
        g_message("button event: %s", event->str);
        /* TODO send to dbus */
    }

}

gboolean acpi_event_callback(GIOChannel * chan, GIOCondition cond,
                             gpointer data)
{
    if (cond & (G_IO_ERR | G_IO_HUP))
    {
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
    else
    {
        gsize i;
        GString *event;
        GError *gerror = NULL;
        event = g_string_new(NULL);

        if (g_io_channel_read_line_string(acpi.io_channel, event, &i, &gerror)
            == G_IO_STATUS_NORMAL)
        {
            acpi_event_handle(event);
        }

    }

    return TRUE;
}

gboolean libial_acpi_start()
{
    if (acpi_event_fd_init() == TRUE)
    {
        acpi_event_watch =
            g_io_add_watch(acpi.io_channel, G_IO_IN | G_IO_ERR | G_IO_HUP,
                           acpi_event_callback, NULL);
        return TRUE;
    }

    return FALSE;
}
