#define MAX_BUF 512

#define INPUT           0
#define OUTPUT          1
#define INPUT_OUTPUT    2

#define ENABLED         TRUE
#define DISABLED        FALSE

typedef struct ModuleOption_s {
    const char *name;
    char value[MAX_BUF];
    const char *descr;
} ModuleOption;

typedef struct ModuleData_s {
    const char *name;
    const char *token;
    const char *version;
    const char *author;
    const char *descr;

    int type;

    ModuleOption *options;

    gboolean initialized;
    gboolean state;

    gboolean(*load) (void);
    gboolean(*unload) (void);
} ModuleData;

ModuleData *mod_get_data(void);
