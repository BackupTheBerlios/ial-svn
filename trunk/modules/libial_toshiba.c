#include "libial_toshiba.h"

ModuleOption mod_options[] = {
    {"disable", "false", "disable=(true|false)"}
    ,
    {"poll_freq", "250", "poll_freq=n (n: polling frquency in ms)"}
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
    int poll_freq_val;

    /** Checking value for option "disable" */
    if (strcmp(mod_options[0].value, "true\0") == 0) {
        WARNING(("Setting module state to disabled."));
        mod_data.state = DISABLED;
    }
    else if (strcmp(mod_options[0].value, "false\0") == 0) {
        INFO(("Setting module state to enabled."));
        mod_data.state = ENABLED;
    }
    else {
        WARNING(("Wrong option value (%s) for option \"%s\".",
                 mod_options[0].value, mod_options[0].name));
        WARNING(("Setting module state to disabled."));
        strcpy(mod_options[0].value, "true");
        mod_data.state = DISABLED;
    }

    //* Checking value for option "poll_freq" */
    poll_freq_val = atoi(mod_options[1].value);
    if ((poll_freq_val < POLL_FREQ_MIN) || (poll_freq_val > POLL_FREQ_MAX)) {
        WARNING(("Bad value (%s) for polling frequence. Please use values between %i and %i.", mod_options[1].value, POLL_FREQ_MIN, POLL_FREQ_MAX));
        WARNING(("Using default value %i for polling frequence.",
                 POLL_FREQ_DEFAULT));
        sprintf(mod_options[1].value, "%i", POLL_FREQ_DEFAULT);
    }

    /* At this point we can be sure that atio(mod_options[1].value) gives resonable
     * value for the polling frequency.
     */

    INFO(("Setting polling frequency to %s ms.", mod_options[1].value));

    if (mod_data.state == DISABLED) {
        INFO(("%s is disabled and not going to be loaded.", mod_data.name));
        return FALSE;
    }

    return (toshiba_start());
}

gboolean mod_unload()
{
    return TRUE;
}
