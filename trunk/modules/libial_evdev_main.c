/***************************************************************************
 *
 * libial_evdev_main.c - Linux Event Interface Input Abstraction Layer Module
 *
 * SVN ID: $Id$
 *
 * Copyright (C) 2004, 2005 Timo Hoenig <thoenig@nouse.net>
 *
 * Licensed under the Academic Free License version 2.1
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 **************************************************************************/

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "libial_evdev.h"

extern ModuleData mod_data;

/**
 * @addtogroup IALMODEVDEV
 *
 * @{
 *
 */

IalEvent event;
evdev event_devices[MAX_EVENT_DEV];
struct Input_Event_s input_event;
struct stat stats;

/** Event interface observer callback function
 *
 * @param       none
 * @returns     TRUE on success, FALSE on error.
 */

gboolean
evdev_observer_callback ()
{
    struct stat new_stats;

    if (stat (DEV_INPUT, &new_stats) != 0) {
        DEBUG (("Error while accesing \"" DEV_INPUT "\"."));
        return FALSE;
    }

    if (stats.st_ctime == new_stats.st_ctime) {
        /* nothing has changed */
        return TRUE;
    } else {
        /* we either lost an input device or found a new one. */
        evdev_fd_init ();
        return FALSE;
    }
}

/** Event Interface callback function.
 *
 * @param       user_data Pointer to evdev_current->io_channel.
 * @returns     TRUE on success, FALSE on error.
 */

gboolean
evdev_callback (gpointer user_data)
{
    int fd = g_io_channel_unix_get_fd (user_data);
    char source[MAX_EVENT_DEV_NAME];

    if (read (fd, &input_event, sizeof (input_event)) == -1) {
        DEBUG (("Lost input device."));
        return FALSE;
    }

    /* We only care about key presses. */
    if ((input_event.type == EV_KEY) && (input_event.value == 1)) {
        /* We do _not_ want to report all keys. Otherwise passwords and everything else would go to the bus */
        if (key_blacklisted (input_event.code) == TRUE) {
            DEBUG (("Key blacklisted. Not sending to D-Bus."));
        } else {
            /* TODO: move checks to event_send implementation */
            /* Get device name */
            if (ioctl (fd, EVIOCGNAME (sizeof (source)), source) == -1)
                strcpy (source, "Unknown device");

            /* Fill structure and send D-Bus message */
            event.sender = mod_data.token;
            event.source = source;
            event.name = key_to_string (input_event.code);
            event.raw = input_event.code;

            event_send (&event);
        }
    }

    return TRUE;
}

/** Initialize Event Interfaces
 *
 * @returns TRUE on success, FALSE on error.
 */

gboolean
evdev_fd_init ()
{
    evdev *evdev_current = NULL;
    char evdev_if[32];
    char source[MAX_EVENT_DEV_NAME];
    char phys[MAX_EVENT_DEV_NAME];
    int i, protocol_version, evdev_if_count = 0;

    if (stat (DEV_INPUT, &stats) != 0) {
        DEBUG (("Error while accesing \"" DEV_INPUT "\"."));
        return FALSE;
    }

    for (i = 0; i < MAX_EVENT_DEV; i++) {

        evdev_current = &event_devices[i];

        /* remove old fds and watches if we're reinitializing because of a new device */
        if (evdev_current->fd)
            close (evdev_current->fd); 
        if (evdev_current->watch)
            g_source_remove (evdev_current->watch);

        snprintf (evdev_if, sizeof (evdev_if), DEV_INPUT_EVENT "%d", i);
        evdev_current->fd = open (evdev_if, O_RDONLY);

        if (evdev_current->fd > 0) {
            if (ioctl (evdev_current->fd, EVIOCGVERSION, &protocol_version)
                == -1) {
                WARNING (("Could not gather protocol version."));
                close (evdev_current->fd);
                return FALSE;
            }

            if (protocol_version != EV_VERSION) {
                WARNING (("Protocol version mismatch."));
                close (evdev_current->fd);
                return FALSE;
            }

            /* Valid event interface found */
            evdev_if_count++;
            evdev_current->io_channel = g_io_channel_unix_new (evdev_current->fd);

            evdev_current->watch =
                g_io_add_watch (evdev_current->io_channel,
                                G_IO_IN | G_IO_ERR, (GIOFunc) evdev_callback, evdev_current->io_channel);

            if (ioctl (evdev_current->fd, EVIOCGNAME (sizeof (source)), source) == -1) {
                strcpy (source, "Unknown device");
            }

            if (ioctl (evdev_current->fd, EVIOCGPHYS (sizeof (phys)), phys) == -1) {
                strcpy (source, "Unknown physical address");
            }

            DEBUG (("Event interface for %s (%s) found.", source, phys));
        }
    }

    g_timeout_add (1000, (GSourceFunc) evdev_observer_callback, NULL);

    if (evdev_if_count > 0) {
        DEBUG (("Found %i event interface(s).", evdev_if_count));
        return TRUE;
    } else {
        DEBUG (("No event interface(s) (" DEV_INPUT_EVENT "/*) found."));
        return TRUE;
    }
}

/** Linux Event Interface Input Abstraction Layer Module initialization.
 * 
 * @returns TRUE on success, FALSE on error.
 */
gboolean
libial_evdev_start ()
{
    if (evdev_fd_init () == TRUE) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/* @} */
