/***************************************************************************
 * 
 * libial.h - Input Abstraction Layer Library
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

#include <glib.h>

#include <dbus/dbus.h>
#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-lowlevel.h>

#include "libial_mod.h"
#include "libial_log.h"

#define IAL_LOG "stderr"

#define IAL_DBUS_SERVICENAME "com.novell.Ial"

#define IAL_DBUS_PATH_EVENT  "/com/novell/Ial"
#define IAL_DBUS_PATH_STATUS "/com/novell/Ial"

#define IAL_DBUS_INTERFACE_EVENT  "com.novell.Ial"
#define IAL_DBUS_INTERFACE_STATUS "com.novell.Ial"

#define IAL_DBUS_SIGNAL_EVENT "InputEvent"

typedef struct IalEvent_s {
    const char *sender;
    const char *source;
    const char *name;
    int raw;
} IalEvent;

gboolean ial_dbus_connect (void);
IalEvent event_receive (DBusMessage *);
void event_send (IalEvent *);
