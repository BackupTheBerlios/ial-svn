/***************************************************************************
 *
 * libial_acpi.c - ACPI Input Abstraction Layer Module
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
 ***************************************************************************/

#include "libial_acpi.h"

/**
 * @defgroup IALMODACPI Input Abstraction Layer Module for ACPI Events
 * @brief   This module reads ACPI events (either by accessing /proc/acpi/event
 *          or the ACPI daemons socket) and reports them as IAL events.
 *          
 * @{
 */

/** Module Options. */
ModuleOption mod_options[] = {
    {"disable", "false", "disable=(true|false)"}
    ,
    {"force_acpid", "false", "force_acpid=(true|false)"}
    ,
    {NULL}
};

/** Module Data. */
ModuleData mod_data = {
    .name = MODULE_NAME,
    .token = MODULE_TOKEN,
    .version = MODULE_VERSION,
    .author = MODULE_AUTHOR,
    .descr = MODULE_DESCR,
    .type = INPUT,
    .options = mod_options,
    .state = DISABLED,
    .load = mod_load,
    .unload = mod_unload,
};

/** Return pointer to module data struct (mod_data).
 *
 * @returns pointer to mod_data.
 */
ModuleData *
mod_get_data ()
{
    return &mod_data;
}

/** Load Module
 *
 * @returns TRUE if initialization was successful, else FALSE.
 */
gboolean
mod_load ()
{
    /* Checking value for option "disable" */
    if (strcmp (mod_options[0].value, "true\0") == 0) {
        WARNING (("Setting module state to disabled."));
        mod_data.state = DISABLED;
    } else if (strcmp (mod_options[0].value, "false\0") == 0) {
        INFO (("Setting module state to enabled."));
        mod_data.state = ENABLED;
    } else {
        WARNING (("Wrong option value (%s) for option \"%s\".", mod_options[0].value, mod_options[0].name));
        WARNING (("Setting module state to disabled."));
        strcpy (mod_options[0].value, "true");
        mod_data.state = DISABLED;
    }

    if (mod_data.state == DISABLED) {
        INFO (("%s is disabled and not going to be loaded.", mod_data.name));
        return FALSE;
    }

    return (libial_acpi_start ());
}

/** Unload Module
 *
 *  @returns TRUE if unloading was successful, else FALSE.
 */
gboolean
mod_unload ()
{
    return TRUE;
}

/* @} */
