/***************************************************************************
 *
 * libial_acpi_main.c - ACPI Input Abstraction Layer Module
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

#include "libial_acpi.h"

/* Socket operations */
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

extern ModuleOption mod_options[];
extern ModuleData mod_data;

/**
 * @addtogroup IALMODACPI
 *
 * @{
 *
 */

struct Acpi_s acpi;
int acpi_event_watch;

/** Send IAL event.
 *
 * @param button        Button description.
 */

void
acpi_event_send (char *button)
{
    IalEvent event;

    event.sender = mod_data.token;
    event.source = ACPI_EVENT;
    event.name = button;
    event.raw = -1;

    event_send (&event);
}

/** Initialize ACPI event source
 *
 * @returns TRUE on success, else FALSE.
 */

gboolean
acpi_event_fd_init ()
{
    /* mod_options[1].value indicates whether we may use the ACPI event interface or not */
    if (strcmp (mod_options[1].value, "false\0") == 0) {
        /* raw access to the ACPI event interface */
        acpi.event_fd = open (ACPI_EVENT, 0);

        if (acpi.event_fd >= 0) {
            acpi.io_channel = g_io_channel_unix_new (acpi.event_fd);
            return TRUE;
        } else {
            WARNING (("Failed to access the ACPI event interface.")); 
        }
    }
        
    /* access to the ACPI events using socket `acpid` */
    acpi.event_fd = socket (PF_UNIX, SOCK_STREAM, 0);

    if (acpi.event_fd >= 0) {
        struct sockaddr_un addr;
        addr.sun_family = AF_UNIX;
        strcpy (addr.sun_path, ACPID_SOCKET);
        if (connect (acpi.event_fd, (struct sockaddr *) &addr, sizeof (addr))
            == 0) {
            acpi.io_channel = g_io_channel_unix_new (acpi.event_fd);
            return TRUE;
        } else {
            WARNING (("Failed to access the socket interface of 'acpid'."));
        }
    }

    return FALSE;
}

/** Check LID state
 *
 * @returns ACPI_LID_STATE_OPEN if LID is open, ACPI_LID_STATE_CLOSED if LID is closed, ACPI_LID_STATE_ERROR on error.
 */

int
acpi_lid_state ()
{
    FILE *fd;
    char buffer[MAX_BUF];

    fd = fopen (ACPI_LID, "r");

    if (fd) {
        fscanf (fd, "%[^\"]", buffer);
        fclose (fd);

        if (strstr (buffer, "open")) {
            return ACPI_LID_STATE_OPEN;
        } else if (strstr (buffer, "closed")) {
            return ACPI_LID_STATE_CLOSED;
        } else {
            return ACPI_LID_STATE_ERROR;
        }
    } else {
        WARNING (("Could not open %s.", ACPI_LID));
        return ACPI_LID_STATE_ERROR;
    }
}

/** Handle an ACPI event.
 *
 * @param acpi_event String which contains the whole ACPI event.
 */

void
acpi_event_handle (GString * acpi_event)
{
    /* regular ACPI events */
    if (strstr (acpi_event->str, "button")) {
        if (strstr (acpi_event->str, "PWRF")) {
            /* Power Button */
            acpi_event_send ("Power Button");
        } else if (strstr (acpi_event->str, "SLPF")) {
            /* Sleep Button */
            acpi_event_send ("Sleep Button");
        } else if (strstr (acpi_event->str, "LID")) {
            /* Lid Button */

            /** Determine lid state and send event */
            switch (acpi_lid_state ()) {
            case ACPI_LID_STATE_OPEN:
                acpi_event_send ("Lid Switch (open)");
                break;

            case ACPI_LID_STATE_CLOSED:
                acpi_event_send ("Lid Switch (close)");
                break;

            case ACPI_LID_STATE_ERROR:
                break;

            default:
                break;
            }
        }
    /* IBM ACPI events */
    } else if (strstr (acpi_event->str, "ibm")) {
        if (strstr (acpi_event->str, ACPI_IBM_STB)) {
            /* Stand-by */
            acpi_event_send ("Stand-by (IBM)");
        } else if (strstr (acpi_event->str, ACPI_IBM_STR)) {
            /* Suspend to RAM */
            acpi_event_send ("Suspend to RAM (IBM)");
        } else if (strstr (acpi_event->str, ACPI_IBM_STD)) {
            /* Suspend to Disk */
            acpi_event_send ("Suspend to Disk (IBM)");
        }
    }
}

/** ACPI Event callback
 *
 * @param       chan GIO Channel.
 * @param       cond GIO Conndition.
 * @param       data Data.
 * @returns     TRUE on success, FALSE on error (callback function will be removed from event loop).
 */

gboolean
acpi_event_callback (GIOChannel * chan, GIOCondition cond, gpointer data)
{
    if (cond & (G_IO_ERR | G_IO_HUP)) {
        /* better implemenration TODO */

        /* does this lead to some race? e.g. if acpid is restarted we claim
         * /proc/acpi/event and acpid will not be longer able to receive acpi
         * events.
         *
         * it is nice to restart the module since:
         * - if we were listening on the socket of acpid and acpid shuts down
         *   we are reinitializing the module and get the events from the
         *   /proc/acpi/event.
         */

	if (acpi.event_fd)
		close (acpi.event_fd);

        /* wait a bit to give acpid a chance to claim the interface again */
        usleep (1000 * 1000 * 2);

        libial_acpi_start ();
        return FALSE;
    } else {
        gsize i;
        GString *acpi_event;
        GError *gerror = NULL;
        acpi_event = g_string_new (NULL);

        if (g_io_channel_read_line_string (acpi.io_channel, acpi_event, &i, &gerror)
            == G_IO_STATUS_NORMAL) {

            acpi_event_handle (acpi_event);
        }

        g_string_free (acpi_event, TRUE);

    }

    return TRUE;
}

/** ACPI Input Abstraction Layer Module initialization.
 *
 * @returns TRUE on success, FALSE on error.
 */

gboolean
libial_acpi_start ()
{
    if (acpi_event_fd_init () == TRUE) {
        acpi_event_watch = g_io_add_watch (acpi.io_channel, G_IO_IN | G_IO_ERR | G_IO_HUP, acpi_event_callback, NULL);

        return TRUE;
    }

    return FALSE;
}

/* @} */
