#include "libial.h"

/** Global D-Bus connection */
DBusConnection *dbus_connection;

gboolean ial_dbus_connect()
{
    DBusError dbus_error;

    if (dbus_connection == NULL)
    {
        dbus_connection_set_change_sigpipe(TRUE);
        dbus_error_init(&dbus_error);
        dbus_connection = dbus_bus_get(DBUS_BUS_SYSTEM, &dbus_error);

        if (dbus_connection == NULL)
        {   
            ERROR(("dbus_bus_get(): Error. (%s)", dbus_error.message));
            return FALSE;
        }
        else
        {
            INFO(("dbus_bus_get(): Success."));
            return TRUE;
        }
    }
    else
    {
        INFO(("Already connected to D-Bus."));
        return TRUE;
    }
}

void send_event(IalEvent *event)
{
    DBusMessage *dbus_message = NULL;
    DBusMessageIter dbus_it;
    char *signal_name = "event";
    char *dbus_path = IAL_DBUS_PATH_EVENT;
    char *dbus_interface = IAL_DBUS_INTERFACE_EVENT;
    
    if(dbus_connection == NULL)
    {
        ERROR(("No connection to D-Bus."));
        return;
    }

    dbus_message = dbus_message_new_signal(dbus_path, dbus_interface, signal_name);

    if(dbus_message == NULL)
    {
        ERROR(("Not enough memory."));
        return;
    }

    dbus_message_iter_init(dbus_message, &dbus_it);
    dbus_message_iter_append_string(&dbus_it, event->name);

    if (!dbus_connection_send(dbus_connection, dbus_message, NULL))
    {
        ERROR(("Sending message failed."));
        return;
    }
    else
    {
        DEBUG(("Message sent."));
    }

    dbus_message_unref(dbus_message);
    dbus_connection_flush(dbus_connection);
}

