/* libial_toshiba_main.c - Toshiba SMM Input Abstraction Layer Module
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

#include "libial_toshiba.h"

extern ModuleOption mod_options[];
extern ModuleData mod_data;

/**
 * @addtogroup IALMODTOSHIBA
 *
 * @{
 *
 */

IalEvent event;
static struct Fnkey_s fnkey;

/** Send IAL event. */
void toshiba_event_send()
{
    event.sender = mod_data.token;
    event.source = ACPI_TOSHIBA_KEYS;
    event.name = toshiba_fnkey_description(fnkey.value);
    event.raw = fnkey.value;

    /** Sending the event with libial */
    event_send(&event);
}

/** Check for Toshiba ACPI interface /proc/acpi/toshiba/keys.
 *
 * @returns TRUE on success, else FALSE.
 */
gboolean toshiba_acpi_check()
{
    fnkey.fp = fopen(ACPI_TOSHIBA_KEYS, "r+");

    if (!fnkey.fp) {
        ERROR(("Could not open %s.", ACPI_TOSHIBA_KEYS));
        return FALSE;
    }

    fclose(fnkey.fp);
    return TRUE;
}

/** Flush keys from the Toshiba hotkey register. */
void toshiba_key_flush()
{
    int flush_count = -1;
    fnkey.fp = fopen(ACPI_TOSHIBA_KEYS, "r+");

    if (!fnkey.fp) {
        ERROR(("Could not open %s.", ACPI_TOSHIBA_KEYS));
        return;
    }
    else {
        fnkey.hotkey_ready = 1;

        while (fnkey.hotkey_ready) {
            flush_count++;

            fprintf(fnkey.fp, "hotkey_ready:0");
            fclose(fnkey.fp);

            fnkey.fp = fopen(ACPI_TOSHIBA_KEYS, "r+");
            fscanf(fnkey.fp, "hotkey_ready: %d\nhotkey: 0x%4x",
                   &fnkey.hotkey_ready, &fnkey.value);
        }

        if (fnkey.fp)
            fclose(fnkey.fp);

        if (flush_count)
            INFO(("Flushed %i keys from %s.", flush_count,
                  ACPI_TOSHIBA_KEYS));
    }
}

/** Check whether there is a new event in the hotkey register
 *
 * @returns TRUE if there is an event pending, FALSE if no event pending.
 */
gboolean toshiba_key_ready()
{
    fnkey.fp = fopen(ACPI_TOSHIBA_KEYS, "r+");

    if (!fnkey.fp) {
        /* TODO remove module, panic */
        return FALSE;
    }

    fscanf(fnkey.fp, "hotkey_ready: %1d\nhotkey: 0x%4x",
           &fnkey.hotkey_ready, &fnkey.value);

    if (fnkey.hotkey_ready) {
        /** Signal the driver that we have read the key */
        fprintf(fnkey.fp, "hotkey_ready:0");
        fclose(fnkey.fp);
        return TRUE;
    }
    else {
        fclose(fnkey.fp);
        return FALSE;
    }
}

/** Callback to poll hotkey register and report occuring events.
 *
 * @returns TRUE on success, else FALSE.
 */
gboolean toshiba_key_poll()
{
    while (toshiba_key_ready() == TRUE) {

        /* If we have a description it is a known key.
         * otherwise we have either a key up event
         * or some unkown key.
         */

        fnkey.description = toshiba_fnkey_description(fnkey.value);

        if (fnkey.description) {
            toshiba_event_send();
        }
        else {
            if (!toshiba_fnkey_description(fnkey.value - 0x80) &&
                (fnkey.value != FN)) {
                INFO(("Unknown key event (0x%x). Please report to <thoenig at nouse dot net>", fnkey.value));
            }

        }
    }

    return TRUE;
}

/** Toshiba SMM Input Abstraction Layer Module initialization.
 *
 * @returns TRUE on success, FALSE on error.
 */
gboolean toshiba_start()
{
    DBusError dbus_error;

    dbus_error_init(&dbus_error);

    if (toshiba_acpi_check() == FALSE) {
        ERROR(("Failed to access the Toshiba ACPI interface."));
        return FALSE;
    }

    toshiba_key_flush();

    if (!
        (g_timeout_add
         (atoi(mod_options[1].value), (GSourceFunc) toshiba_key_poll,
          NULL))) {
        ERROR(("g_timeout_add() for toshiba_key_poll() failed."));
        return FALSE;
    }

    if (!(toshiba_add_filter())) {
        ERROR(("toshiba_add_filter() failed."));
        return FALSE;
    }
    return TRUE;
}

/* @} */
