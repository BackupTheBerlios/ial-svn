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

/** Allocate a given amount of bytes of memory. Aborts if no memory is left.
 *
 *  @param  amount              Number of bytes to allocated
 *  @return                     Pointer to allocated memory
 */
void *xmalloc(unsigned int amount)
{
    void *p = malloc(amount);
    if (!p) {
        exit(1);
    }
    return p;
}

/** String duplication; aborts if no memory.
 *
 *  @param  how_much            Number of bytes to allocated
 *  @return                     Pointer to allocated storage
 */
char *xstrdup(const char *str)
{
    char *p = strdup(str);
    if (!p) {
        exit(1);
    }
    return p;
}

void *dl_function(char *filename, const char *symbol)
{
    void *handle;
    char *error;
    void (*function) (void);

    handle = NULL;
    error = NULL;

    handle = dlopen(filename, RTLD_LAZY);

    if (!handle) {
        WARNING(("%s is not a shared library.", filename));
        return NULL;
    }

    function = dlsym(handle, symbol);
    if ((error = dlerror()) != NULL) {
        WARNING(("%s (%s).", error, filename));
        return NULL;
    }

    return function;
}

/** Add a new module.
 *
 *  @return                     Nothing 
 */
void module_add(char *filename)
{
    ModuleData *(*function) (void);
    IalModule *module;

    module = (IalModule *) xmalloc(sizeof(IalModule));

    function = dl_function(filename, "mod_get_data");
    module->data = function();

    module->prev = NULL;
    module->next = modules_list_head;

    if (module->next != NULL)
        module->next->prev = module;

    modules_list_head = module;
}

void module_remove(IalModule * module)
{

}

/* if return is true, the module initializes successfully */
gboolean module_init(char *filename)
{
    gboolean(*mod_init) (void);

    mod_init = dl_function(filename, "mod_init");

    return (*mod_init) ();
}

/* Check if module has all neccessary symbols */
gboolean module_verify(char *filename)
{
    void (*function) (void);

    DEBUG(("Checking %s for symbol.", filename));

    function = dl_function(filename, "mod_get_data");
    if (!function) {
        WARNING(("%s is not a valid module.", filename));
        return FALSE;
    }

    DEBUG(("Symbol found."));

    return TRUE;
}

/* Heavily based on scan_plugins() by XMMS (http://www.xmms.org) */

void modules_scan()
{
    char *filename, *extension;
    char *module_dir = MODULE_DIR;
    DIR *dir;
    struct dirent *entry;
    struct stat statbuf;

    dir = opendir(module_dir);
    if (!dir)
        return;

    while ((entry = readdir(dir)) != NULL) {
        filename = g_strdup_printf("%s/%s", module_dir, entry->d_name);
        if (!stat(filename, &statbuf) && S_ISREG(statbuf.st_mode) &&
            (extension = strrchr(entry->d_name, '.')) != NULL) {
            if (!strcmp(extension, ".so")) {
                if (module_verify(filename) == TRUE) {
                    module_add(filename);
                }
                else {
                    DEBUG(("Verification of \"%s\" failed.", filename));
                }
            }
        }
        g_free(filename);
    }
    closedir(dir);

}

void modules_load()
{
    IalModule *m;

    m = modules_list_head;

    if (modules_list_head == NULL) {
        modules_scan();
        m = modules_list_head;
    }

    if (m == NULL) {
        ERROR(("No modules found."));
    }

    /** Try to start the modules. */
    while (m) {
        if (m->data->load() == FALSE)
            WARNING(("Failed to load %s.", m->data->name));
        else
            INFO(("%s loaded.", m->data->name));

        m = m->next;
    }

}

void modules_unload()
{
    ERROR(("QQQ"));

}
