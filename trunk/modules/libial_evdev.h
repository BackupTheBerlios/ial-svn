#define MODULE_NAME "Event Interface Input Abstraction Layer Module"
#define MODULE_TOKEN "evdev"
#define MODULE_VERSION "0.0.1"
#define MODULE_AUTHOR "Timo Hoenig <thoenig@nouse.net>"
#define MODULE_DESCR "This module reports (unknown) keycodes."

#include <sys/time.h>
#include <sys/ioctl.h>
#include <asm/types.h>

#include "../libial/libial.h"
#include "libial_evdev_keys.h"

static char *KEY_NAME[KEY_MAX] = {
#include "libial_evdev_key_to_string.h"
};

#define MAX_EVENT_DEV           32
#define MAX_EVENT_DEV_NAME      128
#define EV_VERSION              0x010000

/* IOCTL */
#define EVIOCGVERSION           _IOR('E', 0x01, int)                    /* get driver version */
#define EVIOCGNAME(len)         _IOC(_IOC_READ, 'E', 0x06, len)         /* get device name */

#define key_blacklisted(code) (((code) < KEY_MAX) && ((code) > KEY_MIN) ? FALSE : TRUE)
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

gboolean libial_keyboard_start(void);

gboolean mod_load(void);
gboolean mod_unload(void);

typedef struct evdev_s {
    int fd, watch;
    GIOChannel *io_channel;
}
evdev;

struct input_event_s {
    struct timeval time;
    __u16 type;
    __u16 code;
    __s32 value;
};
