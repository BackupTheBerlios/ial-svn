/*************************************************************************** 
 *
 * iald_dbus.c - Input Abstraction Layer Daemon D-BUS Interface
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

#include "iald.h"
#include "iald_mod.h"
#include "iald_dbus.h"
#include "string.h"

extern GMainLoop *loop;
extern DBusConnection *dbus_connection;
extern IalModule *modules_list_head;

gboolean
iald_dbus_setup ()
{
    DBusError dbus_error;

    if (ial_dbus_connect () == FALSE) {
        ERROR (("D-BUS connection failed."));
        return FALSE;
    }

    dbus_connection_set_exit_on_disconnect (dbus_connection, FALSE);
    dbus_connection_setup_with_g_main (dbus_connection, NULL);
    dbus_error_init (&dbus_error);

    /* TODO
     *
     * Evaluate if it makes sense to check whether dbus_bus_service_exists() == TRUE and abort if so..
     *
     */

    dbus_bus_request_name (dbus_connection, IAL_DBUS_SERVICENAME, 0, &dbus_error);

    if (dbus_error_is_set (&dbus_error)) {
        ERROR (("dbus_bus_request_name(): Error. (%s)", dbus_error.message));
        return FALSE;
    } else {
        INFO (("dbus_bus_request_name(): Success. (%s)", IAL_DBUS_SERVICENAME));
    }

    dbus_connection_add_filter (dbus_connection, filter_function, loop, NULL);

    dbus_bus_add_match (dbus_connection,
                        "type='signal',"
                        "interface='" IAL_DBUS_INTERFACE_EVENT "'," "path='" IAL_DBUS_PATH_EVENT "',", NULL);

    return TRUE;
}

gboolean
iald_dbus_reconnect (gpointer user_data)
{
    if (iald_dbus_setup () == FALSE)
        return TRUE;
    return FALSE;
}

DBusHandlerResult
get_modules (DBusConnection *connection, DBusMessage *message)
{
    DBusMessage *reply;
    DBusMessageIter it_message;
    DBusMessageIter it_array;
    DBusMessageIter it_module;

    IalModule *m = NULL;
    const char *true = "true";
    const char *false = "false";

    reply = dbus_message_new_method_return (message);
    if (reply == NULL) {
        ERROR (("No memory."));
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }

    dbus_message_iter_init_append (reply, &it_message);
    m = modules_list_head;

    dbus_message_iter_open_container (&it_message,
                                      DBUS_TYPE_ARRAY, DBUS_TYPE_ARRAY_AS_STRING DBUS_TYPE_STRING_AS_STRING, &it_array);

    while (m) {
        dbus_message_iter_open_container (&it_array,
                                          DBUS_TYPE_ARRAY,
                                          DBUS_TYPE_STRING_AS_STRING DBUS_TYPE_INT32_AS_STRING, &it_module);

        dbus_message_iter_append_basic (&it_module, DBUS_TYPE_STRING, &(m->data->name));
        dbus_message_iter_append_basic (&it_module, DBUS_TYPE_STRING, &(m->data->author));
        dbus_message_iter_append_basic (&it_module, DBUS_TYPE_STRING, m->data->initialized ? &true : &false);
        dbus_message_iter_append_basic (&it_module, DBUS_TYPE_STRING, m->data->state ? &true : &false);
        dbus_message_iter_close_container (&it_array, &it_module);

        m = m->next;
    }

    dbus_message_iter_close_container (&it_message, &it_array);

    if (!dbus_connection_send (connection, reply, NULL)) {
        ERROR (("No memory."));
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }

    dbus_message_unref (reply);

    return DBUS_HANDLER_RESULT_HANDLED;
}

DBusHandlerResult
filter_function (DBusConnection *connection, DBusMessage *message, void *user_data)
{
    if (dbus_message_is_signal (message,
                                DBUS_INTERFACE_LOCAL,
                                "Disconnected")) {
        DEBUG (("Connection to D-BUS system bus lost. Trying to reconnect."));
        dbus_connection_unref (dbus_connection);
        dbus_connection = NULL;

        g_timeout_add (1000, (GSourceFunc) iald_dbus_reconnect, NULL);
        return DBUS_HANDLER_RESULT_HANDLED;
    }

    if ((dbus_message_is_method_call (message,
                                      "com.novell.Ial",
                                      "GetModules")) &&
        (strcmp (dbus_message_get_path (message), "/com/novell/Ial")) == 0) {

        return get_modules (connection, message);
    } else {
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }

    return DBUS_HANDLER_RESULT_HANDLED;
}
