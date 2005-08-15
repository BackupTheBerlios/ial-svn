/***************************************************************************
 *
 * Initial_ui.cs - User Interface for Initial
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
using Glade;
using System;
using System.Collections;
using System.IO;
using System.Runtime.InteropServices;
using System.Xml;
using System.Xml.Serialization;

namespace Initial
{
    public class Tray
    {
        public Tray ()
        {
            EventBox eb      = new EventBox ();
            Gdk.Pixbuf ticon = new Gdk.Pixbuf (null, "ial.png").ScaleSimple (24, 24, Gdk.InterpType.Nearest);
            
            eb.Add (new Image(ticon));
            eb.ButtonPressEvent += new ButtonPressEventHandler (this.OnImageClick);

            TrayIcon icon = new TrayIcon("Initial");
            icon.Add( eb);

            icon.ShowAll();
        }

        private void OnImageClick (object o, ButtonPressEventArgs args)
        {
            if (args.Event.Button == 1) {
                if (Initial.ui.Visible)
                    Initial.ui.Hide ();
                else
                    Initial.ui.Show ();
            } else if (args.Event.Button == 3) {
                Menu popupMenu = new Menu();
                
                ImageMenuItem menuPopup2;
                Image appimg2;
                    
                if (Initial.ui.Visible) {
                    menuPopup2 = new ImageMenuItem (Mono.Posix.Catalog.GetString ("H_ide"));
                    appimg2 = new Image (Stock.Remove, IconSize.Menu);
                } else {
                    menuPopup2 = new ImageMenuItem (Mono.Posix.Catalog.GetString ("_Show"));
                    appimg2 = new Image (Stock.Add, IconSize.Menu);
                }

                menuPopup2.Image = appimg2;
                popupMenu.Add (menuPopup2);

                ImageMenuItem menuPopup1 = new ImageMenuItem (Mono.Posix.Catalog.GetString ("_Quit"));
                Image appimg1 = new Image (Stock.Quit, IconSize.Menu);
                menuPopup1.Image = appimg1;
                popupMenu.Add (menuPopup1);

                menuPopup1.Activated += new EventHandler (this.OnPopupClickQuit);
                menuPopup2.Activated += new EventHandler (this.OnPopupClickShowHide);

                popupMenu.ShowAll ();
                
                popupMenu.Popup (null, null, null, IntPtr.Zero, args.Event.Button, args.Event.Time);
            }
        }

        private void OnPopupClickQuit (object o, EventArgs args)
        {
            Initial.ui.Quit ();
        }

        private void OnPopupClickShowHide (object o, EventArgs args)
        {
            if (Initial.ui.Visible)
                Initial.ui.Hide ();
            else
                Initial.ui.Show ();
        }
    }
        
    public class AssignActionDialog
    {
        [Glade.Widget] Gtk.Dialog dlg_assign_action;
            
        public AssignActionDialog (Gtk.Window wnd_main, InitialEvent initial_event)
        {
            Glade.XML ui = Glade.XML.FromAssembly ("initial.glade", "dlg_assign_action", "initial");
            ui.Autoconnect (this);

            dlg_assign_action.TransientFor = wnd_main;
            dlg_assign_action.WindowPosition = Gtk.WindowPosition.CenterOnParent;
        }
    }
                
    public class AddEventDialog
    {
        [Glade.Widget] Gtk.Window wnd_main;

        [Glade.Widget] Gtk.Dialog dlg_add_event;
        [Glade.Widget] Gtk.Dialog dlg_press_button;

        [Glade.Widget] Gtk.VBox dlg_add_event_vbox;

        [Glade.Widget] Gtk.Button btn_assign_action;
        [Glade.Widget] Gtk.Button btn_cancel;           

        [Glade.Widget] Gtk.Frame frm_application;
        [Glade.Widget] Gtk.Frame frm_xkeysym;
        [Glade.Widget] Gtk.Frame frm_dummy;
                        
        [Glade.Widget] Gtk.Label lbl_e_name_val;
        [Glade.Widget] Gtk.Label lbl_e_device_val;
        [Glade.Widget] Gtk.Label lbl_e_module_val;
        [Glade.Widget] Gtk.Label lbl_e_numeric_val;

        [Glade.Widget] Gtk.Entry entry_application_path_val;
        [Glade.Widget] Gtk.Entry entry_application_args_val;

        [Glade.Widget] Gtk.Button btn_capture_event;
        [Glade.Widget] Gtk.Button btn_save_action;

        [Glade.Widget] Gtk.Label lbl_user_information;

        private InputManager inputmanager;

        private EventAction ea;

        private ActionCommand acmd;
        private ActionXKeysym axkey;
        private ActionSystem  asys;
                
        void on_entry_application_path_val_changed (object o, EventArgs args)
        {
            ActionCommand ac = new ActionCommand (entry_application_path_val.Text,
                                                  entry_application_args_val.Text);               

            ea.initial_action = ac;

            if (ac.isValid ()) { 
                btn_save_action.Sensitive = true;
            } else {
                btn_save_action.Sensitive = false;
            }                       
        }

        void on_entry_application_args_val_changed (object o, EventArgs args)
        {                                                                                                         
            ActionCommand ac = new ActionCommand (entry_application_path_val.Text,
                                                  entry_application_args_val.Text);               

            ea.initial_action = ac;

            if (ac.isValid ()) { 
                btn_save_action.Sensitive = true;
            } else {
                btn_save_action.Sensitive = false;
            }
        }

        void on_dlg_add_event_delete_event (object o, EventArgs args)
        {
            inputmanager.InputEvent -= EventReceived;;
            dlg_add_event.Destroy ();
        }

        void on_dlg_press_button_close (object o, EventArgs args)
        {
            dlg_add_event_vbox.Sensitive = true;
            dlg_press_button.Destroy ();
        }
                
        void on_dlg_press_button_close ()
        {
            inputmanager.InputEvent -= EventReceived;
            dlg_add_event_vbox.Sensitive = true;
            dlg_press_button.Destroy ();
        }

        void on_btn_close_clicked (object o, EventArgs args)
        {               
            on_dlg_press_button_close ();
        }

        void on_btn_capture_event_clicked (object o, EventArgs args)
        {               
            Glade.XML ui = Glade.XML.FromAssembly ("initial.glade", "dlg_press_button", "initial");
            ui.Autoconnect (this);

            lbl_e_name_val.Text    = "not available";
            lbl_e_device_val.Text  = "not available";
            lbl_e_module_val.Text  = "not available";
            lbl_e_numeric_val.Text = "not available";
                        
            dlg_press_button.TransientFor   = dlg_add_event;
            dlg_press_button.WindowPosition = Gtk.WindowPosition.CenterOnParent;
            dlg_add_event_vbox.Sensitive    = false;

            inputmanager.InputEvent += new InputEventHandler (EventReceived);
        }
                
        void on_rb_application_clicked (object o, EventArgs args)
        {
            ea.initial_action = new ActionCommand (entry_application_path_val.Text,
            entry_application_args_val.Text);
                        
            frm_application.Visible = true;
            frm_xkeysym.Visible     = false;
            frm_dummy.Visible       = false;
        }
                
        void on_rb_xkeysym_clicked (object o, EventArgs args)
        {               
            ea.initial_action = new ActionXKeysym ();

            frm_application.Visible = false;
            frm_xkeysym.Visible     = true;
            frm_dummy.Visible       = false;                
        }

        void on_rb_dummy_clicked (object o, EventArgs args)
        {               
            ea.initial_action = new ActionSystem ();

            frm_application.Visible = false;
            frm_xkeysym.Visible     = false;
            frm_dummy.Visible       = true;         
        }
                
        void on_btn_browse_clicked (object o, EventArgs args)
        {
            CompatFileChooserDialog dlg_file_chooser =
                new CompatFileChooserDialog (Mono.Posix.Catalog.GetString ("Open"),
                                             wnd_main, 0);

            if (dlg_file_chooser.Run () == (int) ResponseType.Ok)
                entry_application_path_val.Text = dlg_file_chooser.Filename;

            dlg_file_chooser.Destroy ();
        }

        void on_btn_cancel_clicked (object o, EventArgs args)
        {
            dlg_add_event.Destroy ();
        }

        void on_btn_save_action_clicked (object o, EventArgs args)
        {
            Initial.Config.AddEventAction (ea);
            Initial.Config.Save ();
            
            dlg_add_event.Destroy ();
        }

        void EventReceived (string sender, string source, string name, int raw)
        {       
            inputmanager.InputEvent -= EventReceived;

            ea = new EventAction (new InitialEvent (sender, source, name, raw));

            lbl_e_name_val.Text    = ea.initial_event.Name;
            lbl_e_device_val.Text  = ea.initial_event.Source;
            lbl_e_module_val.Text  = ea.initial_event.Sender;
            lbl_e_numeric_val.Text = "0x" + ea.initial_event.Raw.ToString ("x");

            on_dlg_press_button_close ();
        }
        
        public AddEventDialog (Gtk.Window wnd_main)
        {
            Glade.XML ui = Glade.XML.FromAssembly ("initial.glade", "dlg_add_event", "initial");
            ui.Autoconnect (this);
                                    
            ui = Glade.XML.FromAssembly ("initial.glade", "dlg_press_button", "initial");
            ui.Autoconnect (this);

            this.wnd_main = wnd_main;

            dlg_add_event.TransientFor   = wnd_main;
            dlg_add_event.WindowPosition = Gtk.WindowPosition.CenterOnParent;

            dlg_press_button.TransientFor      = wnd_main;
            dlg_press_button.WindowPosition    = Gtk.WindowPosition.CenterOnParent;
            dlg_press_button.DestroyWithParent = true;

            acmd  = new ActionCommand ();
            axkey = new ActionXKeysym ();
            asys  = new ActionSystem ();

            inputmanager = InputManager.New ();
            inputmanager.InputEvent += new InputEventHandler (EventReceived);
        }
    }
        
    public class UserInterface 
    {
        [Glade.Widget] Gtk.Window   wnd_main;
        [Glade.Widget] Gtk.TreeView tvw_ea;

        void on_about_clicked (object sender, EventArgs args)
        {
            Gdk.Pixbuf logo       = new Gdk.Pixbuf (null, "ial.png");
            string [] authors     = { "Timo Hoenig <thoenig@nouse.net>" };
            string [] documenters = { "Timo Hoenig <thoenig@nouse.net>" };
            string translators    = Mono.Posix.Catalog.GetString ("translator-credits");
 
            if(System.String.Compare(translators, "Translator Credits") == 0) 
                translators = null;

            /* TODO: get version by define */
            new Gnome.About ("Initial", Defines.VERSION , "Copyright 2005 Timo Hoenig",
                             Mono.Posix.Catalog.GetString ("A User Interface for the\n Input Abstraction Layer"),
                             authors, documenters, translators, logo).Show ();
        }

        public void on_cb_toggled (object o, EventArgs args)
        {
            CheckButton cb = (CheckButton) o;
            Initial.Config.AskOnExit = !cb.Active;
            Initial.Config.Save ();
        }

        public void on_wnd_main_delete_event (object o, EventArgs args)
        {
            Application.Quit ();
        }

        public bool Visible
        {
            get {
                return wnd_main.Visible;
            }
        }

        public void Hide ()
        {
            wnd_main.Hide ();
        }

        public void Show ()
        {
            wnd_main.Show ();
        }

        public void Quit ()
        {
            if (Initial.Config.AskOnExit == false) {
                Application.Quit ();
            } else {
                MessageDialog md = new MessageDialog (wnd_main,
                                                      DialogFlags.DestroyWithParent,
                                                      MessageType.Question,
                                                      ButtonsType.YesNo,
                                                      Mono.Posix.Catalog.GetString ("Really quit?") );

                CheckButton cb = new CheckButton (Mono.Posix.Catalog.GetString ("Do Not Ask Again"));
                cb.Toggled += on_cb_toggled;
                cb.Visible = true;

                md.ActionArea.Add (cb);
                md.ActionArea.ReorderChild (cb, 0);

                ResponseType result = (ResponseType) md.Run ();
                
                md.Destroy ();

                if (result == ResponseType.Yes) {
                    Application.Quit ();
                }
            }
        }
        
        public void on_quit1_activate (object o, EventArgs args)
        {
            Quit ();
        }

        public void on_btn_event_add_clicked (object o, EventArgs args)
        {
            new AddEventDialog (this.wnd_main);
        }

        public void on_btn_event_delete_clicked (object o, EventArgs args)
        {
            TreeIter iter;
            TreeModel model;

            if (tvw_ea.Selection.GetSelected (out model, out iter)) {
                string event_name  = (string) model.GetValue (iter, 0); 
                string action_name = (string) model.GetValue (iter, 1);

                string question = 
                    String.Format (Mono.Posix.Catalog.GetString ("Really delete the action \"{0}\" for the event \"{1}\"?"),
                                   action_name,
                                   event_name);

                MessageDialog md = new MessageDialog (wnd_main,
                    DialogFlags.DestroyWithParent,
                    MessageType.Question,
                    ButtonsType.YesNo,
                    question);

                ResponseType result = (ResponseType) md.Run ();
                
                md.Destroy ();

                if (result == ResponseType.Yes) {
                    foreach (EventAction ea in Initial.Config.EventActions) {
                        if (ea.iter.Equals (iter)) {
                            Initial.Config.RemoveEventAction (ea);
                            Initial.Config.Save ();
                        }
                    }
                }
            }
        }

        public void on_btn_quit_clicked (object o, EventArgs args)
        {
            Quit ();
        }
                
        public void on_add_event1_activate (object o, EventArgs args)
        {
            new AddEventDialog (this.wnd_main);
        }
                
        public void PopulateTreeView ()
        {
            TreeStore tvw_ea_store = new TreeStore (typeof(string), typeof(string));

            foreach (EventAction ea in Initial.Config.EventActions) {
                string   col1;
                string   col2;
                TreeIter iter;

                col1 = ea.initial_event.Name;

                InitialAction a = ea.initial_action;
                if (a.GetType() == typeof(ActionCommand)) {
                    string args = ((ActionCommand) ea.initial_action).args; 
                    if (args.Length > 0) {
                        col2 =
                            String.Format (Mono.Posix.Catalog.GetString ("Execute command \"{0}\" with argument(s) \"{1}\""),
                                           ((ActionCommand) ea.initial_action).command, args);
                    } else {
                        col2 =
                            String.Format (Mono.Posix.Catalog.GetString ("Execute command \"{0}\" with no additional arguments"),
                                           ((ActionCommand) ea.initial_action).command);

                    }
                } else if (a.GetType() == typeof(ActionXKeysym)) {
                    col2 = "Emulate FIXME "; 
                } else if (a.GetType() == typeof(ActionSystem)) {
                    col2 = "Execute FIX ME ";
                } else {
                    col2 = "(unknown action)";
                }

                iter = tvw_ea_store.AppendValues (col1, col2);
                tvw_ea_store.AppendValues (iter, Mono.Posix.Catalog.GetString("Type:"),
                                           ea.initial_action.GetType().ToString ());
                tvw_ea_store.AppendValues (iter, Mono.Posix.Catalog.GetString("Sender:"),
                                           ea.initial_event.Sender);
                tvw_ea_store.AppendValues (iter, Mono.Posix.Catalog.GetString("Source:"),
                                           ea.initial_event.Source);
                tvw_ea_store.AppendValues (iter, Mono.Posix.Catalog.GetString("Raw Value:"),
                                           ea.initial_event.Raw.ToString ());

                ea.iter = iter;
            }
                        
            tvw_ea.Model = tvw_ea_store;
        }

        public UserInterface ()
        {
            Glade.XML ui = Glade.XML.FromAssembly ("initial.glade", "wnd_main", "initial");
            ui.Autoconnect (this);

            wnd_main.Icon = new Gdk.Pixbuf (null, "ial.png");

            tvw_ea.AppendColumn (Mono.Posix.Catalog.GetString("Event Name"), new CellRendererText (), "text", 0);
            tvw_ea.AppendColumn (Mono.Posix.Catalog.GetString("Action"),     new CellRendererText (), "text", 1);

            PopulateTreeView ();

            wnd_main.DeleteEvent += on_wnd_main_delete_event;
            wnd_main.DestroyEvent += on_wnd_main_delete_event;

        }
    }
}

