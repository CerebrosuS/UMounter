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

#ifndef UMOUNTER_VOLUME_H
#define UMOUNTER_VOLUME_H


#define UMOUNTER_TYPE_VOLUME (umounter_config_get_type())
#define UMOUNTER_VOLUME(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), \
        UMOUNTER_TYPE_VOLUME, UMounterVolume))
#define UMOUNTER_IS_VOLUME(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), \
        UMOUNTER_TYPE_VOLUME))
#define UMOUNTER_VOLUME_CLASS(cls) (G_TYPE_CHECK_CLASS_CAST((cls), \
        UMOUNTER_TYPE_VOLUME, UMounterVolumeClass))
#define UMOUNTER_IS_VOLUME_CLASS(cls) (G_TYPE_CHECK_CLASS_TYPE((cls), \
        UMOUNTER_TYPE_VOLUME))
#define UMOUNTER_VOLUME_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), \
        UMOUNTER_TYPE_VOLUME, UMounterVolumeClass))
#define UMOUNTER_VOLUME_GET_PRIVATE(obj) \
        (G_TYPE_INSTANCE_GET_PRIVATE((obj), UMOUNTER_TYPE_VOLUME, \
        UMounterVolumePrivate))

#include <glib.h>
#include <glib-object.h>


typedef struct _UMounterVolume UMounterVolume;
typedef struct _UMounterVolumeClass UMounterVolumeClass;
typedef struct _UMounterVolumePrivate UMounterVolumePrivate;

struct _UMounterVolume {
    GObject parent_instance;

    UMounterVolumePrivate *priv;
};

struct _UMounterVolumeClass {
    GObjectClass parent_class;
};


////////////////////////////////////////////////////////////////////////////////
/* Function declaration... all this function are accessible form other source
files and therefore something like public. :) */

GType 
umounter_volume_get_type(void);

/* Create a new UMounterVolume instance and returns it. */
UMounterVolume* 
umounter_volume_new(void);

////////////////////////////////////////////////////////////////////////////////


#endif /* UMOUNTER_VOLUME_H */

