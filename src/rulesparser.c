/* Copyright (c) Christian Krippendorf
 *
 * This file is part of UMounter.
 * 
 * UMounter is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * UMounter is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>. */

#include "rulesparser.h"
#include "rulesparser-private.h"

#include <libxml/tree.h>


////////////////////////////////////////////////////////////////////////////////
/* Function declaration... All functions being defined here are static and
therefore PRIVATE (can not be called outside this source file. */

static void 
umounter_rulesparser_class_init(UMounterRulesParserClass *cls);

static void
umounter_rulesparser_init(UMounterRulesParser *self);

static void 
umounter_rulesparser_dispose(GObject *gobject);

static void 
umounter_rulesparser_finalize(GObject *gobject);

static void 
umounter_rulesparser_set_property(GObject *object, guint property_id, 
    const GValue *value, GParamSpec *pspec);

static void 
umounter_rulesparser_get_property(GObject *gobject, guint property_id, 
    GValue *value, GParamSpec *pspec);

////////////////////////////////////////////////////////////////////////////////

/* Macro for implementing the _get_type function and and defining a parent
pointer, which is accessible from the whole source file. */
G_DEFINE_TYPE(UMounterRulesParser, umounter_rulesparser, G_TYPE_OBJECT);

static void
umounter_rulesparser_dispose(GObject *gobject) {
    g_message(__FUNCTION__);

    UMounterRulesParser *self = UMOUNTER_RULESPARSER(gobject);

     
    /* In dispose, you are supposed to free all types referenced from this
    object which might themselves hold a reference to self. Generally,
    the most simple solution is to unref all members on which you own a 
    reference. */

    /* Chain up to the parent class. */
    G_OBJECT_CLASS(umounter_rulesparser_parent_class)->dispose(gobject);
}

static void
umounter_rulesparser_finalize(GObject *gobject) {
    UMounterRulesParser *self = UMOUNTER_RULESPARSER(gobject);

    /* Chain up to the parent class. */
    G_OBJECT_CLASS(umounter_rulesparser_parent_class)->finalize(gobject);
}

static void
umounter_rulesparser_set_property(GObject *gobject, guint property_id, 
    const GValue *value, GParamSpec *pspec) {

    UMounterRulesParser *self = UMOUNTER_RULESPARSER(gobject);

    switch(property_id) {
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, property_id, pspec);
            break;
    }
}

static void
umounter_rulesparser_get_property(GObject *gobject, guint property_id, 
    GValue *value, GParamSpec *pspec) {

    UMounterRulesParser *self = UMOUNTER_RULESPARSER(gobject);

    switch(property_id) {
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, property_id, pspec);
            break;    
    }
}

static void
umounter_rulesparser_class_init(UMounterRulesParserClass *cls) {
    GObjectClass *gobject_class = G_OBJECT_CLASS(cls);
    GParamSpec *pspec;

    /* Override different methods. */

    gobject_class->dispose = umounter_rulesparser_dispose;
    gobject_class->finalize = umounter_rulesparser_finalize;
    gobject_class->set_property = umounter_rulesparser_set_property;
    gobject_class->get_property = umounter_rulesparser_get_property;

    /* Set different properties. */

    /* Add private class... */

    g_type_class_add_private(cls, sizeof(UMounterRulesParserPrivate));
}

static void
umounter_rulesparser_init(UMounterRulesParser *self) {
    self->priv = UMOUNTER_RULESPARSER_GET_PRIVATE(self);
}

static xmlDoc*
umounter_rulseparser_get_doc(UMounterRulesParser *self, 
    const gchar *file_path) {

    g_return_val_if_fail(NULL != self, NULL);
    g_return_val_if_fail(NULL != file_path, NULL);

    xmlDoc *doc;
    doc = xmlParseFile(file_path);
    if(doc == NULL) {
        g_warning("File could not be parsed: %s", file_path);
        return NULL;
    }

    return doc;
}

static xmlNode*
umounter_rulesparser_get_root_node(UMounterRulesParser *self, xmlDoc *doc) {
    
    g_return_val_if_fail(NULL != self, NULL);
    g_return_val_if_fail(NULL != doc, NULL);

    xmlNode *root_node;
    root_node = xmlDocGetRootElement(doc);
    if(root_node == NULL) {
        g_warning("Maybe the document is empty.");
        return NULL;
    }

    return root_node;
}

UMounterRulesParser* 
umounter_rulesparser_new(void) {
    UMounterRulesParser *rulesparser = g_object_new(UMOUNTER_TYPE_RULESPARSER,
        NULL);

    return rulesparser;
}

UMounterVolumes*
umounter_rulesparser_parse(UMounterRulesParser *self, const gchar *path) {
    g_return_val_if_fail(NULL != path, NULL);

    xmlDoc *doc;
    xmlNode *cur_node;
    GFile *rules_dir;
    GFileEnumerator *enumerator;
    GError *error;

    error = NULL;
    rules_dir = g_file_new_for_path(path);

    enumerator = g_file_enumerate_children(rules_dir, "*", 
        G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS, NULL, &error);
    if(NULL != error) {
        g_warning("Can't parse rule: %s : %s", error->message, path);

        g_object_unref(rules_dir);
        g_error_free(error);

        return NULL;
    }

    /* Get file info for every file. */
    GFileInfo *file_info = NULL;
    error = NULL;
    do {
        /* If there is a object from the last loop, unref it. */
        if(NULL != file_info)
            g_object_unref(file_info);

        file_info = g_file_enumerator_next_file(enumerator, NULL, &error);
        if(NULL != error) {
            g_warning(error->message);

            g_object_unref(rules_dir);
            g_object_unref(enumerator);
            g_error_free(error);

            return NULL;
        } else if(NULL != file_info) {
            gchar* file_name = (gchar*) g_file_info_get_attribute_string(
                file_info, G_FILE_ATTRIBUTE_STANDARD_DISPLAY_NAME);

            /* Look if the string ends with .rule */
            if(FALSE == g_str_has_suffix(file_name, ".rule")) {
                g_free(file_name);
                continue;
            }
            
            g_print("%s\n", g_build_path("/", path, g_file_info_get_attribute_string(
                file_info, G_FILE_ATTRIBUTE_STANDARD_DISPLAY_NAME), NULL));
        }
    } while(NULL != file_info);

    return NULL;

//    doc = umounter_rulesparser_get_doc(self, file_path);

//    if(xmlStrcmp(cur_node->name, "story")) {
//        fprintf(stderr,"document of the wrong type, root node != story");
//        xmlFreeDoc(doc);
//        return;
//    }
}


