/***************************************************************************
 *
 * libial_acpi.h - ACPI Input Abstraction Layer Module
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

#define MODULE_NAME "ACPI Input Abstraction Layer Module"
#define MODULE_TOKEN "acpi"
#define MODULE_VERSION "0.0.1"
#define MODULE_AUTHOR "Timo Hoenig <thoenig@nouse.net>"
#define MODULE_DESCR "This module reports ACPI button events."

/** ACPI interface */
#define ACPI_EVENT              "/proc/acpi/event"
#define ACPI_LID                "/proc/acpi/button/lid/LID/state"
#define ACPID_SOCKET            "/var/run/acpid.socket"

/** IBM ACPI events */
#define ACPI_IBM_STB            "00001003"
#define ACPI_IBM_STR            "00001004"
#define ACPI_IBM_STD            "0000100c"

/** Lid states */
#define ACPI_LID_STATE_OPEN     1
#define ACPI_LID_STATE_CLOSED   0
#define ACPI_LID_STATE_ERROR    -1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libial/libial.h>

/** libial_acpi_main.c */
void acpi_event_send (char *);
gboolean acpi_event_fd_init (void);
int acpi_lid_state (void);
void acpi_event_handle (GString *);
gboolean acpi_event_callback (GIOChannel *, GIOCondition, gpointer);
gboolean acpi_socket (void);
gboolean acpi_raw (void);
gboolean libial_acpi_start (void);

/** libial_acpi.c */
gboolean mod_load (void);
gboolean mod_unload (void);

struct Acpi_s {
    const char *event;
    GIOChannel *io_channel;

    int event_fd;
};
