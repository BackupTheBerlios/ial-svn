/* main.c - Input Abstraction Layer Daemon
 *
 * Copyright (C) 2004 Timo Hoenig <thoenig@nouse.net>
 *                    All rights reserved
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
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <getopt.h>

#include "iald.h"
#include "iald_mod.h"
#include "iald_conf.h"

IalModule *modules_list_head = NULL;

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

/** D-BUS filter function
 *
 * Message handler for method invocations. All invocations on any object
 * or interface is routed through this function.
 * 
 * @param  connection                   D-BUS connection
 * @param  message                      A D-BUS message
 * @param  user_data                    User data
 * @return                              What to do with the message
 *
 */

static DBusHandlerResult filter_function(DBusConnection * connection,
                                         DBusMessage * message,
                                         void *user_data)
{

    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
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

void opt_header()
{
    static char *header = {
        "Input Abstraction Layer v" VERSION "\n"
            "Copyright (C) 2004 Timo Hoenig <thoenig@nouse.net>\n" "\n"
    };

    printf("%s", header);
}

/**
 * Print usage for command line options.
 */

void opt_usage()
{
    static char *usage = {
        "Usage: iald [ options ] [ command ]\n"
            "\n"
            "Options:\n"
            "-d n, --debug n:            Set debug level (n = 0..3)\n"
            "-f,   --foreground:         Run in foreground\n"
            "-m s, --module-options s:   Set module options (s = { module options })\n"
            "\n"
            "module options = token:option[:option][,token:{option}[,option]]\n"
            "token          = Module token (see --list-verbose)\n"
            "option         = Module option (see --list-verbose)\n"
            "\n"
            "Commands:\n"
            "-h, --help:           Show this message and exit\n"
            "-l, --list:           List modules and exit\n"
            "-L, --list-verbose:   List modules and options and exit \n"
            "-v, --version:        Show version and exit\n" "\n"
            "Note: Command line options overwrite configuration file options.\n"
            "\n"
    };

    opt_header();

    printf("%s", usage);
}

/**
 * Print available Input Abtraction Layer modules.
 */

void opt_list()
{
    IalModule *m = NULL;

    opt_header();

    printf("Available modules:\n\n");

    m = modules_list_head;

    while (m) {
        printf("%s\n", m->data->name);
        m = m->next;
    }

    printf("\n");

    printf
        ("Use  'iald --list-verbose'  for module options and more information.\n\n");

}

/**
 * Print available Input Abstraction Layer modules and their options (verbose).
 */

void opt_list_verbose()
{
    IalModule *m = NULL;
    ModuleOption *opt = NULL;

    opt_header();

    printf("Available modules:\n\n");

    m = modules_list_head;

    while (m) {
        printf("Module name:        %s v%s (%s)\n", m->data->name,
               m->data->version, m->data->author);
        printf("Module description: %s\n",
               m->data->descr ? m->data->descr : "(no description)");
        printf("Module token:       %s\n", m->data->token);
        printf("Module options:     ");

        opt = m->data->options;

        while (opt->name) {
            printf("%s, default: %s=%s\n                    ", opt->descr,
                   opt->name, opt->value);
            *opt++;
        }
        printf("\n");

        m = m->next;
    }

    printf
        ("Example:  'iald --module-options acpi:disable=yes,toshiba:poll_freq=100:disable=no'\n\n");

}

/**
 * Parse command line options for Input Abstraction Layer modules.
 *
 * @param  optarg                   Command line option arguments for module options (-m,
 *                                  --module-options).
 *                                  
 */

void opt_modules_opts(char *optarg)
{
    IalModule *m = NULL;
    ModuleOption *opt = NULL;
    char *token_opts = NULL;
    char *token_opt = NULL;
    char *token_val = NULL;
    const char *module_opts = optarg;

    DEBUG(("Parsing module options (%s)...", optarg));

    m = modules_list_head;

    while (m) {
        /* get pointer to token */
        token_opts = strstr(module_opts, m->data->token);

        /* token must be present and followed by ':' */
        if ((token_opts == NULL) ||
            (*(token_opts + strlen(m->data->token)) != ':')) {
            DEBUG(("No options passed for \"%s\" (token: \"%s\").",
                   m->data->name, m->data->token));
        }
        else {
            /* copy all options for token */
            token_opts =
                strndup(token_opts, strchr(token_opts, ',') - token_opts);

            DEBUG(("Parsing passed options for \"%s\" (token: \"%s\").",
                   m->data->name, m->data->token));
            DEBUG(("Token options: %s.", token_opts));

            opt = m->data->options;

            while (opt->name) {
                DEBUG(("Looking for option \"%s\"...", opt->name));

                /* get pointer to token option */
                token_opt = strstr(token_opts, opt->name);

                /* option must be present and followed by '=' */
                if (token_opt == NULL) {
                    DEBUG(("Not found."));
                }
                else if (*(token_opt + strlen(opt->name)) != '=') {
                    WARNING(("Wrong syntax for option %s. Wrong spelling or no option value supplied.", opt->name));
                }
                else {
                    DEBUG(("Found."));

                    /* copy option */
                    token_opt =
                        strndup(token_opt,
                                strchr(token_opt, ':') - token_opt);

                    /* remove option and trailing '=' */
                    token_val = strstr(token_opt, "=");

                    *token_val++;

                    if (strlen(token_val) <= MAX_BUF) {
                        strcpy(opt->value, token_val);
                        DEBUG(("Option \"%s\" set to \"%s\".", opt->name,
                               opt->value));
                    }
                    else {
                        WARNING(("Option value for \"%s\" too long (%i).",
                                 opt->name, strlen(token_val)));
                    }

                    if (token_opt)
                        free(token_opt);
                }

                *opt++;
            }

            if (token_opts)
                free(token_opts);
        }

        m = m->next;
    }
}

/**
 * Print Input Abstraction layer version and build date.
 */

void opt_version()
{
    static char *version = {
        "Package name: " PACKAGE_NAME
            "\n" "Package version: " PACKAGE_VERSION "\n" "Build date: "
            __DATE__ "\n" "Build time: " __TIME__ "\n" "\n"
    };

    opt_header();

    printf("%s", version);

}

/**
 * Parse command line options for Input Abstraction Layer daemon.
 *
 * @param argc                          Number of arguments.
 * @param argv                          Argument values
 * 
 */

void opt_parse(int argc, char *argv[])
{
    int option_index = 0;
    int c;

    static struct option long_options[] = {
        {"foreground", no_argument, 0, 'f'},
        {"help", no_argument, 0, 'h'},
        {"list", no_argument, 0, 'l'},
        {"list-verbose", no_argument, 0, 'L'},
        {"module-options", required_argument, 0, 'm'},
        {"version", no_argument, 0, 'v'},
        {"debug", required_argument, 0, 'd'},
        {0, 0, 0, 0}
    };

    while (1) {
        c = getopt_long(argc, argv, "fhlLvd:m:", long_options,
                        &option_index);

        if (c == -1)
            break;

        switch (c) {
        case 'h':
            opt_usage();
            exit(0);
            break;

        case 'l':
            opt_list();
            exit(0);
            break;

        case 'L':
            opt_list_verbose();
            exit(0);
            break;

        case 'm':
            opt_modules_opts(optarg);
            break;

        case 'v':
            opt_version();
            exit(0);
            break;

        case 'd':
            log_level_set(atoi(optarg));
            break;

        case 'f':
            printf("TODO\n");
            break;

        default:
            opt_usage();
            exit(1);
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
int main(int argc, char *argv[])
{
    IalModule *m = NULL;
    GMainLoop *loop;
    DBusError dbus_error;

    log_level_set(3);

    /* Scan for modules that both conf_parse() and opt_parse() can set
     * options for modules.
     */

    modules_scan();

    m = modules_list_head;
    if (!m) {
        WARNING(("No modules available"));
    }

    /* Parse config file */
    conf_parse();

    /* Parse command line options */
    if (argc > 1) {
        opt_parse(argc, argv);
    }

    loop = g_main_loop_new(NULL, FALSE);

    if (ial_dbus_connect() == FALSE) {
        ERROR(("D-Bus connection failed."));
        exit(1);
    }

    dbus_connection_setup_with_g_main(dbus_connection, NULL);

    dbus_error_init(&dbus_error);

    /* TODO
     *
     * Evaluate if it makes sense to check whether dbus_bus_service_exists() == TRUE and abort if so..
     *
     */

    dbus_bus_acquire_service(dbus_connection, IAL_DBUS_SERVICENAME, 0,
                             &dbus_error);

    if (dbus_error_is_set(&dbus_error)) {
        ERROR(("dbus_bus_acquire_service(): Error. (%s)",
               dbus_error.message));
        exit(1);
    }
    else {
        INFO(("dbus_bus_acquire_service(): Success. (%s)",
              IAL_DBUS_SERVICENAME));
    }

    dbus_connection_add_filter(dbus_connection, filter_function, NULL,
                               NULL);

    modules_load();

    g_main_loop_run(loop);

    return 0;
}

/** @} */
