/* iald_conf.c - Input Abstraction Layer Configuration File Parser
 *
 * Copyright (C) 2004, 2005 Timo Hoenig <thoenig@nouse.net>
 *                          All rights reserved
 *
 * Licensed under the Academic Free License version 2.1
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "iald.h"
#include "iald_mod.h"
#include "iald_conf.h"

extern IalModule *modules_list_head;

/**
 * @addtogroup IAL
 * @{
 */

/**         
 * @defgroup IALCONF Configuration File Parser
 * @ingroup  IAL
 * @brief    The Input Abstraction Layer configuration file parser uses libxml2 to parse
 *           the configuration file `iald.conf`. The configuration file includes both options
 *           for the IAL daemon and IAL modules.
 * @{
 */

/**
 * Parse a module node found in the configuration file.
 *
 * @param       doc             Pointer to XML document.
 * @param       cur             Ponter to XML node.
 * @param       attr            Pointer to XML attribute.
 * @param       conf_file       Pointer to the configuration file.
 */

void conf_parse_module(xmlDocPtr doc, xmlNodePtr cur, xmlAttrPtr attr,
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
                            DEBUG(("%s: Setting option \"%s\" to \"%s\".",
                                   tok, opt->name, val));
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

/**
 * Parse Input Abstraction Layer Daemon configuration file using libxml2.
 *
 * @param conf_file     Location of configuration file.
 */

void conf_parse_file(const char *conf_file)
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
            opt_debug_set(atoi(key));

            xmlFree(key);
        }
        else if (!xmlStrcmp(cur->name, (const xmlChar *) "logfile")) {
            key = xmlNodeListGetString(doc, cur->children, 1);

            INFO(("Found option \"logfile\", value \"%s\".", key));
            opt_logfile_set(key);

            xmlFree(key);
        }
        else if (!xmlStrcmp(cur->name, (const xmlChar *) "foreground")) {
            key = xmlNodeListGetString(doc, cur->children, 1);
                        
            INFO(("Found option \"foreground\", value \"%s\".", key));
            if (!xmlStrcmp(key, (const xmlChar *) "false")) {
                opt_foreground_set(FALSE);
            }
            else if (!xmlStrcmp(key, (const xmlChar *) "true")) {
                opt_foreground_set(TRUE);
            }
            else {
                WARNING(("Invalid value (\"%s\") for option \"foreground\" supplied. Must be either \"true\" or \"false\".)", key));
            }

            xmlFree(key);
            
        }
        else if (!xmlStrcmp(cur->name, (const xmlChar *) "module")) {
            if ((attr =
                 xmlHasProp(cur, (const xmlChar *) "token")) != NULL) {
                conf_parse_module(doc, cur->children->next, attr,
                                  conf_file);
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

/**
 * Invoke parser for Input Abstraction Layer Daemon configuration file.
 */
void conf_parse()
{
    const char *conf_file = SYSCONF_DIR "/ial/iald.conf";

    DEBUG(("Parsing configuration file %s.", conf_file));
    conf_parse_file(conf_file);
    DEBUG(("Finished parsing configuration file."));
}

/** @} */
