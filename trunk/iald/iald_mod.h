/*************************************************************************** 
 *
 * iald_mod.h - Input Abstraction Layer Daemon Module Loader
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

typedef struct IalModule_s {
    ModuleData *data;

    struct IalModule_s *prev;
    struct IalModule_s *next;
} IalModule;

void *dl_function (char *, const char *);

/* Add/Remove a single modules to the configuration */
void module_add (char *);
void module_remove (IalModule *);

gboolean module_init (char *);
gboolean module_verify (char *);

/* Scan MODULES_DIR for modules */
void modules_scan (void);

/* Load/Unload all modules */
void modules_load (void);
void module_unload (IalModule *);
void modules_unload (void);
