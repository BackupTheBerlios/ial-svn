/***************************************************************************
 *
 * Initial.cs - A Graphical Frontend for the Input Abstraction Layer Daemon
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
using System.Diagnostics;
using System.IO;
using System.Xml;
using System.Xml.Serialization;

namespace Initial
{
    public delegate void InputEventHandler (string sender, string source, string name, int raw);

    public class InitialConfiguration
    {
        private ArrayList event_actions;
        public bool AskOnExit;

        public static string HomeDirectory
        {
            get {
                return System.IO.Path.Combine (System.Environment.GetEnvironmentVariable ("HOME"), "");
            }
        }
                
        public EventAction[] EventActions
        {
            get {
                EventAction[] ea = new EventAction[event_actions.Count];
                event_actions.CopyTo (ea);
                return ea;
            }
                        
            set {
                if (value == null) {
                    return;
                } else {
                    event_actions.Clear ();

                    foreach (EventAction ea in value)
                        event_actions.Add (ea);
                }
            }           
        }
                
        public int AddEventAction (EventAction ea)
        {
            int ret = event_actions.Add (ea);
            Initial.ui.PopulateTreeView ();
                
            return ret;
        }
                
        public void RemoveEventAction (EventAction ea)
        {
            event_actions.Remove (ea);
            Initial.ui.PopulateTreeView ();
        }

        public void Load ()
        {
            XmlSerializer s = new XmlSerializer ( typeof(InitialConfiguration) );
            TextReader r;
            
            try {
                r = new StreamReader (HomeDirectory + "/.initial");
                Initial.Config = (InitialConfiguration) s.Deserialize (r);
                r.Close ();      
            } catch (System.IO.FileNotFoundException e) {
                Initial.Config = new InitialConfiguration (); 
                Initial.Config.Save ();
            } catch (System.Exception e) {
                Console.WriteLine (e);
            }
        }
        
        public void Save ()
        {
            XmlSerializer s = new XmlSerializer ( typeof(InitialConfiguration) );
            TextWriter w;
            
            try {
                w = new StreamWriter (HomeDirectory + "/.initial");
                s.Serialize (w, Initial.Config);
                w.Close ();
            } catch (System.UnauthorizedAccessException e) {
                string warning =
                    String.Format (Mono.Posix.Catalog.GetString ("Could not save the configuration to disk. Please " +
                                                                 "check the persmissions of \"{0}/.initial\"."),
                                   HomeDirectory);

                MessageDialog md = new MessageDialog (null, DialogFlags.DestroyWithParent,
                                                      MessageType.Warning,
                                                      ButtonsType.Ok,
                                                      warning);
                md.Run ();
                md.Destroy ();
            } catch (System.Exception e) {
                Console.WriteLine (e);
            }
        }
            
        public InitialConfiguration ()
        {
            event_actions = new ArrayList ();
            AskOnExit = true;
        }               
    }
    
    public class EventAction
    {
        public InitialEvent  initial_event;
        public InitialAction initial_action;
            
        [XmlIgnore]
        public TreeIter iter;

        public bool isValid ()
        {
            if ((this.initial_action != null) && (this.initial_event != null)) {
                return true;
            } else {
                return false;
            }
        }

        public EventAction (InitialEvent e)
        {
            this.initial_action = null;
            this.initial_event  = e;
        }
        
        public EventAction (InitialAction a)
        {
            this.initial_action = a;
            this.initial_event  = null;
        }
        
        public EventAction (InitialEvent e, InitialAction a)
        {
            this.initial_action = a;
            this.initial_event  = e;                
        }
        
        public EventAction ()
        {
            this.initial_action = null;
            this.initial_event  = null;
        }
    }

    public class InitialEvent
    {
        private string sender;
        private string source;
        private string name;
        private int raw;

        public string Sender
        {
            get {
                return sender;
            }
            
            set {
                sender = value;
            }
        }

        public string Source
        {
            get {
                return this.source;
            }
            
            set {
                source = value;
            }
        }

        public string Name
        {
            get {
                return this.name;
            }
            
            set {
                name = value;
            }
        }

        public int Raw
        {
            get {
                return this.raw;
            }
            
            set {
                raw = value;
            }
        }

        public bool Equals (InitialEvent e)
        {
            if ( (this.sender == e.sender) && (this.source == e.source) &&
                 (this.name == e.name)     && (this.raw == e.raw) ) {
                return true;
            } else {
                return false;
            }
        }

        public InitialEvent ()
        {

        }

        public InitialEvent (string sender, string source, string name, int raw)
        {
            this.sender = sender;
            this.source = source;
            this.name = name;
            this.raw = raw;
        }
    }

    [XmlInclude ( typeof(ActionCommand) )]
    [XmlInclude ( typeof(ActionXKeysym) )]
    [XmlInclude ( typeof(ActionSystem)  )]
    public abstract class InitialAction
    {       
        public abstract void Trigger ();
    
        public InitialAction ()
        {
            
        }
    }
    
    public class ActionCommand : InitialAction
    {
        public string args;     
        public string command;

        public override void Trigger ()
        {
            Process p = new Process ();
            p.StartInfo.FileName        = command;
            p.StartInfo.Arguments       = args;
            p.StartInfo.UseShellExecute = false;
            try {
                    p.Start ();
            } catch (System.ComponentModel.Win32Exception e) {
                string warning =
                    String.Format (Mono.Posix.Catalog.GetString ("Could not execute the command \"{0}\". Please make " +
                                                                 "sure that the application is in your path."),
                                  command);

                MessageDialog md = new MessageDialog (null, DialogFlags.DestroyWithParent,
                                                      MessageType.Warning,
                                                      ButtonsType.Ok,
                                                      warning);
                md.Run ();
                md.Destroy ();
            }
        }

        public bool isValid ()
        {
            if ((command.Length > 0) && (command.IndexOf (" ") == -1)) {
                return true;
            } else {
                return false;
            }
        }
                            
        public ActionCommand (string command)
        {
            this.args    = null;
            this.command = command;
        }
                        
        public ActionCommand (string command, string args)
        {
            this.args    = args;
            this.command = command; 
        }
        
        public ActionCommand ()
        {
            this.args    = null;
            this.command = null;
        }
    }
    
    public class ActionXKeysym : InitialAction
    {
        public override void Trigger ()
        {
        
        }       

        public ActionXKeysym ()
        {
        
        }
    }
    
    public class ActionSystem : InitialAction
    {
        public override void Trigger ()
        {
        
        }       

        public ActionSystem ()
        {
        
        }
    }
    
    public class Initial
    {
        public static InitialConfiguration Config;
        public static UserInterface ui;    
        
        static void OnInputEvent (string sender, string source, string name, int raw)
        {
            InitialEvent e = new InitialEvent (sender, source, name, raw);
            
            foreach (EventAction ea in Initial.Config.EventActions) {
                if (ea.initial_event.Equals (e))
                    ea.initial_action.Trigger ();
            }
        }
        
        public Initial ()
        {
            Mono.Posix.Catalog.Init ("initial", Defines.LOCALE_DIR);

            Gtk.Application.Init ();

            InputManager inputmanager;

            inputmanager = InputManager.New ();
            if (inputmanager == null) {
                return;
            }
            
            inputmanager.InputEvent += new InputEventHandler (OnInputEvent);
            
            Config = new InitialConfiguration ();
            Config.Load ();

            ui = new UserInterface ();

            new Tray ();

            Gtk.Application.Run ();          
        }

        public static void Main(string[] args) 
        { 
            new Initial (); 
        } 
    }
}
