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

#include "volume.h"
#include "volume-private.h"


////////////////////////////////////////////////////////////////////////////////

enum {
    PROP_0,
    
    PROP_UUID,
    PROP_NAME,
    PROP_DEVICE,
    PROP_IGNORE_MOUNT,
    PROP_IS_MOUNTED,
    PROP_IS_AVAILABLE,
    PROP_COMMAND_LIST,
    PROP_COMMAND_ON_MOUNT_LIST
};

////////////////////////////////////////////////////////////////////////////////
/* Function declaration... All functions being defined here are static and
therefore PRIVATE (can not be called outside this source file. */

static void 
umounter_volume_class_init(UMounterVolumeClass *cls);

static void
umounter_volume_init(UMounterVolume *self);

static void 
umounter_volume_dispose(GObject *gobject);

static void 
umounter_volume_finalize(GObject *gobject);

static void 
umounter_volume_set_property(GObject *object, guint property_id, 
    const GValue *value, GParamSpec *pspec);

static void 
umounter_volume_get_property(GObject *gobject, guint property_id, 
    GValue *value, GParamSpec *pspec);

////////////////////////////////////////////////////////////////////////////////

/* Macro for implementing the _get_type function and and defining a parent
pointer, which is accessible from the whole source file. */
G_DEFINE_TYPE(UMounterVolume, umounter_volume, G_TYPE_OBJECT);

static void
umounter_volume_dispose(GObject *gobject) {
    g_message(__FUNCTION__);

    UMounterVolume *self = UMOUNTER_VOLUME(gobject);
     
    /* In dispose, you are supposed to free all types referenced from this
    object which might themselves hold a reference to self. Generally,
    the most simple solution is to unref all members on which you own a 
    reference. */

    /* Chain up to the parent class. */
    G_OBJECT_CLASS(umounter_volume_parent_class)->dispose(gobject);
}

static void
umounter_volume_finalize(GObject *gobject) {
    UMounterVolume *self = UMOUNTER_VOLUME(gobject);

    /* Chain up to the parent class. */
    G_OBJECT_CLASS(umounter_volume_parent_class)->finalize(gobject);
}

static void
umounter_volume_set_property(GObject *gobject, guint property_id, 
    const GValue *value, GParamSpec *pspec) {

    UMounterVolume *self = UMOUNTER_VOLUME(gobject);

    switch(property_id) {
        case PROP_UUID:
            g_free(self->priv->uuid);
            self->priv->uuid = g_value_dup_string(value);
            break;
        case PROP_NAME:
            g_free(self->priv->name);
            self->priv->name = g_value_dup_string(value);
            break;
        case PROP_DEVICE:
            g_free(self->priv->device);
            self->priv->device = g_value_dup_string(value);
            break;
        case PROP_IGNORE_MOUNT:
            self->priv->ignore_mount = g_value_get_boolean(value);
            break;
        case PROP_IS_MOUNTED:
            self->priv->is_mounted = g_value_get_boolean(value);
            break;
        case PROP_IS_AVAILABLE:
            self->priv->is_available = g_value_get_boolean(value);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, property_id, pspec);
            break;
    }
}

static void
umounter_volume_get_property(GObject *gobject, guint property_id, 
    GValue *value, GParamSpec *pspec) {

    UMounterVolume *self = UMOUNTER_VOLUME(gobject);

    switch(property_id) {
        case PROP_UUID:
            g_value_set_string(value, self->priv->uuid);
            break;
        case PROP_NAME:
            g_value_set_string(value, self->priv->name);
            break;
        case PROP_DEVICE:
            g_value_set_string(value, self->priv->device);
            break;
        case PROP_IGNORE_MOUNT:
            g_value_set_boolean(value, self->priv->ignore_mount);
            break;
        case PROP_IS_MOUNTED:
            g_value_set_boolean(value, self->priv->is_mounted);
            break;
        case PROP_IS_AVAILABLE:
            g_value_set_boolean(value, self->priv->is_available);
            break;
        case PROP_COMMAND_LIST:
            g_value_set_pointer(value, self->priv->command_list);
            break;
        case PROP_COMMAND_ON_MOUNT_LIST:
            g_value_set_pointer(value, self->priv->command_on_mount_list);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, property_id, pspec);
            break;
    }
}

static void
umounter_volume_class_init(UMounterVolumeClass *cls) {
    GObjectClass *gobject_class = G_OBJECT_CLASS(cls);
    GParamSpec *pspec;

    /* Override different methods. */

    gobject_class->dispose = umounter_volume_dispose;
    gobject_class->finalize = umounter_volume_finalize;
    gobject_class->set_property = umounter_volume_set_property;
    gobject_class->get_property = umounter_volume_get_property;

    /* Set different properties. */

    pspec = g_param_spec_string("uuid", 
        "The uuid of the file.", "Set uuid.", "", G_PARAM_READWRITE | 
        G_PARAM_CONSTRUCT);
    g_object_class_install_property(gobject_class, PROP_UUID, pspec);

    pspec = g_param_spec_string("name", 
        "The name of the device.", "Set name.", "", G_PARAM_READWRITE | 
        G_PARAM_CONSTRUCT);
    g_object_class_install_property(gobject_class, PROP_NAME, pspec);

    pspec = g_param_spec_string("device", 
        "The device of the volume.", "Set device.", "", G_PARAM_READWRITE | 
        G_PARAM_CONSTRUCT);
    g_object_class_install_property(gobject_class, PROP_NAME, pspec);

    pspec = g_param_spec_boolean("ignore_mount",
        "The volume should be mounted or not.", "Set the ignore mount value.",
        FALSE, G_PARAM_READWRITE | G_PARAM_CONSTRUCT);
    g_object_class_install_property(gobject_class, PROP_IGNORE_MOUNT, pspec);

    pspec = g_param_spec_boolean("is_mounted", 
        "The volume is mounted or not.", "Set wether the volume is mounted.", 
        FALSE, G_PARAM_READWRITE | G_PARAM_CONSTRUCT);
    g_object_class_install_property(gobject_class, PROP_IS_MOUNTED, pspec);

    pspec = g_param_spec_boolean("is_available", 
        "The is added or not.", "Set wether the volume is present.", FALSE, 
        G_PARAM_READWRITE | G_PARAM_CONSTRUCT);
    g_object_class_install_property(gobject_class, PROP_IS_AVAILABLE, pspec);

    pspec = g_param_spec_pointer("command_list",
        "The list with alle commands that will be run when the volume will be"
        "added.", "The whole list.", G_PARAM_READABLE);
    g_object_class_install_property(gobject_class, PROP_COMMAND_LIST, pspec);

    pspec = g_param_spec_pointer("command_on_mount_list",
        "The list with alle commands that will be run when the volume will be"
        "mounted.", "The whole list.", G_PARAM_READABLE);
    g_object_class_install_property(gobject_class, PROP_COMMAND_ON_MOUNT_LIST, 
        pspec);

    /* Add private class... */

    g_type_class_add_private(cls, sizeof(UMounterVolumePrivate));
}

static void
umounter_volume_init(UMounterVolume *self) {
    self->priv = UMOUNTER_VOLUME_GET_PRIVATE(self);
    
    self->priv->command_list = NULL;
    self->priv->command_on_mount_list = NULL;
}

UMounterVolume*
umounter_volume_new(void) {
    UMounterVolume *volume = g_object_new(UMOUNTER_TYPE_VOLUME, NULL);

    return volume;
}

gboolean
umounter_volume_add_command(UMounterVolume *self, gchar *command) {
    self->priv->command_list = g_list_append(self->priv->command_list, command);

    return TRUE;
}

