#include "ialmon.h"

extern DBusConnection *dbus_connection;

static DBusHandlerResult event_callback(DBusConnection * connection,
                                        DBusMessage * dbus_message,
                                        void *user_data)
{
    IalEvent event;
    int message_type;

    message_type = dbus_message_get_type(dbus_message);

    switch (message_type) {
    case DBUS_MESSAGE_TYPE_METHOD_CALL:
        DEBUG(("DBUS_MESSAGE_TYPE_METHOD_CALL"));
        break;
    case DBUS_MESSAGE_TYPE_METHOD_RETURN:
        DEBUG(("DBUS_MESSAGE_TYPE_METHOD_RETURN"));
        break;
    case DBUS_MESSAGE_TYPE_ERROR:
        DEBUG(("DBUS_MESSAGE_TYPE_ERROR"));
        break;
    case DBUS_MESSAGE_TYPE_SIGNAL:
        DEBUG(("DBUS_MESSAGE_TYPE_SIGNAL"));
        break;
    case DBUS_MESSAGE_TYPE_INVALID:
        DEBUG(("DBUS_MESSAGE_TYPE_INVALID"));
        break;
    default:
        DEBUG(("DBUS_MESSAGE_TYPE_ .... UNKNOWN"));
        break;
    }

    DEBUG(("Path=%s, Interface=%s, Member=%s, Destination=%s, Sender=%s",
           dbus_message_get_path(dbus_message),
           dbus_message_get_interface(dbus_message),
           dbus_message_get_member(dbus_message),
           dbus_message_get_destination(dbus_message),
           dbus_message_get_sender(dbus_message)
          ));

    if (dbus_message_is_signal
        (dbus_message, IAL_DBUS_INTERFACE_EVENT, IAL_DBUS_SIGNAL_EVENT)) {
        event = event_receive(dbus_message);

        DEBUG(("Received IAL Event: %s (Sender=%s, Source=%s, Raw=0x%x).",
               event.name, event.sender, event.source, event.raw));
    }

    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

int main(int argc, char *argv[])
{
    GMainLoop *loop;
    DBusError dbus_error;

    loop = g_main_loop_new(NULL, FALSE);
    dbus_error_init(&dbus_error);

    log_level_set(3);

    if (ial_dbus_connect() == FALSE) {
        DEBUG(("D-Bus connection failed."));
        return 1;
    }
    else {
        DEBUG(("D-Bus connection established."));
    }

    dbus_connection_setup_with_g_main(dbus_connection, NULL);

    if (dbus_bus_service_exists
        (dbus_connection, IAL_DBUS_SERVICENAME, &dbus_error) == FALSE) {
        WARNING(("D-Bus service \"%s\" does not exist ('iald' not running?).",
                 IAL_DBUS_SERVICENAME));
    }
    if (dbus_error_is_set(&dbus_error)) {
        ERROR(("Error while looking for service."));
        return 1;
    }

    dbus_connection_add_filter(dbus_connection, event_callback, loop, NULL);

    dbus_bus_add_match(dbus_connection,
                       "type='signal',"
                       "interface='" IAL_DBUS_INTERFACE_EVENT "',"
                       "path='" IAL_DBUS_PATH_EVENT "',", &dbus_error);

    if (dbus_error_is_set(&dbus_error)) {
        ERROR(("Could not register signal handler."));
        return 1;
    }

    g_main_loop_run(loop);

    return 0;
}
