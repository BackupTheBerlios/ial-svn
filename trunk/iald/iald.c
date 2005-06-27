/***************************************************************************
 *
 * iald.c - Input Abstraction Layer Daemon
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

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <signal.h>
#include <getopt.h>
#include <unistd.h>
#include <fcntl.h>

#include "iald.h"
#include "iald_mod.h"
#include "iald_conf.h"
#include "iald_dbus.h"

IalModule *modules_list_head = NULL;

static gboolean opt_foreground = FALSE;
static char opt_logfile[128] = IAL_LOG;
static int opt_debug = 0;

GMainLoop *loop;

/** Global D-BUS connection, libial */
extern DBusConnection *dbus_connection;

/** 
 * @defgroup IAL Input Abstraction Layer Daemon
 * @brief   The Input Abstraction Layer Daemon (iald) makes all kind of function keys
 *          found on laptops and multimedia keyboards accessible to other applications
 *          through a D-BUS interface.
 *          
 * @{
 */

gboolean
pid_file_exists ()
{
    struct stat sb;

    if (stat (IALD_PID_FILE, &sb) < 0) {
        /* PID file does not exist */
        return FALSE;
    } else {
        /* PID file does exist */
        return TRUE;
    }
}

void
pid_file_remove ()
{
    unlink (IALD_PID_FILE);
}

void
h_sigterm (int sigval)
{
    pid_file_remove ();
    exit (1);
}

void
h_sighup (int sigval)
{
    INFO (("Signal handler for SIGHUP is not yet implemented. SIGHUP ignored."));
}

void
opt_debug_set (int log_level)
{
    opt_debug = log_level;
}

void
opt_logfile_set (const char *logfile)
{
    if (strlen (logfile) > 128) {
        ERROR (("Filename for logfile to long."));
        return;
    }

    strncpy (opt_logfile, logfile, strlen (logfile));
    opt_logfile[strlen (logfile)] = 0;
}

void
opt_foreground_set (gboolean state)
{
    opt_foreground = state;
}

/**         
 * @defgroup IALCLI Command Line Interface (CLI)
 * @brief    The Input Abstraction Layer CLI implements the user interface and parses
 *           the supplied command line options.
 * 
 * @{
 */

/** 
 * Print header for command line options.
 */

void
opt_header ()
{
    static char *header = {
        "Input Abstraction Layer v" VERSION "\n" "Copyright (C) 2004, 2005 Timo Hoenig <thoenig@nouse.net>\n\n"
    };

    printf ("%s", header);
}

/**
 * Print usage for command line options.
 */

void
opt_usage ()
{
    static char *usage = {
        "Usage: iald [ options ] [ command ]\n"
            "\n"
            "Options:\n"
            "-d n, --debug n:                      Set debug level (n = 0..3)\n"
            "-o <filename>, --output <filename>:   Set log file (`stdout' for console output)\n"
            "-f,   --foreground:                   Run in foreground\n"
            "-m s, --module-options s:             Set module options (s = { module options })\n"
            "\n"
            "module option  = token:option=value[,option=value]\n"
            "token          = Module token (see --list-verbose)\n"
            "option         = Module option (see --list-verbose)\n"
            "\n"
            "Commands:\n"
            "-h, --help:           Show this message and exit\n"
            "-l, --list:           List modules and exit\n"
            "-L, --list-verbose:   List modules and options and exit \n"
            "-v, --version:        Show version and exit\n" "\n"
            "Note: Command line options overwrite configuration file options.\n" "\n"
    };

    opt_header ();

    printf ("%s", usage);
}

/**
 * Print available Input Abtraction Layer modules.
 */

void
opt_list ()
{
    IalModule *m = NULL;

    opt_header ();

    printf ("Available modules:\n\n");

    m = modules_list_head;

    while (m) {
        printf ("%s\n", m->data->name);
        m = m->next;
    }

    printf ("\n");

    printf ("Use the command  'iald --list-verbose'  for all available modules, "
            "their options options and more information.\n\n");

}

/**
 * Print available Input Abstraction Layer modules and their options (verbose).
 */

void
opt_list_verbose ()
{
    IalModule *m = NULL;
    ModuleOption *opt = NULL;

    opt_header ();

    printf ("Available modules:\n\n");

    m = modules_list_head;

    while (m) {
        printf ("Module name:        %s v%s (%s)\n", m->data->name, m->data->version, m->data->author);
        printf ("Module description: %s\n", m->data->descr ? m->data->descr : "(no description)");
        printf ("Module token:       %s\n", m->data->token);
        printf ("Module options:     ");

        opt = m->data->options;

        while (opt->name) {
            printf ("%s, default: %s=%s\n                    ", opt->descr, opt->name, opt->value);
            *opt++;
        }
        printf ("\n");

        m = m->next;
    }

    printf ("Example:  'iald --module-options acpi:disable=yes,toshiba:poll_freq=100:disable=no'\n\n");

}

/**
 * Parse command line options for Input Abstraction Layer modules.
 *
 * @param  optarg                   Command line option arguments for module options (-m,
 *                                  --module-options).
 *                                  
 */

void
opt_modules_opts (char *optarg)
{
    IalModule *m = NULL;
    ModuleOption *opt = NULL;
    char *token_opts = NULL;
    char *token_opt = NULL;
    char *token_val = NULL;
    const char *module_opts = optarg;

    DEBUG (("Parsing module options (%s)...", optarg));

    m = modules_list_head;

    while (m) {
        /* get pointer to token */
        token_opts = strstr (module_opts, m->data->token);

        /* token must be present and followed by ':' */
        if ((token_opts == NULL)
            || (*(token_opts + strlen (m->data->token)) != ':')) {
            DEBUG (("No options passed for \"%s\" (token: \"%s\").", m->data->name, m->data->token));
        } else {
            /* copy all options for token */
            token_opts = strndup (token_opts, strchr (token_opts, ',') - token_opts);

            DEBUG (("Parsing passed options for \"%s\" (token: \"%s\").", m->data->name, m->data->token));
            DEBUG (("Token options: %s.", token_opts));

            opt = m->data->options;

            while (opt->name) {
                DEBUG (("Looking for option \"%s\"...", opt->name));

                /* get pointer to token option */
                token_opt = strstr (token_opts, opt->name);

                /* option must be present and followed by '=' */
                if (token_opt == NULL) {
                    DEBUG (("Not found."));
                } else if (*(token_opt + strlen (opt->name)) != '=') {
                    WARNING (("Wrong syntax for option %s. Wrong spelling or no option value supplied.", opt->name));
                } else {
                    DEBUG (("Found."));

                    /* copy option */
                    token_opt = strndup (token_opt, strchr (token_opt, ':') - token_opt);

                    /* remove option and trailing '=' */
                    token_val = strstr (token_opt, "=");

                    *token_val++;

                    if (strlen (token_val) <= MAX_BUF) {
                        strcpy (opt->value, token_val);
                        DEBUG (("Option \"%s\" set to \"%s\".", opt->name, opt->value));
                    } else {
                        WARNING (("Option value for \"%s\" too long (%i).", opt->name, strlen (token_val)));
                    }

                    if (token_opt) {
                        free (token_opt);
                    }
                }

                *opt++;
            }

            if (token_opts)
                free (token_opts);
        }

        m = m->next;
    }
}

/**
 * Print Input Abstraction layer version and build date.
 */

void
opt_version ()
{
    static char *version = {
        "Package name: " PACKAGE_NAME "\n"
            "Package version: " PACKAGE_VERSION "\n" "Build date: " __DATE__ "\n" "Build time: " __TIME__ "\n" "\n"
    };

    opt_header ();

    printf ("%s", version);

}

/**
 * Parse command line options for Input Abstraction Layer daemon.
 *
 * @param argc                          Number of arguments.
 * @param argv                          Argument values
 * 
 */

void
opt_parse (int argc, char *argv[])
{
    int option_index = 0;
    int c = 0;

    static struct option long_options[] = {
        {"debug", required_argument, NULL, 'd'},
        {"output", required_argument, NULL, 'o'},
        {"foreground", no_argument, NULL, 'f'},
        {"help", no_argument, NULL, 'h'},
        {"list", no_argument, NULL, 'l'},
        {"list-verbose", no_argument, NULL, 'L'},
        {"module-options", required_argument, NULL, 'm'},
        {"version", no_argument, NULL, 'v'},
        {0, 0, 0, 0}
    };

    while (1) {
        c = getopt_long (argc, argv, "fhalLvd:m:o:", long_options, &option_index);
        if (c == -1)
            break;

        switch (c) {
        case 'h':
            opt_usage ();
            exit (0);
            break;

        case 'l':
            opt_list ();
            exit (0);
            break;

        case 'L':
            opt_list_verbose ();
            exit (0);
            break;

        case 'm':
            opt_modules_opts (optarg);
            break;

        case 'v':
            opt_version ();
            exit (0);
            break;

        case 'd':
            opt_debug_set (atoi (optarg));
            break;

        case 'f':
            opt_foreground_set (TRUE);
            break;

        case 'o':
            opt_logfile_set (optarg);
            break;

        default:
            opt_usage ();
            exit (1);
            break;
        }
    }

}

/** @} */

/** Entry point for Input Abstraction Layer Daemon.
 *
 * The main function of IAL first scans for IAL modules, then parses the configuration
 * file, the command line options (which overwrite options given by the configuration
 * file) and calls the init functions of the IAL modules.
 *
 * At the end of all initialization, the main loop is started.
 *
 * @param  argc                         Number of arguments
 * @param  argv                         Array of arguments
 * @return                              Exit code
 *
 */
int
main (int argc, char *argv[])
{
    IalModule *m = NULL;
    DBusError dbus_error;

/* Scan for modules that both conf_parse() and opt_parse() can set
 * options for modules.
 */

    modules_scan ();

    m = modules_list_head;
    if (!m) {
        WARNING (("No modules available"));
    }

    /* Parse config file */
    conf_parse ();

    /* Parse command line options */
    if (argc > 1) {
        opt_parse (argc, argv);
    }

    /* Setup logfile */
    log_logfile_set (opt_logfile);

    /* Set logging level */
    log_level_set (opt_debug);

    if (pid_file_exists () == TRUE) {
        ERROR (("PID file %s exists. Please remove if you're sure that there is no other instance of `iald` running."));
        exit (1);
    }

    /* Daemonize or run in foreground */
    if (opt_foreground == TRUE) {
        INFO (("Running in foreground"));
    } else {
        int child_pid;
        int pid_file;
        char pid[9];

        INFO (("Running as daemon"));

        chdir ("/");
        child_pid = fork ();
        switch (child_pid) {
        case -1:
            ERROR (("Could not fork."));
            exit (1);
            break;

        case 0:
            /* Child */
            break;

        default:
            /* Parent */
            exit (0);
            break;
        }

        unlink (IALD_PID_FILE);

        pid_file = open (IALD_PID_FILE, O_WRONLY | O_CREAT | O_TRUNC | O_EXCL, 0644);
        if (pid_file == -1) {
            ERROR (("Could not create pid file (%s).", IALD_PID_FILE));
            exit (1);
        }

        sprintf (pid, "%lu\n", (long unsigned) getpid ());
        write (pid_file, pid, strlen (pid));
        close (pid_file);

        atexit (pid_file_remove);
    }

    signal (SIGTERM, h_sigterm);
    signal (SIGHUP, h_sighup);

    loop = g_main_loop_new (NULL, FALSE);

    if (ial_dbus_connect () == FALSE) {
        ERROR (("D-BUS connection failed."));
        exit (1);
    }

    dbus_connection_setup_with_g_main (dbus_connection, NULL);

    dbus_error_init (&dbus_error);

    /* TODO
     *
     * Evaluate if it makes sense to check whether dbus_bus_service_exists() == TRUE and abort if so..
     *
     */

    dbus_bus_request_name (dbus_connection, IAL_DBUS_SERVICENAME, 0, &dbus_error);

    if (dbus_error_is_set (&dbus_error)) {
        ERROR (("dbus_bus_request_name(): Error. (%s)", dbus_error.message));
        exit (1);
    } else {
        INFO (("dbus_bus_request_name(): Success. (%s)", IAL_DBUS_SERVICENAME));
    }

    modules_load ();

    iald_dbus_init ();

    g_main_loop_run (loop);

    return 0;
}

/** @} */
