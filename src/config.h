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

#ifndef UMOUNTER_CONFIG_H
#define UMOUNTER_CONFIG_H


#define UMOUNTER_TYPE_CONFIG (umounter_config_get_type())
#define UMOUNTER_CONFIG(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), \
        UMOUNTER_TYPE_CONFIG, UMounterConfig))
#define UMOUNTER_IS_CONFIG(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), \
        UMOUNTER_TYPE_CONFIG))
#define UMOUNTER_CONFIG_CLASS(cls) (G_TYPE_CHECK_CLASS_CAST((cls), \
        UMOUNTER_TYPE_CONFIG, UMounterConfigClass))
#define UMOUNTER_IS_CONFIG_CLASS(cls) (G_TYPE_CHECK_CLASS_TYPE((cls), \
        UMOUNTER_TYPE_CONFIG))
#define UMOUNTER_CONFIG_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), \
        UMOUNTER_TYPE_CONFIG, UMounterConfigClass))
#define UMOUNTER_CONFIG_GET_PRIVATE(obj) \
        (G_TYPE_INSTANCE_GET_PRIVATE((obj), UMOUNTER_TYPE_CONFIG, \
        UMounterConfigPrivate))

#include <glib.h>


typedef struct _UMounterConfig UMounterConfig;
typedef struct _UMounterConfigClass UMounterConfigClass;
typedef struct _UMounterConfigPrivate UMounterConfigPrivate;

struct _UMounterConfig {
    GObject parent_instance;

    UMounterConfigPrivate *priv;
};

struct _UMounterConfigClass {
    GObjectClass parent_class;
};


////////////////////////////////////////////////////////////////////////////////
/* Function declaration... all this function are accessible form other source
files and therefore something like public. :) */

GType 
umounter_config_get_type(void);

/* Create a new UMounterConfig instance and returns it. */
UMounterConfig* 
umounter_config_new(void);

////////////////////////////////////////////////////////////////////////////////


#endif /* UMOUNTER_CONFIG_H */

