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

#include "automounter.h"
#include "automounter-private.h"


////////////////////////////////////////////////////////////////////////////////

enum {
    PROP_0,
    
    PROP_CONFIG,
    PROP_RULESPARSER
};

////////////////////////////////////////////////////////////////////////////////
/* Function declaration... All functions being defined here are static and
therefore PRIVATE (can not be called outside this source file. */

static void 
umounter_automounter_class_init(UMounterAutomounterClass *cls);

static void
umounter_automounter_init(UMounterAutomounter *self);

static void 
umounter_automounter_dispose(GObject *gobject);

static void 
umounter_automounter_finalize(GObject *gobject);

static void 
umounter_automounter_set_property(GObject *gobject, guint property_id, 
    const GValue *value, GParamSpec *pspec);

static void 
umounter_automounter_get_property(GObject *gobject, guint property_id, 
    GValue *value, GParamSpec *pspec);

static void
umounter_automounter_contructed(GObject *gobject);

static void 
umounter_automounter_volume_added(GVolumeMonitor *volume_monitor, 
    GVolume *volume, gpointer user_data);

static void 
umounter_automounter_volume_removed(GVolumeMonitor *volume_monitor, 
    GVolume *volume, gpointer user_data);

static void
umounter_automounter_volume_mount_ready(GObject *source_object, 
    GAsyncResult *result, gpointer user_data);

static gpointer
umounter_automounter_thread_func(gpointer data);

////////////////////////////////////////////////////////////////////////////////

/* Macro for implementing the _get_type function and and defining a parent
pointer, which is accessible from the whole source file. */
G_DEFINE_TYPE(UMounterAutomounter, umounter_automounter, G_TYPE_OBJECT);

static void
umounter_automounter_dispose(GObject *gobject) {
    g_message(__FUNCTION__);

    UMounterAutomounter *self = UMOUNTER_AUTOMOUNTER(gobject);

     
    /* In dispose, you are supposed to free all types referenced from this
    object which might themselves hold a reference to self. Generally,
    the most simple solution is to unref all members on which you own a 
    reference. */

    /* Chain up to the parent class. */
    G_OBJECT_CLASS(umounter_automounter_parent_class)->dispose(gobject);
}

static void
umounter_automounter_finalize(GObject *gobject) {
    UMounterAutomounter *self = UMOUNTER_AUTOMOUNTER(gobject);

    /* Chain up to the parent class. */
    G_OBJECT_CLASS(umounter_automounter_parent_class)->finalize(gobject);
}

static void
umounter_automounter_set_property(GObject *gobject, guint property_id, 
    const GValue *value, GParamSpec *pspec) {

    UMounterAutomounter *self = UMOUNTER_AUTOMOUNTER(gobject);

    switch(property_id) {
        case PROP_CONFIG:
            if(NULL != self->priv->config)
                g_object_unref(self->priv->config);

            self->priv->config = g_value_get_object(value);
            break;
        case PROP_RULESPARSER:
            if(NULL != self->priv->rulesparser)
                g_object_unref(self->priv->rulesparser);

            self->priv->rulesparser = g_value_get_object(value);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, property_id, pspec);
            break;
    }
}

static void
umounter_automounter_get_property(GObject *gobject, guint property_id, 
    GValue *value, GParamSpec *pspec) {

    UMounterAutomounter *self = UMOUNTER_AUTOMOUNTER(gobject);

    switch(property_id) {
        case PROP_CONFIG:
            g_value_set_object(value, self->priv->config);
            break;
        case PROP_RULESPARSER:
            g_value_set_object(value, self->priv->rulesparser);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, property_id, pspec);
            break;    
    }
}

static void
umounter_automounter_constructed(GObject *gobject) {
    /* Chain up to the parent constrcuted function. */
    if(NULL != G_OBJECT_CLASS(umounter_automounter_parent_class)->constructed)
        G_OBJECT_CLASS(umounter_automounter_parent_class)->constructed(gobject);

    UMounterAutomounter *self = UMOUNTER_AUTOMOUNTER(gobject);

    /* Parse all rules and set them to the UMounterVolumes. */
    const gchar *rules_path;
    g_object_get(G_OBJECT(self->priv->config), "rules_path", &rules_path, NULL);
    self->priv->volumes = umounter_rulesparser_parse(self->priv->rulesparser, 
        rules_path);

    if(NULL == self->priv->volumes) {
        g_debug("Creating empty volmes object cause parsing failed");
        self->priv->volumes = umounter_volumes_new();
    }    

    /* Build the signal connections. */
    GVolumeMonitor *volume_monitor = g_volume_monitor_get();
    g_signal_connect(volume_monitor, "volume-added", 
        G_CALLBACK(umounter_automounter_volume_added), self->priv->volumes);
    g_signal_connect(volume_monitor, "volume-removed", 
        G_CALLBACK(umounter_automounter_volume_removed), self->priv->volumes);
}

static void
umounter_automounter_class_init(UMounterAutomounterClass *cls) {
    GObjectClass *gobject_class = G_OBJECT_CLASS(cls);
    GParamSpec *pspec;

    /* Override different methods. */

    gobject_class->dispose = umounter_automounter_dispose;
    gobject_class->finalize = umounter_automounter_finalize;
    gobject_class->constructed = umounter_automounter_constructed;
    gobject_class->set_property = umounter_automounter_set_property;
    gobject_class->get_property = umounter_automounter_get_property;

    /* Set different properties. */

    pspec = g_param_spec_object("config",
        "The object of config.", "Set config object.", UMOUNTER_TYPE_CONFIG, 
        G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_CONSTRUCT);
    g_object_class_install_property(gobject_class, PROP_CONFIG, pspec);

    pspec = g_param_spec_object("rulesparser", "The object of rulesparser.", 
        "Set rulesparser object.", UMOUNTER_TYPE_RULESPARSER, 
        G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_CONSTRUCT);
    g_object_class_install_property(gobject_class, PROP_RULESPARSER, pspec);

    /* Add private class... */

    g_type_class_add_private(cls, sizeof(UMounterAutomounterPrivate));
}

static void
umounter_automounter_init(UMounterAutomounter *self) {
    self->priv = UMOUNTER_AUTOMOUNTER_GET_PRIVATE(self);
    
    self->priv->main_loop = g_main_loop_new(NULL, FALSE);
    self->priv->volumes = NULL;
    self->priv->config = NULL;
    self->priv->rulesparser = NULL;
}

static void
umounter_automounter_volume_mount_ready(GObject *source_object, 
    GAsyncResult *result, gpointer user_data) {

    GError *error;
    error = NULL;
    
    gboolean mount_ok = g_volume_mount_finish((GVolume*) source_object, result, 
        &error);

    if(FALSE == mount_ok) {
        g_print(error->message);
        g_error_free(error);
    } else {
        g_print(" Ok!");
    }
}

static void 
umounter_automounter_volume_added(GVolumeMonitor *volume_monitor, 
    GVolume *volume, gpointer user_data) {

    g_return_if_fail(NULL != user_data);
    g_return_if_fail(UMOUNTER_IS_VOLUMES(user_data));

    GMountOperation *mount_operation;
    UMounterVolumes *volumes;
    UMounterVolume *tmp_volume;
    gchar *device, *name, *uuid;
    gboolean ignore_mount;
    

    device = g_volume_get_identifier(volume, 
        G_VOLUME_IDENTIFIER_KIND_UNIX_DEVICE);
    name = g_volume_get_name(volume);
    uuid = g_volume_get_uuid(volume);

    g_debug("FUNC(%s) Volume name: %s", __FUNCTION__, name);
    g_debug("FUNC(%s) Volume uuid: %s", __FUNCTION__, uuid);
    g_debug("FUNC(%s) Volume device: %s", __FUNCTION__, device);

    volumes = UMOUNTER_VOLUMES(user_data);
    tmp_volume = umounter_volumes_exist_volume_name_uuid(volumes, name, uuid);

    /* If can't find a volume with the given values, create a new one. */
    if(NULL == tmp_volume) 
        tmp_volume = umounter_volume_new();

    /* Set the given values to the volume. */
    g_object_set(G_OBJECT(tmp_volume), "name", name, "uuid", uuid,
        "device", device, NULL);

    g_print("\nSignal: volume added!\n");
    g_print("-- Device: %s\n", device);
    g_print("-- Name: %s\n", name);
    g_print("-- UUID: %s\n", uuid);

    gboolean can_mount = g_volume_can_mount(volume);   

    g_object_get(G_OBJECT(tmp_volume), "ignore_mount", &ignore_mount, NULL);
    if(FALSE == ignore_mount) {
        if(can_mount) {
            g_print("-- Mount: ...");

            /* Create a mounting operation and mount it... */
            mount_operation = g_mount_operation_new();
            g_mount_operation_set_anonymous(mount_operation, TRUE);

            g_volume_mount(volume, G_MOUNT_MOUNT_NONE, NULL, NULL, 
                umounter_automounter_volume_mount_ready, NULL);
        }
    } else {
        g_print("-- Mount: ... Ignore!");
    }

    /* Cleaning... */
    if(NULL != device)
        g_free(device);

    if(NULL != name)
        g_free(name);

    if(NULL != uuid)
        g_free(uuid);
}

static void 
umounter_automounter_volume_removed(GVolumeMonitor *volume_monitor, 
    GVolume *volume, gpointer user_data) {

    gchar *device, *name, *uuid;

    device = g_volume_get_identifier(volume, 
        G_VOLUME_IDENTIFIER_KIND_UNIX_DEVICE);
    name = g_volume_get_name(volume);
    uuid = g_volume_get_uuid(volume);

    g_print("\nSignal: volume removed!\n");
    g_print("-- Device: %s\n", g_volume_get_identifier(volume,
        G_VOLUME_IDENTIFIER_KIND_UNIX_DEVICE));
    g_print("-- Name: %s\n", g_volume_get_name(volume));
    g_print("-- UUID: %s\n", g_volume_get_uuid(volume));

    /* Cleaning... */
    g_free(device);
    g_free(name);
    g_free(uuid);
}

UMounterAutomounter*
umounter_automounter_new(UMounterConfig *config, 
    UMounterRulesParser *rulesparser) {

    g_return_val_if_fail(config != NULL, NULL);
    g_return_val_if_fail(rulesparser != NULL, NULL);

    UMounterAutomounter *automounter = g_object_new(UMOUNTER_TYPE_AUTOMOUNTER, 
        "config", config, "rulesparser", rulesparser, NULL);

    return automounter;
}

gboolean
umounter_automounter_run(UMounterAutomounter *self) {
    g_return_val_if_fail(self != NULL, FALSE);

    g_main_loop_run(self->priv->main_loop);

    return TRUE;
}

gboolean
umounter_automounter_run_as_thread(UMounterAutomounter *self) {
    g_return_val_if_fail(self != NULL, FALSE);

    g_thread_init(NULL);
    GThread *thread = g_thread_create(umounter_automounter_thread_func, self, 
        TRUE, NULL);

    return TRUE;
}

gpointer
umounter_automounter_thread_func(gpointer data) {
    g_return_val_if_fail(UMOUNTER_IS_AUTOMOUNTER(data), NULL);

    g_main_loop_run(((UMounterAutomounter*)data)->priv->main_loop);
}

