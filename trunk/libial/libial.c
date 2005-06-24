/***************************************************************************
 * 
 * libial.c - Input Abstraction Layer Library
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

#include "libial.h"

/** Global D-Bus connection */
DBusConnection *dbus_connection;

/**
 * @defgroup IALLIB Input Abstraction Layer Library
 * @brief   The Input Abstraction Layer Daemon (libial) includes common function
 *          used by both the IAL daemon and IAL modules.
 *          
 * @{
 */

/**
 * @defgroup IALLIBDBUS D-BUS wrapper 
 * @ingroup IALLIB
 * @brief D-BUS wrapper functions hide the actual calls to D-BUS. Thus the IPC is
 *        transparent.
 *
 * @{
 */

/** Establish connection to the D-BUS daemon.
 * 
 * @returns     TRUE if connection could be established or if connection is already,
 *              established. FALSE if connection failed.
 */
gboolean
ial_dbus_connect ()
{
    DBusError dbus_error;

    if (dbus_connection == NULL) {
        dbus_connection_set_change_sigpipe (TRUE);
        dbus_error_init (&dbus_error);
        dbus_connection = dbus_bus_get (DBUS_BUS_SYSTEM, &dbus_error);

        if (dbus_connection == NULL) {
            ERROR (("dbus_bus_get(): Error. (%s)", dbus_error.message));
            return FALSE;
        } else {
            INFO (("dbus_bus_get(): Success."));
            return TRUE;
        }
    } else {
        INFO (("Already connected to D-Bus."));
        return TRUE;
    }
}

/* @} */

/**
 * @defgroup IALLIBEVENT Event Management
 * @ingroup IALLIB
 *
 * @{
 */

/** Receive an IAL Event (IalEvent) via D-BUS.
 *
 * @param dbus_message  Pointer to a DBusMessage.
 * @returns             IalEvent struct.
 */

IalEvent
event_receive (DBusMessage * dbus_message)
{
    IalEvent event;
    DBusError dbus_error;

    dbus_error_init (&dbus_error);

    dbus_message_get_args (dbus_message, &dbus_error,
                           DBUS_TYPE_STRING, &event.sender,
                           DBUS_TYPE_STRING, &event.source,
                           DBUS_TYPE_STRING, &event.name, DBUS_TYPE_INT32, &event.raw, DBUS_TYPE_INVALID);

    return event;
}

/** Send an IAL Event (IalEvent) via D-BUS.
 *
 * @param event      Pointer to an IalEvent.
 */

void
event_send (IalEvent * event)
{
    DBusMessage *dbus_message = NULL;
    DBusMessageIter dbus_it;
    const char *dbus_signal_name = IAL_DBUS_SIGNAL_EVENT;
    const char *dbus_path = IAL_DBUS_PATH_EVENT;
    const char *dbus_interface = IAL_DBUS_INTERFACE_EVENT;

    if (dbus_connection == NULL) {
        ERROR (("No connection to D-Bus."));
        return;
    }

    dbus_message = dbus_message_new_signal (dbus_path, dbus_interface, dbus_signal_name);

    if (dbus_message == NULL) {
        ERROR (("Not enough memory."));
        return;
    }

    /* TODO: Check if event is valid */

    dbus_message_iter_init_append (dbus_message, &dbus_it);

    dbus_message_iter_append_basic (&dbus_it, DBUS_TYPE_STRING, &(event->sender));
    dbus_message_iter_append_basic (&dbus_it, DBUS_TYPE_STRING, &(event->source));
    dbus_message_iter_append_basic (&dbus_it, DBUS_TYPE_STRING, &(event->name));
    dbus_message_iter_append_basic (&dbus_it, DBUS_TYPE_INT32, &(event->raw));

    if (!dbus_connection_send (dbus_connection, dbus_message, NULL)) {
        ERROR (("Sending message failed."));
        return;
    } else {
        DEBUG (("Sending IAL Event: %s (Sender=%s, Source=%s, Raw=0x%x).",
                event->name, event->sender, event->source, event->raw));
    }

    dbus_message_unref (dbus_message);
    dbus_connection_flush (dbus_connection);
}

/* @} */
/* @} */
