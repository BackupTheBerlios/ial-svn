#include "libial_toshiba.h"

const char *toshiba_fnkey_description(int key_value)
{
    struct key *keylist = keys;

    while (keylist->value) {
        if (keylist->value == key_value) {
            return keylist->descr;
        }
        else {
            *keylist++;
        }
    }
    return NULL;
}
