#include "libial_toshiba.h"

extern DBusConnection *dbus_connection;

static DBusHandlerResult event_callback(DBusConnection * connection, DBusMessage * dbus_message, void *user_data)
{
    IalEvent event;
    if (dbus_message_is_signal(dbus_message, IAL_DBUS_INTERFACE_EVENT, IAL_DBUS_SIGNAL_EVENT)) {
        event = event_receive(dbus_message);
        DEBUG(("Received IAL Event: %s (Sender=%s, Source=%s, Raw=0x%x).",
               event.name, event.sender, event.source, event.raw));
    }

    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

gboolean toshiba_add_filter()
{
    DBusError dbus_error;
    dbus_error_init(&dbus_error);

    dbus_connection_add_filter(dbus_connection, event_callback, NULL, NULL);

    dbus_bus_add_match(dbus_connection,
                       "type='signal',"
                       "interface='" IAL_DBUS_INTERFACE_EVENT "'," "path='" IAL_DBUS_PATH_EVENT "',", &dbus_error);

    if (dbus_error_is_set(&dbus_error)) {
        ERROR(("Could not register signal handler."));
        return FALSE;
    }

    return TRUE;
}
