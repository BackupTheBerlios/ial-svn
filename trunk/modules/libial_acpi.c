#include "libial_acpi.h"

ModuleOption mod_options[] = {
    {"disable", "false", "disable=(true|false)"},
    {"", "", ""}
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
    /** Checking value for option "disable" */
    if (strcmp(mod_options[0].value, "true\0") == 0)
    {   
        WARNING(("Setting module state to disabled."));
        mod_data.state = DISABLED;
    }
    else if (strcmp(mod_options[0].value, "false\0") == 0)
    {   
        INFO(("Setting module state to enabled."));
        mod_data.state = ENABLED;
    }
    else
    {   
        WARNING(("Wrong option value (%s) for option \"%s\".",
               mod_options[0].value, mod_options[0].name));
        WARNING(("Setting module state to disabled."));
        strcpy(mod_options[0].value, "true");
        mod_data.state = DISABLED;
    }

    if(mod_data.state == DISABLED)
    {
        INFO(("%s is disabled and not going to be loaded.", mod_data.name));
        return FALSE;
    }

    return(libial_acpi_start());
}

gboolean mod_unload()
{
    return TRUE;
}
