/* iald.h
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

#include <libial/libial.h>

/** defined in iald.c */
void opt_debug_set(int);
void opt_foreground_set(gboolean);

void opt_header(void);
void opt_usage(void);
void opt_list(void);
void opt_list_verbose(void);
void opt_modules_opts(char *);
void opt_version(void);
void opt_parse(int, char *[]);

int main(int, char *[]);
