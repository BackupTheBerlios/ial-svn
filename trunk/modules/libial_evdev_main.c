#include <unistd.h>
#include <fcntl.h>

#include "libial_evdev.h"

extern ModuleData mod_data;

IalEvent event;

evdev event_devices[MAX_EVENT_DEV];

struct input_event_s input_event;

gboolean evdev_callback(gpointer user_data)
{
    int fd = g_io_channel_unix_get_fd(user_data);
    char source[MAX_EVENT_DEV_NAME];
    
    read(fd, &input_event, sizeof(input_event));

    /* We only care about key presses. */
    if ((input_event.type == EV_KEY) && (input_event.value == 1)) {
        /* We do _not_ want to report all keys. Otherwise passwords and everything else would go to the bus*/
        if (key_blacklisted(input_event.code) == TRUE) {
            DEBUG(("Key blacklisted. Not sending to D-Bus."));
        }
        else {

            printf("\n\ntype %i code: %i value %i\n", input_event.type, input_event.code, input_event.value);
            /* TODO: move checks to event_send implementation */
            /* Get device name */
            if(ioctl(fd,EVIOCGNAME(sizeof(source)), source) == -1)
                strcpy(source, "Unknown device");


            /* Fill structure and send D-Bus message */
            event.sender = mod_data.token;
            event.source = source;
            event.name = key_to_string(input_event.code);
            event.raw = input_event.code;

            event_send(&event);
        }
    }

    return TRUE;
}

gboolean evdev_fd_init()
{
    evdev *evdev_current = NULL;
    char evdev_if[32];
    char source[MAX_EVENT_DEV_NAME];
    char phys[MAX_EVENT_DEV_NAME];
    int i, protocol_version, evdev_if_count = 0;

    for (i = 0; i < MAX_EVENT_DEV; i++) {

        evdev_current = &event_devices[i];
        
        snprintf(evdev_if, sizeof(evdev_if), "/dev/input/event%d", i);
        evdev_current->fd = open(evdev_if, O_RDONLY);

        if (evdev_current->fd > 0) {
            if (ioctl(evdev_current->fd, EVIOCGVERSION, &protocol_version) == -1) {
                ERROR(("Could not gather protocol version."));
                close(evdev_current->fd);
                return FALSE;
            }

            if (protocol_version != EV_VERSION) {
                ERROR(("Protocol version mismatch."));
                close(evdev_current->fd);
                return FALSE;
            }
            
            /* Valid event interface found */
            evdev_if_count++;
            evdev_current->io_channel = g_io_channel_unix_new(evdev_current->fd);
            evdev_current->watch = g_io_add_watch(evdev_current->io_channel, G_IO_IN | G_IO_ERR | G_IO_HUP,
                    (GIOFunc) evdev_callback, evdev_current->io_channel);

            if(ioctl(evdev_current->fd,EVIOCGNAME(sizeof(source)), source) == -1) {
                strcpy(source, "Unknown device");
            }

            if(ioctl(evdev_current->fd,EVIOCGPHYS(sizeof(phys)), phys) == -1) {
                strcpy(source, "Unknown physical address");
            }
            
            DEBUG(("Event interface for %s (%s) found.", source, phys));
        }
    }

    if (evdev_if_count > 0) {
        DEBUG(("Found %i event interface(s).", evdev_if_count));
        return TRUE;
    }
    else {
        DEBUG(("No event interface(s) (/dev/input/*) found."));
        return FALSE;
    }
}

gboolean libial_evdev_start()
{
    if (evdev_fd_init() == TRUE) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}
