#include "libial_event.h"

#include <unistd.h>
#include <fcntl.h>

struct event_s event;
int log_event_watch[MAX_EVENT_DEV];

gboolean event_callback(gpointer user_data)
{
    gsize i;
    GString *key_event;
    GError *gerror = NULL;
    key_event = g_string_new(NULL);
    if (g_io_channel_read_line_string
        ((GIOChannel *)user_data, key_event, &i, &gerror)
        == G_IO_STATUS_NORMAL) {
    }
    DEBUG((">> %i", g_io_channel_unix_get_fd (user_data)));
    g_string_free(key_event, TRUE);
    return TRUE;
}

add_io_channel(int i)
{
            event.io_channel[i] =
                g_io_channel_unix_new(event.fd[i]);

            log_event_watch[i] = g_io_add_watch(event.io_channel[i],
                                                G_IO_IN | G_IO_ERR | G_IO_HUP,
                                                (GIOFunc)
                                                event_callback,
                                                (gpointer)&event.io_channel[i]);
}


gboolean event_fd_init()
{
    char event_if[32];
    gpointer user_data;
    int i;

    for (i = 0; i < MAX_EVENT_DEV; i++) {
        snprintf(event_if, sizeof(event_if), "/dev/input/event%d", i);
        event.fd[i] = open(event_if, O_RDONLY);
        if (event.fd[i] > 0) {
            add_io_channel(i);

            DEBUG(("%s found.", event_if));
            DEBUG(("%i\n", log_event_watch[i]));
        }
        else {
            DEBUG(("%s not found.", event_if));
        }
    }

    return TRUE;
}

gboolean libial_event_start()
{
    if (event_fd_init() == TRUE) {
        return TRUE;
    }
    else
    {
        DEBUG(("No input devices (/dev/input/) found."));
        return FALSE;
    }
}
