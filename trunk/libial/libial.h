#include <glib.h>
#include <dbus/dbus.h>
#include <dbus/dbus-glib.h>

#include "libial_mod.h"
#include "libial_log.h"

#define IAL_DBUS_SERVICENAME "com.novell.Ial"

#define IAL_DBUS_PATH_EVENT  "/com/novell/Ial/Event"
#define IAL_DBUS_PATH_STATUS "/com/novell/Ial/Status"

#define IAL_DBUS_INTERFACE_EVENT  "com.novell.Ial.Event"
#define IAL_DBUS_INTERFACE_STATUS "com.novell.Ial.Status"

#define IAL_DBUS_SIGNAL_EVENT "event"


typedef struct IalEvent_s
{
    const char* sender;
    const char* source;
    const char* name;
    int raw;
}
IalEvent;


gboolean ial_dbus_connect(void);
IalEvent receive_event(DBusMessage *);
void send_event(IalEvent *);



