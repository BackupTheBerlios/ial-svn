#include "ialmon.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

extern DBusConnection *dbus_connection;

static DBusHandlerResult event_callback(DBusConnection * connection,
                                        DBusMessage * dbus_message,
                                        void *user_data)
{
    IalEvent event;
    int message_type;

    message_type = dbus_message_get_type(dbus_message);

    if (dbus_message_is_signal
        (dbus_message, IAL_DBUS_INTERFACE_EVENT, IAL_DBUS_SIGNAL_EVENT)) {
        event = event_receive(dbus_message);

        /* ACPI, any button, quick compare on raw */
        if(event.raw == -1) {
            open("/proc/latency_event_received", O_RDONLY);
            open("/proc/latency_get_data", O_RDONLY);
        }
            
        /* Toshiba ACPI, Fn-F1 */
        if(event.raw == 0x13b) {
            open("/proc/latency_event_received", O_RDONLY);
            open("/proc/latency_get_data", O_RDONLY);
        }

        /* Input Event Interface, F11 */
        if(event.raw == 0x57) {
            open("/proc/latency_event_received", O_RDONLY);
            open("/proc/latency_get_data", O_RDONLY);
        }
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
