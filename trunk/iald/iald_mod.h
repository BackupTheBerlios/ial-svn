#include <glib.h>

typedef struct IalModule_s
{
    ModuleData *data;

    struct IalModule_s* prev;
    struct IalModule_s* next;
}
IalModule;

/** Function prototypes */
void* xmalloc(unsigned int);
char* xstrdup(const char*);
void *dl_function(char *, const char *);

/* Add/Remove a single modules to the configuration */
void module_add(char *);
void module_remove(IalModule*);

gboolean module_init(char *);
gboolean module_verify(char *);

/* Scan MODULES_DIR for modules */
void modules_scan(void);

/* Load/Unload all modules */
void modules_load(void);
void modules_unload(void);


