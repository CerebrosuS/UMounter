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

#include "volumes.h"
#include "volumes-private.h"


////////////////////////////////////////////////////////////////////////////////

enum {
    PROP_0,
};

////////////////////////////////////////////////////////////////////////////////
/* Function declaration... All functions being defined here are static and
therefore PRIVATE (can not be called outside this source file. */

static void 
umounter_volumes_class_init(UMounterVolumesClass *cls);

static void
umounter_volumes_init(UMounterVolumes *self);

static void 
umounter_volumes_dispose(GObject *gobject);

static void 
umounter_volumes_finalize(GObject *gobject);

static void 
umounter_volumes_set_property(GObject *object, guint property_id, 
    const GValue *value, GParamSpec *pspec);

static void 
umounter_volumes_get_property(GObject *gobject, guint property_id, 
    GValue *value, GParamSpec *pspec);

static UMounterVolume*
umounter_volumes_exist_generic(UMounterVolumes *self, const gchar *value,
    gint type);

////////////////////////////////////////////////////////////////////////////////

/* Macro for implementing the _get_type function and and defining a parent
pointer, which is accessible from the whole source file. */
G_DEFINE_TYPE(UMounterVolumes, umounter_volumes, G_TYPE_OBJECT);

static void
umounter_volumes_dispose(GObject *gobject) {
    g_message(__FUNCTION__);

    UMounterVolumes *self = UMOUNTER_VOLUMES(gobject);
     
    /* In dispose, you are supposed to free all types referenced from this
    object which might themselves hold a reference to self. Generally,
    the most simple solution is to unref all members on which you own a 
    reference. */

    /* Chain up to the parent class. */
    G_OBJECT_CLASS(umounter_volumes_parent_class)->dispose(gobject);
}

static void
umounter_volumes_finalize(GObject *gobject) {
    UMounterVolumes *self = UMOUNTER_VOLUMES(gobject);

    /* Chain up to the parent class. */
    G_OBJECT_CLASS(umounter_volumes_parent_class)->finalize(gobject);
}

static void
umounter_volumes_set_property(GObject *gobject, guint property_id, 
    const GValue *value, GParamSpec *pspec) {

    UMounterVolumes *self = UMOUNTER_VOLUMES(gobject);

    switch(property_id) {
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, property_id, pspec);
            break;
    }
}

static void
umounter_volumes_get_property(GObject *gobject, guint property_id, 
    GValue *value, GParamSpec *pspec) {

    UMounterVolumes *self = UMOUNTER_VOLUMES(gobject);

    switch(property_id) {  
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, property_id, pspec);
            break;
    }
}

static void
umounter_volumes_class_init(UMounterVolumesClass *cls) {
    GObjectClass *gobject_class = G_OBJECT_CLASS(cls);
    GParamSpec *pspec;

    /* Override different methods. */

    gobject_class->dispose = umounter_volumes_dispose;
    gobject_class->finalize = umounter_volumes_finalize;
    gobject_class->set_property = umounter_volumes_set_property;
    gobject_class->get_property = umounter_volumes_get_property;

    /* Set different properties. */

    /* Add private class... */

    g_type_class_add_private(cls, sizeof(UMounterVolumesPrivate));
}

static void
umounter_volumes_init(UMounterVolumes *self) {
    self->priv = UMOUNTER_VOLUMES_GET_PRIVATE(self);
    self->priv->volumes = NULL;
}

static UMounterVolume*
umounter_volumes_exist_generic(UMounterVolumes *self, const gchar *value,
    gint type) {

    g_return_val_if_fail(NULL != self, NULL);
    g_return_val_if_fail(NULL != value, NULL);

    gint count_volumes = (NULL == self->priv->volumes) ? g_list_length(
        self->priv->volumes) : 0;
    gint loop_counter;
    for(loop_counter = 0; loop_counter < count_volumes; loop_counter++) {
        UMounterVolume *volume = (UMounterVolume*) g_list_nth(
            self->priv->volumes, loop_counter);
        gchar *volume_type_value;
        gchar *type_name;
        switch(type) {
            case VOLUME_NAME:
                type_name = "name";
                break;
            case VOLUME_UUID:
                type_name = "uuid";
                break;
            case VOLUME_DEVICE:
                type_name = "device";
                break;
            default:
                type_name = NULL;
        }
        
        if(NULL == type_name) {
            g_warning("Type could not be named.");
            return NULL;
        }

        g_object_get(G_OBJECT(volume), type_name, &volume_type_value);

        if(g_strcmp0(volume_type_value, value) == 0)
            return volume;
    }
}

gboolean
umounter_volumes_add(UMounterVolumes *self, UMounterVolume *volume) {
    const gchar *name;
    const gchar *uuid;

    g_object_get(G_OBJECT(volume), "name", &name, NULL);
    g_object_get(G_OBJECT(volume), "uuid", &uuid, NULL);

    if(NULL != umounter_volumes_exist_name(self, name))
        return FALSE;

    self->priv->volumes = g_list_append(self->priv->volumes, volume);

    return TRUE; 
}

UMounterVolumes*
umounter_volumes_new(void) {
    UMounterVolumes *volumes = g_object_new(UMOUNTER_TYPE_VOLUMES, NULL);

    return volumes;
}

UMounterVolume*
umounter_volumes_exist_name(UMounterVolumes *self, const gchar *name) {
    return umounter_volumes_exist_generic(self, name, VOLUME_NAME);
}

UMounterVolume*
umounter_volumes_exist_uuid(UMounterVolumes *self, const gchar *uuid) {
    return umounter_volumes_exist_generic(self, uuid, VOLUME_UUID);
}

UMounterVolume*
umounter_volumes_exist_device(UMounterVolumes *self, const gchar *device) {
    return umounter_volumes_exist_generic(self, device, VOLUME_DEVICE);
}

