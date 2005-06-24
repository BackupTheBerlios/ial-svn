/* libial_evdev.h 
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

#define MODULE_NAME "Event Interface Input Abstraction Layer Module"
#define MODULE_TOKEN "evdev"
#define MODULE_VERSION "0.0.1"
#define MODULE_AUTHOR "Timo Hoenig <thoenig@nouse.net>"
#define MODULE_DESCR "This module reports (unknown) keycodes."

#include <sys/time.h>
#include <sys/ioctl.h>
#include <asm/types.h>

#include <libial/libial.h>

#include "libial_evdev_keys.h"

static char *KEY_NAME[KEY_MAX] = {
#include "libial_evdev_key_to_string.h"
};

#define MAX_EVENT_DEV           32
#define MAX_EVENT_DEV_NAME      128
#define EV_VERSION              0x010000

/* IOCTL */
#define EVIOCGVERSION           _IOR('E', 0x01, int)    /* get driver version */
#define EVIOCGNAME(len)         _IOC(_IOC_READ, 'E', 0x06, len) /* get device name */
#define EVIOCGPHYS(len)         _IOC(_IOC_READ, 'E', 0x07, len) /* get physical location */

#define key_blacklisted(code) ((code < KEY_MAX) && (code > KEY_MIN) ? FALSE : TRUE)
#define key_to_string(code) (KEY_NAME[code] ? KEY_NAME[code] : "Unknown key")

/*
 * Event types
 */

#define EV_SYN                  0x00
#define EV_KEY                  0x01
#define EV_REL                  0x02
#define EV_ABS                  0x03
#define EV_MSC                  0x04
#define EV_LED                  0x11
#define EV_SND                  0x12
#define EV_REP                  0x14
#define EV_FF                   0x15
#define EV_PWR                  0x16
#define EV_FF_STATUS            0x17
#define EV_MAX                  0x1f

gboolean evdev_callback (gpointer);
gboolean libial_evdev_start (void);

gboolean mod_load (void);
gboolean mod_unload (void);

typedef struct Evdev_s {
    int fd,
      watch;
    GIOChannel *io_channel;
} evdev;

struct Input_Event_s {
    struct timeval time;
    __u16 type;
    __u16 code;
    __s32 value;
};
