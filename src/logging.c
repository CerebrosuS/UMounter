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


////////////////////////////////////////////////////////////////////////////////

enum {
    PROP_0,

    PROP_DEBUG,
    PROP_INFO
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
logging_handler(const gchar *log_domain, GLogLevelFlags log_level,
    const gchar *message, gpointer user_data);

////////////////////////////////////////////////////////////////////////////////

/* Macro for implementing the _get_type function and and defining a parent
pointer, which is accessible from the whole source file. */
G_DEFINE_TYPE(UMounterLogging, umounter_logging, G_TYPE_OBJECT);

static void
umounter_logging_dispose(GObject *gobject) {
    g_message(__FUNCTION__);

    UMounterLogging *self = UMOUNTER_LOGGING(gobject);
     
    /* In dispose, you are supposed to free all types referenced from this
    object which might themselves hold a reference to self. Generally,
    the most simple solution is to unref all members on which you own a 
    reference. */

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
    g_log_set_handler (NULL, G_LOG_LEVEL_WARNING | G_LOG_LEVEL_ERROR | 
        G_LOG_LEVEL_DEBUG | G_LOG_FLAG_FATAL | G_LOG_FLAG_RECURSION, 
        logging_handler, self);
}

static void
umounter_logging_set_property(GObject *gobject, guint property_id, 
    const GValue *value, GParamSpec *pspec) {

    UMounterLogging *self = UMOUNTER_LOGGING(gobject);

    switch(property_id) {
        case PROP_DEBUG:
            self->priv->debug = g_value_get_boolean(value);
            break;
        case PROP_INFO:
            self->priv->info = g_value_get_boolean(value);
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
        case PROP_INFO:
            g_value_set_boolean(value, self->priv->info);
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

    pspec = g_param_spec_boolean("info",
        "If true info messages will be printed.", "Set info value.", FALSE, 
        G_PARAM_READWRITE | G_PARAM_CONSTRUCT);
    g_object_class_install_property(gobject_class, PROP_INFO, pspec);

    /* Add private class... */

    g_type_class_add_private(cls, sizeof(UMounterLoggingPrivate));
}

static void
umounter_logging_init(UMounterLogging *self) {
    self->priv = UMOUNTER_LOGGING_GET_PRIVATE(self);
}

static void 
logging_handler(const gchar *log_domain, GLogLevelFlags log_level,
    const gchar *message, gpointer user_data) {

    g_return_if_fail(UMOUNTER_IS_LOGGING(user_data));

    UMounterLogging *self = UMOUNTER_LOGGING(user_data);


    /* Go through all the log levels. */
    switch(log_level) {
        case G_LOG_LEVEL_DEBUG:
            if(TRUE == self->priv->debug)
                g_print("-- DEBUG -- %s\n", message);
            break;
        case G_LOG_LEVEL_ERROR:
            g_print("-- ERROR -- %s\n", message);
            break;
        default:
            g_print("-- UNKNOWN LOG LEVEL -- %s\n", message);
            break;
    }        
}

UMounterLogging*
umounter_logging_new(void) {
    UMounterLogging *logging = g_object_new(UMOUNTER_TYPE_LOGGING, NULL);

    return logging;
}

