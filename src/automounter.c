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

static gboolean
umounter_automounter_run_commands(UMounterAutomounter *self, UMounterVolume 
    *volume, gboolean on_mount, GVolume *g_volume, GError **error);

static gboolean
umounter_automounter_volume_mount(UMounterAutomounter *self, GVolume *volume,
UMounterVolume *u_volume, UMounterVolume *all_u_volume, GError **error);

static gchar*
umounter_automounter_solve_command(UMounterAutomounter *self, GVolume *volume, 
    const gchar *command);

////////////////////////////////////////////////////////////////////////////////

/* Macro for implementing the _get_type function and and defining a parent
pointer, which is accessible from the whole source file. */
G_DEFINE_TYPE(UMounterAutomounter, umounter_automounter, G_TYPE_OBJECT);

static void
umounter_automounter_dispose(GObject *gobject) {
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

    if(NULL == rules_path)
        rules_path = g_build_path("/", g_get_home_dir(), 
            ".umounter/rules.d", NULL);

    self->priv->volumes = umounter_rulesparser_parse(self->priv->rulesparser, 
        rules_path);

    if(NULL == self->priv->volumes) {
        g_debug("Creating empty volmes object cause parsing failed");
        self->priv->volumes = umounter_volumes_new();
    }    

    /* Build the signal connections. */
    GVolumeMonitor *volume_monitor = g_volume_monitor_get();
    g_signal_connect(volume_monitor, "volume-added", 
        G_CALLBACK(umounter_automounter_volume_added), self);
    g_signal_connect(volume_monitor, "volume-removed", 
        G_CALLBACK(umounter_automounter_volume_removed), self);
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

    self->priv->volumes = NULL;
    self->priv->config = NULL;
    self->priv->rulesparser = NULL;
}

static void
umounter_automounter_volume_mount_ready(GObject *source_object, 
    GAsyncResult *result, gpointer user_data) {

    g_return_if_fail(NULL != user_data);
    g_return_if_fail(TRUE == UMOUNTER_IS_AUTOMOUNTER(user_data));

    UMounterAutomounter *self;
    UMounterVolume *tmp_volume;
    UMounterVolume *all_volume;
    GError *error;

    gboolean mounted;
    gchar *device;


    /* Get the umounter automounter object from a gpointer. */
    self = UMOUNTER_AUTOMOUNTER(user_data);

    /* Init device information string. */
    device = g_volume_get_identifier(G_VOLUME(source_object), 
        G_VOLUME_IDENTIFIER_KIND_UNIX_DEVICE);

    /* Try to get a volume with the given name or uuid. */
    tmp_volume = umounter_volumes_exist_device(self->priv->volumes, device);
    
    /* Also try to get the all volume. */
    all_volume = umounter_volumes_exist_name(self->priv->volumes, "*");

    /* We should end all and print a message if we don't find a volume. */
    if(NULL == tmp_volume) {
        g_error("Can't find a volume, that was mounted before. There must a "
            "volume for that, otherwise something went wrong.");

        return;
    }

    /* Reset the error pointer. */
    error = NULL;

    /* Test if mounting was ok... */
    mounted = g_volume_mount_finish((GVolume*) source_object, result, 
        &error);

    if(FALSE == mounted) {
        g_error("Mounting the volume went wrong: %s", error->message);
        g_clear_error(&error);
    } else {
        g_message(" Ok!\n");

        g_object_set(G_OBJECT(tmp_volume), "is_mounted", TRUE, NULL);
    }

    umounter_automounter_run_commands(self, tmp_volume, TRUE, 
        G_VOLUME(source_object), &error);

    if(NULL != all_volume)
        umounter_automounter_run_commands(self, all_volume, TRUE, 
            G_VOLUME(source_object), &error);

    /* When recieving an error, informate about. */
    if(NULL != error) {
        g_error("Error while running one or more commands: %s", error->message);
        g_clear_error(&error);
    }        
}

static void 
umounter_automounter_volume_added(GVolumeMonitor *volume_monitor, 
    GVolume *volume, gpointer user_data) {

	g_debug("Volume added");

    g_return_if_fail(NULL != user_data);
    g_return_if_fail(UMOUNTER_IS_AUTOMOUNTER(user_data));

    UMounterAutomounter *self;
    UMounterVolume *tmp_volume;
    UMounterVolume *all_volume;
    GError *error;

    gchar *device, *name, *uuid;


    /* Get the umounter automounter object from a gpointer. */
    self = UMOUNTER_AUTOMOUNTER(user_data);

    /* Init device information string. */
    device = g_volume_get_identifier(volume, 
        G_VOLUME_IDENTIFIER_KIND_UNIX_DEVICE);
    name = g_volume_get_name(volume);
    uuid = g_volume_get_uuid(volume);

    /* Try to get a volume with the given name or uuid. */
    tmp_volume = umounter_volumes_exist_volume_name_uuid(self->priv->volumes, 
        name, uuid);

    /* Also try to get the all volume. */
    all_volume = umounter_volumes_exist_name(self->priv->volumes, "*");

    /* If can't find a volume with the given values, create a new one and add it
    to the volumes. */
    if(NULL == tmp_volume) {
        tmp_volume = umounter_volume_new();

        /* Set the given values to the volume. */
        g_object_set(G_OBJECT(tmp_volume), "name", name, "uuid", uuid,
            "device", device, NULL);

        /* Now add the volume to the volumes list. */
        umounter_volumes_add(self->priv->volumes, tmp_volume);
    }

    /* Set the given values to the volume. */
    g_object_set(G_OBJECT(tmp_volume), "name", name, "uuid", uuid,
        "device", device, NULL);

    g_message("SIGNAL: volume added!");
    g_message("Device: %s", device);
    g_message("Name: %s", name);
    g_message("UUID: %s", uuid);

    /* Running special commands if some are defined. */
    error = NULL;

    umounter_automounter_run_commands(self, tmp_volume, FALSE, volume, &error);

    if(NULL != all_volume)
        umounter_automounter_run_commands(self, all_volume, FALSE, volume, 
            &error);

    /* When recieving an error, informate about. */
    if(NULL != error) {
        g_error("Error while running one or more commands: %s", error->message);
        g_clear_error(&error);
    }

    /* Try to mount the volume. */
    umounter_automounter_volume_mount(self, volume, tmp_volume, all_volume, 
    	&error);

    /* Maybe the mounting failed with an error... */
    if(NULL != error) {
        g_error("Error mounting the volume: %s", error->message);
        g_clear_error(&error);
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

    g_return_if_fail(NULL != user_data);
    g_return_if_fail(UMOUNTER_IS_AUTOMOUNTER(user_data));

    UMounterAutomounter *self;

    gchar *device, *name, *uuid;


    /* Get the umounter automounter object from a gpointer. */
    self = UMOUNTER_AUTOMOUNTER(user_data);

    /* Init device information string. */
    device = g_volume_get_identifier(volume, 
        G_VOLUME_IDENTIFIER_KIND_UNIX_DEVICE);
    name = g_volume_get_name(volume);
    uuid = g_volume_get_uuid(volume);

    g_message("SIGNAL: volume removed!");
    g_message("Device: %s", g_volume_get_identifier(volume,
        G_VOLUME_IDENTIFIER_KIND_UNIX_DEVICE));
    g_message("Name: %s", g_volume_get_name(volume));
    g_message("UUID: %s", g_volume_get_uuid(volume));

    /* Cleaning... */
    if(NULL != device)
        g_free(device);

    if(NULL != name)
        g_free(name);

    if(NULL != uuid)
        g_free(uuid);
}

static gboolean
umounter_automounter_volume_mount(UMounterAutomounter *self, GVolume *volume,
UMounterVolume *u_volume, UMounterVolume *all_u_volume, GError **error) {

    g_return_val_if_fail(NULL != self, FALSE);
    g_return_val_if_fail(NULL != volume, FALSE);
    g_return_val_if_fail(NULL == *error, FALSE);

    GMountOperation *mount_operation;

    gboolean ignore_mount, all_ignore_mount, automount;

    
    /* Init local values... */
    g_object_get(G_OBJECT(self->priv->config), "automount", &automount, NULL);
    g_object_get(G_OBJECT(u_volume), "ignore_mount", &ignore_mount, NULL);

    /* When setting the all ignore value, the all_u_volume could be NULL. */
    if(NULL != all_u_volume)
        g_object_get(G_OBJECT(all_u_volume), "ignore_mount", &all_ignore_mount,
            NULL);
    else
        all_ignore_mount = FALSE;

    /* Now lets see if we schould mount... */
    if(FALSE == ignore_mount && FALSE == all_ignore_mount && 
        TRUE == automount && TRUE == g_volume_can_mount(volume)) {

        g_message("Mount: ...");

        /* Create a mounting operation and mount it... */
        mount_operation = g_mount_operation_new();
        g_mount_operation_set_anonymous(mount_operation, TRUE);

        g_volume_mount(volume, G_MOUNT_MOUNT_NONE, NULL, NULL, 
            umounter_automounter_volume_mount_ready, self);
    }

    return TRUE;
}

static gboolean
umounter_automounter_run_commands(UMounterAutomounter *self, 
    UMounterVolume *volume, gboolean on_mount, GVolume *g_volume, 
    GError **error) {

    g_return_val_if_fail(NULL != volume, FALSE);
    g_return_val_if_fail(NULL == *error, FALSE);

    GList *commands;

    gint commands_count, i;
    const gchar* command;
    const gchar* command_list_name;


    /* Init local variables... */
    if(TRUE == on_mount)
        command_list_name = "command_on_mount_list";
    else
        command_list_name = "command_list";

    g_object_get(G_OBJECT(volume), command_list_name, &commands, NULL);

    /* If we get NULL back, there are no commands, but this is no fail, so
    we will return FALSE but won't init error. */
    if(NULL == commands)
        return FALSE;

    /* Get the count of the commands... */
    commands_count = g_list_length(commands);

    g_debug("FUNC(%s) We found %i commands for %s", __FUNCTION__, 
        commands_count, command_list_name);

    /* ...and go through all of them with a loop. */
    for(i = 0; i < commands_count; i++) {

        /* Get the command from the list... */
        command = g_list_nth_data(commands, i);

        g_debug("FUNC(%s) Try to solve the command: %s", __FUNCTION__, command);

        /* If the command is on mount, solve for variables. */
        command = umounter_automounter_solve_command(self, g_volume, command);
        
        g_debug("FUNC(%s) Command solved to: %s", __FUNCTION__, command);
    
        /* If the command is NULL, nothing will be executed. */
        if(NULL != command) {
            g_message("Running command: %s", command);
            g_spawn_command_line_async(command, error);
        }
    }

    return TRUE;
}

static gchar*
umounter_automounter_solve_command(UMounterAutomounter *self, GVolume *volume, 
    const gchar *command) {
    
    g_return_val_if_fail(NULL != self, NULL);
    g_return_val_if_fail(NULL != command, NULL);
    g_return_val_if_fail(NULL != volume, NULL);

    GFile *root;
    GMount *mount;

    gchar *new_command, *device, *mount_path;
    gint command_length, new_command_length;

    
    /* Init local values. */
    mount = g_volume_get_mount(volume);
    root = g_mount_get_root(mount);
    mount_path = g_file_get_path(root);

    new_command = NULL;

    if(NULL != mount_path) {
        new_command = umounter_string_replace(command, "%d", mount_path);

        g_debug("FUNC(%s) New command after solve: %s", __FUNCTION__, 
            new_command);

        /* Cleaning... */
        g_free(mount_path);
    }

    g_object_unref(mount);
    g_object_unref(root);

    return new_command;
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

