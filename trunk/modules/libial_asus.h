#define MODULE_NAME "Asus Input Abstraction Layer Module"
#define MODULE_VERSION "0.0.1"
#define MODULE_AUTHOR "Timo Hoenig <thoenig@nouse.net>"

#include "../libial/libial.h"


const char *mod_get_name(void);
const char *mod_get_version(void);
const char *mod_get_author(void);
const char *mod_get_descr(void);
const int  mod_get_type(void);
gboolean   mod_init(void);

