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
    g_return_val_if_fail(type > VOLUME_START && type < VOLUME_END, NULL); 

    UMounterVolume *volume;
    gint count_volumes;
    gchar *type_value, *type_name;
    gint i;


    /* Get the count of elements from the volumes list. */
    count_volumes = (self->priv->volumes == NULL) ? 0 : 
        g_list_length(self->priv->volumes);
    
    g_debug("FUNC(%s) Length self->priv->volumes: %i", __FUNCTION__,
        count_volumes);

    /* Get a string of the type int. */
    type_name = NULL;

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
            g_error("FUNC(%s) 'type' got a unkown value: %i", __FUNCTION__,
                type);

            return NULL;
    }

    g_debug("FUNC(%s) Value of 'type_name' after switch: %s", __FUNCTION__,
        type_name);

    /* Go through all of the volume elements and compare their value to the 
    value given as a function parameter. */
    volume = NULL;

    for(i = 0; i < count_volumes; i++) {

        /* Get the volume in the list. */
        volume = UMOUNTER_VOLUME(g_list_nth_data(self->priv->volumes, i));

        if(NULL == volume) {
            g_error("FUNC(%s) Unexpected error, 'volume' == NULL.", 
            __FUNCTION__);

            g_free(type_name);
            
            return NULL;
        }

        /* We get the value with the type_name. */
        g_object_get(G_OBJECT(volume), type_name, &type_value, NULL);

        /* If the value is the same as the one from this func above, the element
        exist and we return it. */
        if(0 == g_strcmp0(type_value, value)) {
            g_debug("FUNC(%s) Find existing element: %s", __FUNCTION__, 
                type_value);

            break;
        } else {
            volume = NULL;
        }
    }

    return volume;
}

gboolean
umounter_volumes_add(UMounterVolumes *self, UMounterVolume *volume) {      

    g_return_val_if_fail(NULL != self, FALSE);
    g_return_val_if_fail(NULL != volume, FALSE);

    UMounterVolume *tmp_volume;
    const gchar *name, *uuid;


    /* Get the name and uuid from the volume object and have a look if a
    volume already exist with these values. */
    g_object_get(G_OBJECT(volume), "name", &name, NULL);
    g_object_get(G_OBJECT(volume), "uuid", &uuid, NULL);

    tmp_volume = umounter_volumes_exist_volume_name_uuid(self, name, uuid);

    /* Lets have a look, wether we find a volume. If we found one, we can't add
    the given volume. */
    if(NULL != tmp_volume) {
        g_debug("FUNC(%s) Volume was found, no volume will be add.", 
            __FUNCTION__);        

        return FALSE;
    }

    self->priv->volumes = g_list_append(self->priv->volumes, volume);

    g_debug("FUNC(%s) Add the volume with the given name (%s) and uuid (%s).", 
        __FUNCTION__, name, uuid);

    return TRUE;
}

UMounterVolumes*
umounter_volumes_new(void) {
    UMounterVolumes *volumes = g_object_new(UMOUNTER_TYPE_VOLUMES, NULL);

    return volumes;
}

UMounterVolume*
umounter_volumes_exist_volume_name_uuid(UMounterVolumes *self, 
    const gchar *name, const gchar *uuid) {

    g_return_val_if_fail(NULL != self, NULL);

    UMounterVolume *tmp_volume;

    
    /* Initialisation of tmp_volume to NULL. */
    tmp_volume = NULL;    

    /* If we got a name, we can search for a volume with the name. */
    if(NULL != name)
        tmp_volume = umounter_volumes_exist_name(self, name);
    
    /* If no volume was found... */
    if(NULL == tmp_volume) {
        g_debug("FUNC(%s) No volume was found with the given name.", 
            __FUNCTION__);

        /* ...and a uuid exist, we search for a volume with the given uuid. */
        if(NULL != uuid)
            tmp_volume = umounter_volumes_exist_uuid(self, uuid);

        /* Just write a debug message if no volume was found. */
        if(NULL == tmp_volume)
            g_debug("FUNC(%s) No volume was found with the given uuid.", 
                __FUNCTION__);
    }

    /* Return the found volume, or NULL if no one was found. */
    return tmp_volume;    
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

