#include "libial_keyboard.h"

#include <unistd.h>
#include <fcntl.h>

struct keyboard_s keyboard;
int log_event_watch;

gboolean log_fd_init()
{
    keyboard.log_fd = open(LOG_FILE, 0);
    if (keyboard.log_fd > 0)
    {
        keyboard.io_channel = g_io_channel_unix_new(keyboard.log_fd);
        return TRUE;
    }

    keyboard.log_fd = -1;

    return FALSE;
}

gboolean keyboard_event_callback()
{

}

gboolean libial_keyboard_start()
{
    if (log_fd_init() == TRUE)
    {
        log_event_watch =
            g_io_add_watch(keyboard.io_channel, G_IO_IN | G_IO_ERR | G_IO_HUP,
                           keyboard_event_callback, NULL);
        return TRUE;
    }

    return FALSE;
}
