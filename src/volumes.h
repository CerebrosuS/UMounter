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

#ifndef UMOUNTER_VOLUMES_H
#define UMOUNTER_VOLUMES_H


#define UMOUNTER_TYPE_VOLUMES (umounter_volumes_get_type())
#define UMOUNTER_VOLUMES(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), \
        UMOUNTER_TYPE_VOLUMES, UMounterVolumes))
#define UMOUNTER_IS_VOLUMES(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), \
        UMOUNTER_TYPE_VOLUMES))
#define UMOUNTER_VOLUMES_CLASS(cls) (G_TYPE_CHECK_CLASS_CAST((cls), \
        UMOUNTER_TYPE_VOLUMES, UMounterVolumesClass))
#define UMOUNTER_IS_VOLUMES_CLASS(cls) (G_TYPE_CHECK_CLASS_TYPE((cls), \
        UMOUNTER_TYPE_VOLUMES))
#define UMOUNTER_VOLUMES_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), \
        UMOUNTER_TYPE_VOLUMES, UMounterVolumesClass))
#define UMOUNTER_VOLUMES_GET_PRIVATE(obj) \
        (G_TYPE_INSTANCE_GET_PRIVATE((obj), UMOUNTER_TYPE_VOLUMES, \
        UMounterVolumesPrivate))

#include <glib.h>
#include <glib-object.h>

#include "volume.h"


typedef struct _UMounterVolumes UMounterVolumes;
typedef struct _UMounterVolumesClass UMounterVolumesClass;
typedef struct _UMounterVolumesPrivate UMounterVolumesPrivate;

struct _UMounterVolumes {
    GObject parent_instance;

    UMounterVolumesPrivate *priv;
};

struct _UMounterVolumesClass {
    GObjectClass parent_class;
};

enum {
    VOLUME_START = 0,

    VOLUME_NAME,
    VOLUME_UUID,
    VOLUME_DEVICE,
    
    VOLUME_END
};

////////////////////////////////////////////////////////////////////////////////
/* Function declaration... all this function are accessible form other source
files and therefore something like public. :) */

GType 
umounter_volumes_get_type(void);

/* Create a new UMounterVolumes instance and returns it. */
UMounterVolumes* 
umounter_volumes_new(void);

/* Several exist functions. If exist fails, NULL will be returned. */
UMounterVolume*
umounter_volumes_exist_volume_name_uuid(UMounterVolumes *self, 
    const gchar *name, const gchar *uuid);

UMounterVolume*
umounter_volumes_exist_name(UMounterVolumes *self, const gchar *name);

UMounterVolume*
umounter_volumes_exist_uuid(UMounterVolumes *self, const gchar *uuid);

UMounterVolume*
umounter_volumes_exist_device(UMounterVolumes *self, const gchar *device);

/* Adds a volume to the volumes. */
gboolean
umounter_volumes_add(UMounterVolumes *self, UMounterVolume *volume);

/* Removes a volume from the volumes and returns the element. */
UMounterVolume*
umounter_volumes_remove(UMounterVolumes *self, UMounterVolume *volume);

UMounterVolume*
umounter_volumes_remove_name(UMounterVolumes *self, const gchar *name);

UMounterVolume*
umounter_volumes_remove_uuid(UMounterVolumes *self, const gchar *uuid);

UMounterVolume*
umounter_volumes_remove_device(UMounterVolumes *self, const gchar *device);

////////////////////////////////////////////////////////////////////////////////


#endif /* UMOUNTER_VOLUMES_H */

