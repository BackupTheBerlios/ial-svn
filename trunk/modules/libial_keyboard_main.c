#include "libial_keyboard.h"

#include <unistd.h>
#include <fcntl.h>

struct keyboard_s keyboard;
int log_event_watch[MAX_EVENT_DEV];

gboolean keyboard_event_callback(gpointer user_data)
{
    gsize i;
    GString *key_event;
    GError *gerror = NULL;
    key_event = g_string_new(NULL);
    if (g_io_channel_read_line_string
        (keyboard.io_channel[*(int *) user_data], key_event, &i, &gerror)
        == G_IO_STATUS_NORMAL) {
    }
    DEBUG((">> %i", *(int *) user_data));
    g_string_free(key_event, TRUE);
    return TRUE;
}

gboolean event_fd_init()
{
    char event_if[32];
    gpointer user_data;
    int i;

    for (i = 0; i < MAX_EVENT_DEV; i++) {

        user_data = &i;

        snprintf(event_if, sizeof(event_if), "/dev/input/event%d", i);

        keyboard.event_fd[i] = open(event_if, O_RDONLY);

        if (keyboard.event_fd[i] > 0) {
            DEBUG(("%s found.", event_if));
            keyboard.io_channel[i] =
                g_io_channel_unix_new(keyboard.event_fd[i]);

            log_event_watch[i] = g_io_add_watch(keyboard.io_channel[i],
                                                G_IO_IN | G_IO_ERR | G_IO_HUP,
                                                (GIOFunc)
                                                keyboard_event_callback,
                                                user_data);

            DEBUG(("%i\n", log_event_watch[i]));
        }
        else {
            DEBUG(("%s not found.", event_if));
        }
    }

    return TRUE;
}

gboolean libial_keyboard_start()
{
    if (event_fd_init() == TRUE) {
//        log_event_watch =
//            g_io_add_watch(keyboard.io_channel[4],
//                           G_IO_IN | G_IO_ERR | G_IO_HUP,
//                           keyboard_event_callback, NULL);
        return TRUE;
    }

    return FALSE;
}
