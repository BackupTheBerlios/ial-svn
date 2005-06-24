/* libial_toshiba.h
 *
 * Copyright (C) 2004, 2005 Timo Hoenig <thoenig@nouse.net>
 *                          All rights reserved
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

#define MODULE_NAME "Toshiba Input Abstraction Layer Module"
#define MODULE_TOKEN "toshiba"
#define MODULE_VERSION "0.0.1"
#define MODULE_AUTHOR "Timo Hoenig <thoenig@nouse.net>"
#define MODULE_DESCR "This module polls for Fn combinations provided by the Toshiba ACPI driver."

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libial/libial.h>

/** libial_toshiba_utils.c */
const char *toshiba_fnkey_description (int);

/** libial_toshiba_filter.c */
gboolean toshiba_add_filter (void);

/** libial_toshiba_main.c */
void toshiba_event_send (void);
gboolean toshiba_acpi_check (void);
void toshiba_key_flush (void);
gboolean toshiba_key_poll (void);
gboolean toshiba_start (void);

gboolean mod_load (void);
gboolean mod_unload (void);

/** Toshiba ACPI interface */
#define ACPI_TOSHIBA            "/proc/acpi/toshiba"
#define ACPI_TOSHIBA_KEYS       "/proc/acpi/toshiba/keys"

/** Polling frequency in ms*/
#define POLL_FREQ_MAX           1000
#define POLL_FREQ_DEFAULT       100
#define POLL_FREQ_MIN           50

/** Fn-Keys and values */

#define FN              0x100
#define FN_ESCAPE       0x101
#define FN_1            0x102
#define FN_2            0x103
#define FN_3            0x104
#define FN_4            0x105
#define FN_5            0x106
#define FN_6            0x107
#define FN_MINUS        0x10c
#define FN_EQUAL        0x10d
#define FN_BACKSPACE    0x10e
#define FN_TAB          0x10f
#define FN_Q            0x110
#define FN_W            0x111
#define FN_E            0x112
#define FN_R            0x113
#define FN_T            0x114
#define FN_Y            0x115
#define FN_L_BRACKET    0x11a
#define FN_R_BRACKET    0x11b
#define FN_A            0x11e
#define FN_S            0x11f
#define FN_D            0x120
#define FN_F            0x121
#define FN_G            0x122
#define FN_H            0x123
#define FN_APOSTROPHE   0x128
#define FN_TILDE        0x129
#define FN_BACKSLASH    0x12b
#define FN_Z            0x12c
#define FN_X            0x12d
#define FN_C            0x12e
#define FN_V            0x12f
#define FN_B            0x130
#define FN_N            0x131
#define FN_LESS_THAN    0x133
#define FN_PRTSC        0x137
#define FN_SPACE        0x139
#define FN_CAPSLOCK     0x13a
#define FN_F1           0x13b
#define FN_F2           0x13c
#define FN_F3           0x13d
#define FN_F4           0x13e
#define FN_F5           0x13f
#define FN_F6           0x140
#define FN_F7           0x141
#define FN_F8           0x142
#define FN_F9           0x143
#define FN_HOME         0x147
#define FN_UP           0x148
#define FN_PAGEUP       0x149
#define FN_LEFT         0x14b
#define FN_RIGHT        0x14d
#define FN_END          0x14f
#define FN_DOWN         0x150
#define FN_PAGEDOWN     0x151
#define FN_INS          0x152
#define FN_DEL          0x153
#define FN_LESS_THAN_2  0x156
#define CD_BUTTON       0xb25
#define DIGITAL_BUTTON  0xb27
#define STOP            0xb30
#define SKIP_BACK       0xb31
#define SKIP_FORWARD    0xb32
#define PLAY_PAUSE      0xb33
#define TV_BUTTON       0xb85
#define E_BUTTON        0xb86
#define I_BUTTON        0xb87

struct Key {
    const int value;
    const char *descr;
};

static struct Key keys[] = {
    {FN_ESCAPE, "Fn-Escape (Mute)"},
    {FN_1, "Fn-1 (Volume Down)"},
    {FN_2, "Fn-2 (Volume Up)"},
    {FN_3, "Fn-3"},
    {FN_4, "Fn-4"},
    {FN_5, "Fn-5"},
    {FN_6, "Fn-6"},
    {FN_MINUS, "Fn--"},
    {FN_EQUAL, "Fn-="},
    {FN_BACKSPACE, "Fn-Backspace"},
    {FN_TAB, "Fn-Tab"},
    {FN_Q, "Fn-q"},
    {FN_W, "Fn-w"},
    {FN_E, "Fn-e"},
    {FN_R, "Fn-r"},
    {FN_T, "Fn-t"},
    {FN_Y, "Fn-y"},
    {FN_L_BRACKET, "Fn-["},
    {FN_R_BRACKET, "Fn-]"},
    {FN_A, "Fn-a"},
    {FN_S, "Fn-s"},
    {FN_D, "Fn-d"},
    {FN_F, "Fn-f"},
    {FN_G, "Fn-g"},
    {FN_H, "Fn-h"},
    {FN_APOSTROPHE, "Fn-'"},
    {FN_TILDE, "Fn-~"},
    {FN_BACKSLASH, "Fn-\\"},
    {FN_Z, "Fn-z"},
    {FN_X, "Fn-x"},
    {FN_C, "Fn-c"},
    {FN_V, "Fn-v"},
    {FN_B, "Fn-b"},
    {FN_N, "Fn-n"},
    {FN_LESS_THAN, "Fn-<"},
    {FN_PRTSC, "Fn-PrtSc"},
    {FN_SPACE, "Fn-Space"},
    {FN_CAPSLOCK, "Fn-Caps Lock"},
    {FN_F1, "Fn-F1 (Lock)"},
    {FN_F2, "Fn-F2"},
    {FN_F3, "Fn-F3 (Suspend to RAM)"},
    {FN_F4, "Fn-F4 (Suspend to Disk)"},
    {FN_F5, "Fn-F5 (Switch LCD/CRT)"},
    {FN_F6, "Fn-F6 (Brightness Down)"},
    {FN_F7, "Fn-F7 (Brightness Up)"},
    {FN_F8, "Fn-F8"},
    {FN_F9, "Fn-F9"},
    {FN_HOME, "Fn-Home"},
    {FN_UP, "Fn-Up"},
    {FN_PAGEUP, "Fn-Page Up"},
    {FN_LEFT, "Fn-Left"},
    {FN_RIGHT, "Fn-Right"},
    {FN_END, "Fn-End"},
    {FN_DOWN, "Fn-Down"},
    {FN_PAGEDOWN, "Fn-Page Down"},
    {FN_INS, "Fn-Ins"},
    {FN_DEL, "Fn-Del"},
    {FN_LESS_THAN_2, "Fn-<"},
    {CD_BUTTON, "Hotbutton (CD-Button)"},
    {DIGITAL_BUTTON, "Hotbutton (Digital-Button)"},
    {STOP, "Hotbutton (Stop)"},
    {SKIP_BACK, "Hotbutton (Skip Track Back)"},
    {SKIP_FORWARD, "Hotbutton (Skip Track Forward)"},
    {PLAY_PAUSE, "Hotbutton (Play/Pause)"},
    {TV_BUTTON, "Hotbutton (TV-Button)"},
    {E_BUTTON, "Hotbutton (E-Button)"},
    {I_BUTTON, "Hotbutton (I-Button)"},
    {0, NULL}
};

struct Fnkey_s {
    FILE *fp;

    int hotkey_ready;
    int value;

    const char *description;
};
