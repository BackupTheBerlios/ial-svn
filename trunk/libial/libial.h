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


typedef struct IalEvent_s
{
    char* name;
    int raw;
    
    ModuleData* mod;
}
IalEvent;


gboolean ial_dbus_connect(void);
void send_event(IalEvent *);



