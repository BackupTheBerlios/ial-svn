#include <glib.h>
#include "libial_keyboard.h"

ModuleOption mod_options[] = {
    {"disable", "false", "disable=(true|false)"}
    ,
    {NULL}
};

ModuleData mod_data = {
    .name = MODULE_NAME,
    .token = MODULE_TOKEN,
    .version = MODULE_VERSION,
    .author = MODULE_AUTHOR,
    .descr = MODULE_DESCR,
    .type = IN_OUT_MODULE,
    .options = mod_options,
    .state = DISABLED,
    .load = mod_load,
    .unload = mod_unload,
};

ModuleData *mod_get_data()
{
    return &mod_data;
}

gboolean mod_load()
{
    if (libial_keyboard_start() == FALSE) {
        return FALSE;
    }

    return TRUE;
}

gboolean mod_unload()
{
    return TRUE;
}
