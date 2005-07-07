/***************************************************************************
 *
 * Initial_dbus.cs - D-BUS Glue for Initial
 *
 * SVN ID: $Id$
 *
 * Copyright (C) 2005 Timo Hoenig <thoenig@nouse.net>
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

using DBus;
using Gtk;
using System;
using System.Collections;

namespace Initial
{
    [Interface ("com.novell.Ial")]
    public abstract class InputManager
    {
        [Signal]
        public event InputEventHandler InputEvent;

        [Method]
        public abstract string[][] GetModules ();

        static public InputManager New ()
        {
            DBus.Connection connection;
            DBus.Service service;
            InputManager manager;

            try {
                connection = DBus.Bus.GetSystemBus ();
            } catch (DBus.DBusException  e)  {
                System.Console.WriteLine (e.Message);
 
                string warning =
                    Mono.Posix.Catalog.GetString ("Could not connect to the D-BUS system bus. Please make sure that " +
                                                  "the D-BUS daemon is running.");


                MessageDialog md = new MessageDialog (null, DialogFlags.DestroyWithParent, 
                                                      MessageType.Error, 
                                                      ButtonsType.Ok,
                                                      warning);
                md.Run (); 
                md.Destroy ();

                return null;
            }

            try {
                service = DBus.Service.Get (connection, "com.novell.Ial");
            } catch (ApplicationException e)  {
                System.Console.WriteLine (e.Message);
 
                string warning = 
                    Mono.Posix.Catalog.GetString ("Could not aquire the Input Abstraction Layer interface. Please " +
                                                  "make sure that the Input Abstraction Layer daemon is running.");

                MessageDialog md = new MessageDialog (null, DialogFlags.DestroyWithParent, 
                                                      MessageType.Error, 
                                                      ButtonsType.Ok,
                                                      warning);

                md.Run (); 
                md.Destroy ();

                return null;
            }

            manager = (InputManager) service.GetObject (typeof (InputManager), "/com/novell/Ial");

            if (manager == null)
                return null;

            return manager;
        }
    }
}
