#define MODULE_NAME "Keyboard Input Abstraction Layer Module"
#define MODULE_TOKEN "keyboard"
#define MODULE_VERSION "0.0.1"
#define MODULE_AUTHOR "Timo Hoenig <thoenig@nouse.net>"
#define MODULE_DESCR "This module reports unknown keycodes."

#define LOG_FILE "/var/log/messages"

#include "../libial/libial.h"

gboolean libial_keyboard_start(void);

gboolean mod_load(void);
gboolean mod_unload(void);

struct keyboard_s
{
    const char *scancode;
    GIOChannel *io_channel;

    int log_fd;
};
