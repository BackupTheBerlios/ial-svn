#include "ialmon.h"
extern DBusConnection *dbus_connection;

static DBusHandlerResult event_callback(DBusConnection * connection,
                                        DBusMessage * message, void *user_data)
{
    int type;

    type = dbus_message_get_type(message);

    switch(type) {
                case DBUS_MESSAGE_TYPE_METHOD_CALL:  
                        DEBUG(("DBUS_MESSAGE_TYPE_METHOD_CALL\n"));
                        break;
                case DBUS_MESSAGE_TYPE_METHOD_RETURN:
                        DEBUG(("DBUS_MESSAGE_TYPE_METHOD_RETURN\n"));
                        break;
                case DBUS_MESSAGE_TYPE_ERROR:
                        DEBUG(("DBUS_MESSAGE_TYPE_ERROR\n"));
                        break;
                case DBUS_MESSAGE_TYPE_SIGNAL:
                        DEBUG(("DBUS_MESSAGE_TYPE_SIGNAL\n"));
                        break;                                                          
                case DBUS_MESSAGE_TYPE_INVALID:                                         
                        DEBUG(("DBUS_MESSAGE_TYPE_INVALID\n"));
                        break;
                default:
                        DEBUG(("DBUS_MESSAGE_TYPE_ .... UNKNOWN\n"));
                        break;
                }               
                                        
                DEBUG(("obj_path=%s interface=%s method=%s destination=%s sender=%s\n",
                                        dbus_message_get_path(message),
                                        dbus_message_get_interface(message),
                                        dbus_message_get_member(message),
                                        dbus_message_get_destination(message),
                                        dbus_message_get_sender(message)
                ));        

    return DBUS_HANDLER_RESULT_HANDLED;
}

int main(int argc, char *argv[])
{
    GMainLoop *loop;
    DBusError dbus_error;

    loop = g_main_loop_new(NULL, FALSE);
    dbus_error_init(&dbus_error);

    log_level_set(3);

    if (ial_dbus_connect() == FALSE) {
        DEBUG(("D-Bus Connection failed."));
        return 1;
    }
    else {
        DEBUG(("D-Bus Connection established."));
    }

    dbus_connection_setup_with_g_main(dbus_connection, NULL);

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
