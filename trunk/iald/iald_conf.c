#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#include "iald.h"
#include "iald_mod.h"
#include "iald_conf.h"

extern IalModule *modules_list_head;

void parse_module(xmlDocPtr doc, xmlNodePtr cur, xmlAttrPtr attr,
                  const char *conf_file)
{
    xmlChar *tok;
    xmlChar *val;
    IalModule *m = NULL;
    ModuleOption *opt = NULL;

    tok = xmlNodeListGetString(doc, attr->children, 1);

    DEBUG(("Configuration for module token \"%s\" found.", tok));

    m = modules_list_head;

    while (m) {
        if (!xmlStrcmp(tok, (const xmlChar *) m->data->token)) {
            while (cur != NULL) {
                opt = m->data->options;

                while (opt->name) {
                    if (!xmlStrcmp(cur->name, opt->name)) {
                        val = xmlNodeListGetString(doc, cur->children, 1);

                        if (strlen(val) <= MAX_BUF) {
                            strcpy(opt->value, val);
                            DEBUG(("%s: Setting option \"%s\" to \"%s\".", tok,
                                   opt->name, val));
                        }

                        xmlFree(val);

                        /* We're done. Leave while(opt->name) */
                        break;
                    }

                    *opt++;

                    /* Complain if a XML_ELEMENT_NODE was found but no
                     * corresponding module option is available.
                     *
                     */
                    if ((!opt->name) && cur->type == XML_ELEMENT_NODE) {
                        WARNING(("There is no such option \"%s\" for module \"%s\" (%s, line %i).", cur->name, m->data->token, conf_file, cur->line));
                    }
                }

                cur = cur->next;
            }

            /* We're done. Leave while(m) */
            break;
        }

        m = m->next;
        if (!m) {
            WARNING(("No module with token \"%s\" was found.", tok));
        }
    }

    xmlFree(tok);

    return;
}

void parse_config(const char *conf_file)
{
    xmlDocPtr doc;
    xmlNodePtr cur;
    xmlChar *key;
    xmlAttr *attr;

    xmlLineNumbersDefault(1);

    doc = xmlParseFile(conf_file);

    if (doc == NULL) {
        ERROR(("Configuration file %s could not be parsed successfully.",
               conf_file));
        return;
    }

    cur = xmlDocGetRootElement(doc);

    if (cur == NULL) {
        ERROR(("Configuration file %s is empty.", conf_file));
        xmlFreeDoc(doc);
        return;
    }

    if (xmlStrcmp(cur->name, (const xmlChar *) "ialdconfig")) {
        ERROR(("Configuration file root is not 'ialdconfig'."));
        xmlFreeDoc(doc);
        return;
    }

    cur = cur->children;

    while (cur != NULL) {
        if (!xmlStrcmp(cur->name, (const xmlChar *) "debug")) {
            key = xmlNodeListGetString(doc, cur->children, 1);

            INFO(("Found option \"debug\", value \"%s\".", key));
            log_level_set(atoi(key));

            xmlFree(key);
        }
        else if (!xmlStrcmp(cur->name, (const xmlChar *) "foreground")) {
            INFO(("Found foreground"));
        }
        else if (!xmlStrcmp(cur->name, (const xmlChar *) "module")) {
            if ((attr = xmlHasProp(cur, (const xmlChar *) "token")) != NULL) {
                parse_module(doc, cur->children->next, attr, conf_file);
            }
            else {
                ERROR((">%s", cur->name));
                ERROR(("Corrupt module definition found (%s, line %i).",
                       conf_file, cur->line));
            }

            attr = NULL;
        }

        cur = cur->next;
    }

    xmlFreeDoc(doc);
    return;
}

void conf_parse(void)
{
    const char *conf_file = SYSCONF_DIR "/iald/iald.conf";

    DEBUG(("Parsing configuration file %s.", conf_file));
    parse_config(conf_file);
    DEBUG(("Finished parsing configuration file."));
}
