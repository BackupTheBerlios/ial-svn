/* iald_dbus.c - Input Abstraction Layer Daemon
 *
 * Copyright (C) 2004, 2005 Timo Hoenig <thoenig@nouse.net>
 *                          All rights reserved
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
 */

#include "iald.h"
#include "iald_mod.h"
#include "iald_dbus.h"

extern GMainLoop *loop;
extern DBusConnection *dbus_connection;
extern IalModule *modules_list_head;

DBusHandlerResult get_modules(DBusConnection * connection,
                              DBusMessage * message)
{
    DBusMessage *reply;
    DBusMessageIter iter, iter1, iter2;
    
    IalModule *m = NULL;

    reply = dbus_message_new_method_return(message);

    if (reply == NULL) {
        ERROR(("No memory."));
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }


    dbus_message_iter_init(reply, &iter);
    dbus_message_iter_append_array(&iter, &iter1, DBUS_TYPE_ARRAY);
    m = modules_list_head;

    while (m) {
        dbus_message_iter_append_array(&iter1, &iter2, DBUS_TYPE_STRING);
        
        dbus_message_iter_append_string(&iter2, m->data->name);
        dbus_message_iter_append_string(&iter2, m->data->token);
        dbus_message_iter_append_string(&iter2, m->data->version); 
        dbus_message_iter_append_string(&iter2, m->data->author);
        dbus_message_iter_append_string(&iter2, m->data->descr);

        switch (m->data->type) {
        case INPUT:
            dbus_message_iter_append_string(&iter2, "0");
            break;
            
        case OUTPUT:
            dbus_message_iter_append_string(&iter2, "1");
            break;

        case INPUT_OUTPUT:
            dbus_message_iter_append_string(&iter2, "2");
            break;

        default:
            dbus_message_iter_append_string(&iter2, "-1");
            break;
        }

        if (m->data->initialized== TRUE)
            dbus_message_iter_append_string(&iter2, "true");
        else
            dbus_message_iter_append_string(&iter2, "false");

        if (m->data->state == TRUE)
            dbus_message_iter_append_string(&iter2, "true");
        else 
            dbus_message_iter_append_string(&iter2, "false");

        m = m->next;
    }

    if (!dbus_connection_send (connection, reply, NULL)) {
        ERROR(("No memory."));
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }

    dbus_message_unref(reply);
    
    return DBUS_HANDLER_RESULT_HANDLED;
}

DBusHandlerResult filter_function(DBusConnection * connection,
                                  DBusMessage * message,
                                  void *user_data)
{
    if (dbus_message_is_method_call(message, "com.novell.Ial", "GetModules") &&
        strcmp(dbus_message_get_path(message), "/com/novell/Ial") == 0) {
        
        return get_modules(connection, message);
    }
    else {
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }

    return DBUS_HANDLER_RESULT_HANDLED;
}


void iald_dbus_init(void)
{
    dbus_connection_add_filter(dbus_connection, filter_function, loop, NULL);

    dbus_bus_add_match(dbus_connection,
                       "type='signal',"
                       "interface='"IAL_DBUS_INTERFACE_EVENT"',"
                       "path='"IAL_DBUS_PATH_EVENT"',",
                       NULL);
}
