#include "libial_toshiba.h"

extern ModuleOption mod_options[];
extern ModuleData mod_data;

IalEvent event;
static struct fnkey_s fnkey;

void toshiba_event_send()
{
    event.sender = mod_data.token;
    event.source = ACPI_TOSHIBA_KEYS;
    event.name = toshiba_fnkey_description(fnkey.value);
    event.raw = fnkey.value;

    /** Sending the event with libial */
    event_send(&event);
}

gboolean toshiba_acpi_check()
{
    fnkey.fp = fopen(ACPI_TOSHIBA_KEYS, "r+");

    if (!fnkey.fp) {
        ERROR(("Could not open %s.", ACPI_TOSHIBA_KEYS));
        return FALSE;
    }

    fclose(fnkey.fp);
    return TRUE;
}

void toshiba_key_flush()
{
    int flush_count = -1;
    fnkey.fp = fopen(ACPI_TOSHIBA_KEYS, "r+");

    if (!fnkey.fp) {
        ERROR(("Could not open %s.", ACPI_TOSHIBA_KEYS));
        return;
    }
    else {
        fnkey.hotkey_ready = 1;

        while (fnkey.hotkey_ready) {
            flush_count++;

            fprintf(fnkey.fp, "hotkey_ready:0");
            fclose(fnkey.fp);

            fnkey.fp = fopen(ACPI_TOSHIBA_KEYS, "r+");
            fscanf(fnkey.fp, "hotkey_ready: %d\nhotkey: 0x%4x", &fnkey.hotkey_ready, &fnkey.value);
        }

        if (fnkey.fp)
            fclose(fnkey.fp);

        if (flush_count)
            INFO(("Flushed %i keys from %s.", flush_count, ACPI_TOSHIBA_KEYS));
    }
}

gboolean toshiba_key_ready()
{
    fnkey.fp = fopen(ACPI_TOSHIBA_KEYS, "r+");

    if (!fnkey.fp) {
        /* TODO remove module, panic */
        return FALSE;
    }

    fscanf(fnkey.fp, "hotkey_ready: %1d\nhotkey: 0x%4x", &fnkey.hotkey_ready, &fnkey.value);

    if (fnkey.hotkey_ready) {
        /** Signal the driver that we have read the key */
        fprintf(fnkey.fp, "hotkey_ready:0");
        fclose(fnkey.fp);
        return TRUE;
    }
    else {
        fclose(fnkey.fp);
        return FALSE;
    }
}

gboolean toshiba_key_poll()
{
    while (toshiba_key_ready() == TRUE) {

        /* If we have a description it is a known key.
         * otherwise we have either a key up event
         * or some unkown key.
         */

        fnkey.description = toshiba_fnkey_description(fnkey.value);

        if (fnkey.description) {
            toshiba_event_send();
        }
        else {
            if (!toshiba_fnkey_description(fnkey.value - 0x80) && (fnkey.value != FN)) {
                INFO(("Unknown key event (0x%x). Please report to <thoenig at nouse dot net>", fnkey.value));
            }

        }
    }

    return TRUE;
}

gboolean toshiba_start()
{
    DBusError dbus_error;

    dbus_error_init(&dbus_error);

    if (toshiba_acpi_check() == FALSE) {
        ERROR(("Failed to access the Toshiba ACPI interface."));
        return FALSE;
    }

    toshiba_key_flush();

    if (!(g_timeout_add(atoi(mod_options[1].value), (GSourceFunc) toshiba_key_poll, NULL))) {
        ERROR(("g_timeout_add() for toshiba_key_poll() failed."));
        return FALSE;
    }

    if (!(toshiba_add_filter())) {
        ERROR(("toshiba_add_filter() failed."));
        return FALSE;
    }
    return TRUE;
}
