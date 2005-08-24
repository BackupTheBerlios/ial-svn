/*************************************************************************** 
 *
 * iald_dbus.h - Input Abstraction Layer Daemon D-BUS Interface
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

gboolean          iald_dbus_setup (void); 
gboolean          iald_dbus_reconnect (gpointer);

DBusHandlerResult get_modules (DBusConnection *,
                               DBusMessage *);

DBusHandlerResult filter_function (DBusConnection *,
                                   DBusMessage *,
                                   void *);
