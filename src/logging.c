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

#include "logging.h"
#include "logging-private.h"

#include <string.h>


////////////////////////////////////////////////////////////////////////////////

enum {
    PROP_0,

    PROP_DEBUG,
    PROP_MESSAGE,
    PROP_CRITICAL,
    PROP_ERROR,
    PROP_LOG_TO_FILE,
    PROP_LOG_FILE_PATH
};

////////////////////////////////////////////////////////////////////////////////
/* Function declaration... All functions being defined here are static and
therefore PRIVATE (can not be called outside this source file. */

static void 
umounter_logging_class_init(UMounterLoggingClass *cls);

static void
umounter_logging_init(UMounterLogging *self);

static void 
umounter_logging_dispose(GObject *gobject);

static void 
umounter_logging_finalize(GObject *gobject);

static void
umounter_logging_constructed(GObject *gobject);

static void 
umounter_logging_set_property(GObject *object, guint property_id, 
    const GValue *value, GParamSpec *pspec);

static void 
umounter_logging_get_property(GObject *gobject, guint property_id, 
    GValue *value, GParamSpec *pspec);

static void 
umounter_logging_handler(const gchar *log_domain, GLogLevelFlags log_level,
    const gchar *message, gpointer user_data);

static void
umounter_logging_log_to_file(UMounterLogging *self, const gchar* message);

static void
umounter_logging_log_file_path_changed(UMounterLogging *self);

////////////////////////////////////////////////////////////////////////////////

/* Macro for implementing the _get_type function and and defining a parent
pointer, which is accessible from the whole source file. */
G_DEFINE_TYPE(UMounterLogging, umounter_logging, G_TYPE_OBJECT);

static void
umounter_logging_dispose(GObject *gobject) {
    UMounterLogging *self = UMOUNTER_LOGGING(gobject);
     
    /* In dispose, you are supposed to free all types referenced from this
    object which might themselves hold a reference to self. Generally,
    the most simple solution is to unref all members on which you own a 
    reference. */

    /* Delete the output stream object of the logging file, if it exist. */
    if(NULL != self->priv->out_stream) {
        g_output_stream_close(self->priv->out_stream, NULL, NULL);
        g_object_unref(self->priv->out_stream);
    }

    /* Chain up to the parent class. */
    G_OBJECT_CLASS(umounter_logging_parent_class)->dispose(gobject);
}

static void
umounter_logging_finalize(GObject *gobject) {
    UMounterLogging *self = UMOUNTER_LOGGING(gobject);

    /* Chain up to the parent class. */
    G_OBJECT_CLASS(umounter_logging_parent_class)->finalize(gobject);
}

static void
umounter_logging_constructed(GObject *gobject) {
    /* Chain up to the parent constrcuted function. */
    if(NULL != G_OBJECT_CLASS(umounter_logging_parent_class)->constructed)
        G_OBJECT_CLASS(umounter_logging_parent_class)->constructed(gobject);

    UMounterLogging *self = UMOUNTER_LOGGING(gobject);

    /* Set the default logging handler. */
    g_log_set_handler(NULL, G_LOG_LEVEL_WARNING | G_LOG_LEVEL_ERROR | 
        G_LOG_LEVEL_DEBUG | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_MESSAGE | 
        G_LOG_LEVEL_MESSAGE | G_LOG_FLAG_FATAL | G_LOG_FLAG_RECURSION, 
        umounter_logging_handler, self);
    g_log_set_handler("GLib", G_LOG_LEVEL_WARNING | G_LOG_LEVEL_ERROR | 
        G_LOG_LEVEL_DEBUG | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_MESSAGE | 
        G_LOG_LEVEL_MESSAGE | G_LOG_FLAG_FATAL | G_LOG_FLAG_RECURSION, 
        umounter_logging_handler, self);
}

static void
umounter_logging_set_property(GObject *gobject, guint property_id, 
    const GValue *value, GParamSpec *pspec) {

    UMounterLogging *self = UMOUNTER_LOGGING(gobject);

    switch(property_id) {
        case PROP_DEBUG:
            self->priv->debug = g_value_get_boolean(value);
            break;
        case PROP_MESSAGE:
            self->priv->message = g_value_get_boolean(value);
            break;
        case PROP_CRITICAL:
            self->priv->critical = g_value_get_boolean(value);
            break;
        case PROP_ERROR:
            self->priv->error = g_value_get_boolean(value);
            break;
        case PROP_LOG_TO_FILE:
            self->priv->log_to_file = g_value_get_boolean(value);
            break;
        case PROP_LOG_FILE_PATH:
            if(NULL != self->priv->log_file_path)
                g_free(self->priv->log_file_path);

            self->priv->log_file_path = g_value_dup_string(value);
            
            /* MESSAGErmate, that the log_file_name has changed. */
            umounter_logging_log_file_path_changed(self);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, property_id, pspec);
            break;
    }
}

static void
umounter_logging_get_property(GObject *gobject, guint property_id, 
    GValue *value, GParamSpec *pspec) {

    UMounterLogging *self = UMOUNTER_LOGGING(gobject);

    switch(property_id) {
        case PROP_DEBUG:
            g_value_set_boolean(value, self->priv->debug);
            break;
        case PROP_MESSAGE:
            g_value_set_boolean(value, self->priv->message);
            break;
        case PROP_CRITICAL:
            g_value_set_boolean(value, self->priv->critical);
            break;
        case PROP_ERROR:
            g_value_set_boolean(value, self->priv->error);
            break;
        case PROP_LOG_TO_FILE:
            g_value_set_boolean(value, self->priv->log_to_file);
            break;
        case PROP_LOG_FILE_PATH:
            g_value_set_string(value, self->priv->log_file_path);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, property_id, pspec);
            break;
    }
}

static void
umounter_logging_class_init(UMounterLoggingClass *cls) {
    GObjectClass *gobject_class = G_OBJECT_CLASS(cls);
    GParamSpec *pspec;

    /* Override different methods. */

    gobject_class->dispose = umounter_logging_dispose;
    gobject_class->finalize = umounter_logging_finalize;
    gobject_class->constructed = umounter_logging_constructed;
    gobject_class->set_property = umounter_logging_set_property;
    gobject_class->get_property = umounter_logging_get_property;

    /* Set different properties. */

    pspec = g_param_spec_boolean("debug",
        "If true debug messages will be printed.", "Set debug value.", FALSE, 
        G_PARAM_READWRITE | G_PARAM_CONSTRUCT);
    g_object_class_install_property(gobject_class, PROP_DEBUG, pspec);

    pspec = g_param_spec_boolean("message",
        "If true MESSAGE messages will be printed.", "Set MESSAGE value.", FALSE, 
        G_PARAM_READWRITE | G_PARAM_CONSTRUCT);
    g_object_class_install_property(gobject_class, PROP_MESSAGE, pspec);

    pspec = g_param_spec_boolean("critical",
        "If true critical messages will be printed.", "Set critical value.", 
        FALSE, G_PARAM_READWRITE | G_PARAM_CONSTRUCT);
    g_object_class_install_property(gobject_class, PROP_CRITICAL, pspec);

    pspec = g_param_spec_boolean("error",
        "If true error messages will be printed.", "Set error value.", FALSE, 
        G_PARAM_READWRITE | G_PARAM_CONSTRUCT);
    g_object_class_install_property(gobject_class, PROP_ERROR, pspec);

    pspec = g_param_spec_boolean("log_to_file",
        "If true all logging messages will be written to a file.", 
        "Set the log_to_file value.", FALSE,
        G_PARAM_READWRITE | G_PARAM_CONSTRUCT);
    g_object_class_install_property(gobject_class, PROP_LOG_TO_FILE, pspec);

    const gchar* log_file_path = g_build_path("/", g_get_home_dir(), 
        ".umounter/umounter.log");
    pspec = g_param_spec_string("log_file_path",
        "Path of the logging file.", "Set the path of the logging file.",
        log_file_path, G_PARAM_READWRITE | G_PARAM_CONSTRUCT);
    g_object_class_install_property(gobject_class, PROP_LOG_FILE_PATH, pspec);

    /* Add private class... */

    g_type_class_add_private(cls, sizeof(UMounterLoggingPrivate));
}

static void
umounter_logging_init(UMounterLogging *self) {
    self->priv = UMOUNTER_LOGGING_GET_PRIVATE(self);
    self->priv->log_file = NULL;
    self->priv->out_stream = NULL;
}

static void 
umounter_logging_handler(const gchar *log_domain, GLogLevelFlags log_level,
    const gchar *message, gpointer user_data) {

    g_return_if_fail(UMOUNTER_IS_LOGGING(user_data));

    UMounterLogging *self = UMOUNTER_LOGGING(user_data);
    
    gboolean write_message;
    gchar *full_message;
    gchar *prefix;


    full_message = NULL;
    write_message = FALSE;

    /* Go through all the log levels. */
    switch(log_level) {
        case G_LOG_LEVEL_DEBUG:
            prefix = "-- DEBUG -- ";
            write_message = self->priv->debug;
            break;
        case G_LOG_LEVEL_ERROR:
            prefix = "-- ERROR -- ";
            write_message = self->priv->error;
            break;
        case G_LOG_LEVEL_CRITICAL:
            prefix = "-- CRITICAL -- ";
            write_message = self->priv->critical;
            break;
        case G_LOG_LEVEL_MESSAGE:
            prefix = "-- MESSAGE -- ";
            write_message = self->priv->message;
            break;
        default:
            g_print("-- UNKNOWN LOG LEVEL -- %s\n", message, NULL);
            break;
    }

    if(TRUE == write_message) {
        full_message = g_strconcat(prefix, message, "\n", NULL);
        
        /* Now go on and log the message to a file, if this is enabled in the config
        and a directory is given. */
        if(TRUE == self->priv->log_to_file)
            umounter_logging_log_to_file(self, full_message);
        else
            g_print("%s", full_message);

        g_free(full_message);
    }
}

static void
umounter_logging_log_file_path_changed(UMounterLogging *self) {
    
    g_return_if_fail(NULL != self);
    g_return_if_fail(NULL != self->priv->log_file_path);

    GError *error;
    GFile *log_file;
    GOutputStream *out_stream;

    
    /* Init several local values from the priv object. */
    log_file = self->priv->log_file;
    out_stream = self->priv->out_stream;

    error = NULL;

    /* Clean memory, if some objects already exist. */

    if(NULL != log_file)
        g_object_unref(log_file);

    if(NULL != out_stream)
        g_object_unref(out_stream);

    /* Create the file object. */
    log_file = g_file_new_for_path(self->priv->log_file_path);

    /* Create the output stream. */
    out_stream = G_OUTPUT_STREAM(g_file_replace(log_file, NULL, FALSE, 
        G_FILE_CREATE_PRIVATE, NULL, &error));
    
    /* If getting the output stream failed... */
    if(NULL != error) {
        g_print("-- ERROR -- FUNC(%s) Cannot open a stream for log file: %s",
            __FUNCTION__, self->priv->log_file_path);

        g_error_free(error);

        return;
    }

    self->priv->log_file = log_file;
    self->priv->out_stream = out_stream;
}

static void
umounter_logging_log_to_file(UMounterLogging *self, const gchar* message) {    

    g_return_if_fail(NULL != self->priv->out_stream);
    g_return_if_fail(NULL != message);

    GError *error;
    GOutputStream *out_stream;
    
    gsize bytes_written;


    /* Init local values from pivate. */
    out_stream = self->priv->out_stream;

    /* Init error to NULL. */
    error = NULL;

    /* Write the message data to the stream. */
    
    g_output_stream_write_all(out_stream, message, strlen(message), 
        &bytes_written, NULL, &error);
    g_output_stream_flush(out_stream, NULL, &error);

    /* If writing to the output stream failed... */
    if(NULL != error) {
        g_print("-- ERROR -- FUNC(%s) Cannot write message to log file: %s",
            __FUNCTION__, self->priv->log_file_path);

        g_error_free(error);
    }
}

UMounterLogging*
umounter_logging_new(void) {
    UMounterLogging *logging = g_object_new(UMOUNTER_TYPE_LOGGING, NULL);

    return logging;
}

