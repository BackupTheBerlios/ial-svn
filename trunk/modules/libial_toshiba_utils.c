/***************************************************************************
 *
 * libial_toshiba_utils.c - Toshiba SMM Input Abstraction Layer Module
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

#include "libial_toshiba.h"

void
toshiba_brightness_set (int mode)
{
    FILE *f;
    int   value = 0;

    f = fopen (ACPI_TOSHIBA_LCD, "r+");

    if (!f) {
        WARNING (("Error opening %s. Could not adjust brightness.", ACPI_TOSHIBA_LCD));
        goto out;
    }

    fscanf (f, "brightness:%1d", &value);

    switch (mode) {
        case BRN_UP:
            fprintf (f, "brightness:%1d", ++value);
            break;
        case BRN_DN:
            fprintf (f, "brightness:%1d", --value);
            break;
        default:
            break;
    }

    fclose (f);

out:
    return;
}

const char *
toshiba_fnkey_description (int key_value)
{
    struct Key *keylist = keys;

    while (keylist->value) {
        if (keylist->value == key_value) {
            return keylist->descr;
        } else {
            *keylist++;
        }
    }

    return NULL;
}

