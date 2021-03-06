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

#include "config.h"
#include "config-private.h"


////////////////////////////////////////////////////////////////////////////////

enum {
    PROP_0,
    
    /* UMounter config variables. */
    
    PROP_RULES_PATH,
    PROP_AUTOMOUNT
};

////////////////////////////////////////////////////////////////////////////////
/* Function declaration... All functions being defined here are static and
therefore PRIVATE (can not be called outside this source file. */

static void 
umounter_config_class_init(UMounterConfigClass *cls);

static void
umounter_config_init(UMounterConfig *self);

static void 
umounter_config_dispose(GObject *gobject);

static void 
umounter_config_finalize(GObject *gobject);

static void 
umounter_config_set_property(GObject *object, guint property_id, 
    const GValue *value, GParamSpec *pspec);

static void 
umounter_config_get_property(GObject *gobject, guint property_id, 
    GValue *value, GParamSpec *pspec);

////////////////////////////////////////////////////////////////////////////////

/* Macro for implementing the _get_type function and and defining a parent
pointer, which is accessible from the whole source file. */
G_DEFINE_TYPE(UMounterConfig, umounter_config, G_TYPE_OBJECT);

static void
umounter_config_dispose(GObject *gobject) {
    g_message(__FUNCTION__);

    UMounterConfig *self = UMOUNTER_CONFIG(gobject);
     
    /* In dispose, you are supposed to free all types referenced from this
    object which might themselves hold a reference to self. Generally,
    the most simple solution is to unref all members on which you own a 
    reference. */

    /* Chain up to the parent class. */
    G_OBJECT_CLASS(umounter_config_parent_class)->dispose(gobject);
}

static void
umounter_config_finalize(GObject *gobject) {
    UMounterConfig *self = UMOUNTER_CONFIG(gobject);

    if(NULL != self->priv->config_key_file)
        g_key_file_free(self->priv->config_key_file);

    if(NULL != self->priv->config_path)
        g_free(self->priv->config_path);

    /* Chain up to the parent class. */
    G_OBJECT_CLASS(umounter_config_parent_class)->finalize(gobject);
}

static void
umounter_config_set_property(GObject *gobject, guint property_id, 
    const GValue *value, GParamSpec *pspec) {

    UMounterConfig *self = UMOUNTER_CONFIG(gobject);

    switch(property_id) {
        case PROP_RULES_PATH:
            g_free(self->priv->rules_path);
            self->priv->rules_path = g_value_dup_string(value);
            break;
        case PROP_AUTOMOUNT:
            self->priv->automount = g_value_get_boolean(value);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, property_id, pspec);
            break;
    }
}

static void
umounter_config_get_property(GObject *gobject, guint property_id, 
    GValue *value, GParamSpec *pspec) {

    UMounterConfig *self = UMOUNTER_CONFIG(gobject);

    switch(property_id) {
        case PROP_RULES_PATH:
            g_value_set_string(value, self->priv->rules_path);
            break;
        case PROP_AUTOMOUNT:
            g_value_set_boolean(value, self->priv->automount);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, property_id, pspec);
            break;
    }
}

static void
umounter_config_class_init(UMounterConfigClass *cls) {
    GObjectClass *gobject_class = G_OBJECT_CLASS(cls);
    GParamSpec *pspec;

    /* Override different methods. */

    gobject_class->dispose = umounter_config_dispose;
    gobject_class->finalize = umounter_config_finalize;
    gobject_class->set_property = umounter_config_set_property;
    gobject_class->get_property = umounter_config_get_property;

    /* Set different properties. */

    pspec = g_param_spec_string("rules_path",
        "The path of the rules files.", "Set rules path.", NULL, 
        G_PARAM_READWRITE | G_PARAM_CONSTRUCT);
    g_object_class_install_property(gobject_class, PROP_RULES_PATH, pspec);

    pspec = g_param_spec_boolean("automount",
        "This option can stop whole automounting.", "Set automounting option.",
        TRUE, G_PARAM_READWRITE | G_PARAM_CONSTRUCT);
    g_object_class_install_property(gobject_class, PROP_AUTOMOUNT,
        pspec);

    /* Add private class... */

    g_type_class_add_private(cls, sizeof(UMounterConfigPrivate));
}

static void
umounter_config_init(UMounterConfig *self) {
    self->priv = UMOUNTER_CONFIG_GET_PRIVATE(self);

    self->priv->config_key_file = NULL;
}

UMounterConfig*
umounter_config_new(void) {
    UMounterConfig *config = g_object_new(UMOUNTER_TYPE_CONFIG, 
        NULL);

    return config;
}

gboolean
umounter_config_read(UMounterConfig *self, const gchar *config_path, 
    GError **error) {

    GKeyFile *key_file;
    gboolean automount;


    if(NULL != self->priv->config_key_file) {
        g_key_file_free(self->priv->config_key_file);
        self->priv->config_key_file = NULL;
    }

    key_file = g_key_file_new();
    g_key_file_set_list_separator(key_file, ',');

    if(!g_key_file_load_from_file(key_file, config_path, G_KEY_FILE_NONE,
        error)) {
        
        g_key_file_free(key_file);
        
        return FALSE;
    }

    /* From this point on, we cann read in the configuration variables. */
    gchar *rules_path = g_key_file_get_string(key_file, "General", "rules_path", 
        error);
    if(*error == NULL) {
        g_free(self->priv->rules_path);
        self->priv->rules_path = rules_path;
    } else {
        g_error_free(*error);
        *error = NULL;
    }

    /* Get the automounting option. */
    automount = g_key_file_get_boolean(key_file, "General", 
        "automount", error);
    
    if(NULL != *error) {
        automount = TRUE;

        g_error_free(*error);
        *error = NULL;
    }

    g_object_set(G_OBJECT(self), "automount", automount, 
        NULL);
    
    self->priv->config_key_file = key_file;

    return TRUE;
}

