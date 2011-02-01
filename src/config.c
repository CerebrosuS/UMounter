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

    /* Chain up to the parent class. */
    G_OBJECT_CLASS(umounter_config_parent_class)->finalize(gobject);
}

static void
umounter_config_set_property(GObject *gobject, guint property_id, 
    const GValue *value, GParamSpec *pspec) {

    UMounterConfig *self = UMOUNTER_CONFIG(gobject);

    switch(property_id) {
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

    /* Add private class... */

    g_type_class_add_private(cls, sizeof(UMounterConfigPrivate));
}

static void
umounter_config_init(UMounterConfig *self) {
    self->priv = UMOUNTER_CONFIG_GET_PRIVATE(self);
}

