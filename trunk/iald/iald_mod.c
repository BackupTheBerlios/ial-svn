/*************************************************************************** 
 *
 * iald_mod.c - Input Abstraction Layer Daemon Module Loader
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>

#include <dlfcn.h>

#include <dirent.h>

#include <dbus/dbus.h>
#include <dbus/dbus-glib.h>

#include "iald.h"
#include "iald_mod.h"

extern IalModule *modules_list_head;

/**         
 * @defgroup IALMOD Module Loader
 * @ingroup  IAL
 * @brief    The Input Abstraction Layer module loader is scanning (LIBDIR)/iald/modules
 *           for valid IAL modules, checks whether they have the correct symbols and
 *           manages them in a linked list.
 * @{
 */

/** Load shared library.
 *
 * @param   filename    Filename of library to open
 * @param   symbol      Symbel to lookup in library
 * @returns             Void Pointer to function
 */

void *
dl_function (char *filename, const char *symbol)
{
    void *handle;
    char *error;
    void (*function) (void);

    handle = NULL;
    error = NULL;

    handle = dlopen (filename, RTLD_LAZY);

    if (!handle) {
        WARNING (("%s is not a shared library.", filename));
        return NULL;
    }

    function = dlsym (handle, symbol);
    if ((error = dlerror ()) != NULL) {
        WARNING (("%s (%s).", error, filename));
        return NULL;
    }

    return function;
}

/** Add a new module.
 *
 *  @param filename     Filename of module.
 */

void
module_add (char *filename)
{
    ModuleData *(*function) (void);
    IalModule *module;

    module = (IalModule *) malloc (sizeof (IalModule));

    function = dl_function (filename, "mod_get_data");
    module->data = function ();

    module->prev = NULL;
    module->next = modules_list_head;

    if (module->next != NULL)
        module->next->prev = module;

    modules_list_head = module;
}

/** Remove module.
 *
 * @param module        Module to be removed.
 */

void
module_remove (IalModule * module)
{

}

/** Initialize module.
 *
 * @param filename      Filename of module.
 * @returns             TRUE if module was initialized succeeded, else FALSE.
 */

gboolean
module_init (char *filename)
{
    gboolean (*mod_init) (void);

    mod_init = dl_function (filename, "mod_init");

    return (*mod_init) ();
}

/** Verify module.
 *
 * @param filename      Filename of module.
 * @returns             TRUE if module has the symbol `mod_get_data', else FALSE;
 */

gboolean
module_verify (char *filename)
{
    void (*function) (void);

    DEBUG (("Checking %s for symbol.", filename));

    function = dl_function (filename, "mod_get_data");
    if (!function) {
        WARNING (("%s is not a valid module.", filename));
        return FALSE;
    }

    DEBUG (("Symbol found."));

    return TRUE;
}

/** Scan (LIBDIR)/iald/modules for modules. 
 *
 */

void
modules_scan ()
{
/* Heavily based on scan_plugins() by XMMS (http://www.xmms.org) */
    char *filename, *extension;
    char *module_dir = MODULE_DIR;
    DIR *dir;
    struct dirent *entry;
    struct stat statbuf;

    dir = opendir (module_dir);
    if (!dir)
        return;

    while ((entry = readdir (dir)) != NULL) {
        filename = g_strdup_printf ("%s/%s", module_dir, entry->d_name);
        if (!stat (filename, &statbuf) && S_ISREG (statbuf.st_mode) &&
            (extension = strrchr (entry->d_name, '.')) != NULL) {
            if (!strcmp (extension, ".so")) {
                if (module_verify (filename) == TRUE) {
                    module_add (filename);
                    DEBUG (("Verification of \"%s\" succeeded.", filename));
                } else {
                    DEBUG (("Verification of \"%s\" failed.", filename));
                }
            }
        }
        g_free (filename);
    }
    closedir (dir);

}

/** Load all modules.
 *
 */

void
modules_load ()
{
    IalModule *m;

    m = modules_list_head;

    if (modules_list_head == NULL) {
        modules_scan ();
        m = modules_list_head;
    }

    if (m == NULL) {
        ERROR (("No modules found."));
    }

    /* Try to start the modules. */
    while (m) {
        if (m->data->load () == FALSE) {
            WARNING (("Failed to initialize %s.", m->data->name));
            m->data->initialized = FALSE;

            if (m->data->state == ENABLED) {
                WARNING (("Setting module state to disabled since initialization failed."));
                m->data->state = DISABLED;
            }
        } else {
            INFO (("%s loaded.", m->data->name));
            m->data->initialized = TRUE;
        }

        m = m->next;
    }
}

/** Unload a specific module
 *
 */

void
module_unload (IalModule * m)
{
    if (m == modules_list_head) {
        DEBUG (("Removing list head."));
        if (m->next != NULL) {
            modules_list_head = m->next;
            modules_list_head->prev = NULL;
        } else {
            modules_list_head = NULL;
        }
    } else if (m->next == NULL) {
        DEBUG (("Removing list tail."));
        m->prev->next = NULL;
    } else {
        DEBUG (("Removing in the list."));
        m->prev->next = m->next;
        m->next->prev = m->prev;
    }

}

/** Unload all modules.
 *
 */

void
modules_unload ()
{

}

/** @} */
