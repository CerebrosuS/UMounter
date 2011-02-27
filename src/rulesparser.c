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

static xmlDoc*
umounter_rulesparser_get_doc(UMounterRulesParser *self, const gchar *file_path);

static xmlNode*
umounter_rulesparser_get_root_node(UMounterRulesParser *self, xmlDoc *doc);

////////////////////////////////////////////////////////////////////////////////

/* Macro for implementing the _get_type function and and defining a parent
pointer, which is accessible from the whole source file. */
G_DEFINE_TYPE(UMounterRulesParser, umounter_rulesparser, G_TYPE_OBJECT);

GQuark
umounter_rulesparser_error_quark(void) {
    return g_quark_from_static_string("umounter-rulesparser-error-quark");
}

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
umounter_rulesparser_get_doc(UMounterRulesParser *self, 
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

static UMounterVolume*
umounter_rulesparser_parse_rule(UMounterRulesParser *self, xmlDoc *doc, xmlNode 
    *root_node, GError **error) {

    g_return_val_if_fail(NULL != self, NULL);
    g_return_val_if_fail(NULL != doc, NULL);
    g_return_val_if_fail(NULL != root_node, NULL);
    g_return_val_if_fail(NULL == error || NULL == *error, NULL);

    UMounterVolume *volume;
    xmlNode *current_node;


    /* Create new volume to return. */
    volume = umounter_volume_new();

    /* The root node must have a special name. */
    if(0 != g_strcmp0(root_node->name, "volume")) {
		g_set_error(error, UMOUNTER_RULESPARSER_ERROR, 
            UMOUNTER_RULESPARSER_ERROR_PARSING, 
            "Wrong tag name. Should be <volume> not %s", root_node->name);
		
        g_object_unref(volume);

		return NULL;
	}

    /* Now we can get the childs of the root node and go through all of them
    with a loop. */
    current_node = root_node->xmlChildrenNode;
    while(NULL != current_node) {
        
        /* Lets look for known tag names. */
        if(0 == g_strcmp0(current_node->name, "name")) {
            g_object_set(G_OBJECT(volume), (const gchar*)"name", 
                xmlNodeListGetString(doc, current_node->xmlChildrenNode, 1), 
                NULL);
        } else if(0 == g_strcmp0(current_node->name, "uuid")) {
            g_object_set(G_OBJECT(volume), (const gchar*)"uuid", 
                xmlNodeListGetString(doc, current_node->xmlChildrenNode, 1), 
                NULL);
        } else if(0 == g_strcmp0(current_node->name, "ignore_mount")) {
            g_object_set(G_OBJECT(volume), (const gchar*)"ignore_mount", TRUE,
                NULL);
            g_debug("Set ignore_mount to TRUE.");
        } else if(0 == g_strcmp0(current_node->name, "command")) {
            umounter_volume_add_command(volume, xmlNodeListGetString(doc, 
                current_node->xmlChildrenNode, 1));
        } else if(0 == g_strcmp0(current_node->name, "command_on_mount")) {
            umounter_volume_add_command(colume, xmlNodeListGetString(doc,
                current_node->xmlChildrenNode, 1));
        }

        current_node = current_node->next;
    }

    return volume;
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

    /* Needed variables... */
    xmlDoc *doc;
    xmlNode *root_node;

    GFile *rules_dir;
    GFileInfo *file_info;
    GFileEnumerator *enumerator;
    GError *error;

    const gchar* file_name;
    gchar* file_path;

    UMounterVolumes *volumes;

    
    /* Init volumes with a new object. */
    volumes = umounter_volumes_new();

    /* Error must be set to NULL. */
    error = NULL;

    /* Create a new file object which is actually the rules dir. */
    rules_dir = g_file_new_for_path(path);

    /* Get a list of all file info objects of the rules dir. */
    enumerator = g_file_enumerate_children(rules_dir, "*", 
        G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS, NULL, &error);

    /* If error is not NULL there must be an error while getting the file info
    objects. */
    if(NULL != error) {
        g_warning("Can't parse rule: %s : %s", error->message, path);

        /* Clean memory of created datas... */
        g_object_unref(rules_dir);
        g_error_free(error);

        return NULL;
    }

    /* Set the file_info initial to NULL, otherwise the algo below won't work
    as expected. */
    file_info = NULL;

    /* Now we can go throught all existing file info objects and have a look,
    if this file is a rule file or not. */
    do {

        /* If there is a object from the last loop, unref it. */
        if(NULL != file_info)
            g_object_unref(file_info);

        /* Get every next file info in the enumerator. */
        file_info = g_file_enumerator_next_file(enumerator, NULL, &error);

        /* If any error occured getting the next file info, free space and
        display the error. */
        if(NULL != error) {
            g_warning(error->message);

            g_object_unref(rules_dir);
            g_object_unref(enumerator);
            g_error_free(error);

            return NULL;
        }

        /* If the file info object is NULL, but there was no error, we are at
        the end of the file info objects. So lets continue the loop, which will
        then exit. */ 
        if(NULL == file_info)
            continue;

        /* Get the file name of an existing file info object. */
        file_name = g_file_info_get_attribute_string(
            file_info, G_FILE_ATTRIBUTE_STANDARD_DISPLAY_NAME);

        /* If the file name does not exist, we should skip the rest and continue
        the loop. */
        if(NULL == file_name)
            continue;

        /* Lets have a look if the file_name got the right suffix. Else we will
        conitnue the loop. */
        if(FALSE == g_str_has_suffix(file_name, ".rule"))
            continue;

        /* Try to get the full path of the file_name...*/
        file_path = (gchar*) g_build_path("/", path, file_name, NULL);

        /* ...and parse the file for getting the doc object. */
        doc = umounter_rulesparser_get_doc(self, file_path);

        /* After that, we can free the file_path, cause we won't need the file
        path any longer. */
        g_free(file_path);

        /* If the creation of the document failed with a NULL return, we should
        give a message and continue with the loop. */
        if(NULL == doc)
            continue;

        /* When all went ok, we can create the root node... */
        root_node = umounter_rulesparser_get_root_node(self, doc);

        /* ...and parse it to get the volume. */
        UMounterVolume *volume = umounter_rulesparser_parse_rule(
            self, doc, root_node, &error);

        if(NULL != error) {
            g_warning(error->message);
            g_clear_error(&error);
        }        
        
        /* If parsing the root node fail, we will go on with the loop. */    
        if(NULL != volume) {
            if(FALSE == umounter_volumes_add(volumes, volume)) {
                g_object_unref(volume);
            } else {
                g_debug("Adding a new volume entry.");
            }
        }

        xmlFree(doc);
        xmlFree(root_node);
    } while(NULL != file_info);

    return volumes;
}


