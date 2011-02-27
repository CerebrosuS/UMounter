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

#ifndef UMOUNTER_LOGGING_H
#define UMOUNTER_LOGGING_H


#define UMOUNTER_TYPE_LOGGING (umounter_logging_get_type())
#define UMOUNTER_LOGGING(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), \
        UMOUNTER_TYPE_LOGGING, UMounterLogging))
#define UMOUNTER_IS_LOGGING(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), \
        UMOUNTER_TYPE_LOGGING))
#define UMOUNTER_LOGGING_CLASS(cls) (G_TYPE_CHECK_CLASS_CAST((cls), \
        UMOUNTER_TYPE_LOGGING, UMounterLoggingClass))
#define UMOUNTER_IS_LOGGING_CLASS(cls) (G_TYPE_CHECK_CLASS_TYPE((cls), \
        UMOUNTER_TYPE_LOGGING))
#define UMOUNTER_LOGGING_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), \
        UMOUNTER_TYPE_LOGGING, UMounterLoggingClass))
#define UMOUNTER_LOGGING_GET_PRIVATE(obj) \
        (G_TYPE_INSTANCE_GET_PRIVATE((obj), UMOUNTER_TYPE_LOGGING, \
        UMounterLoggingPrivate))

#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>


typedef struct _UMounterLogging UMounterLogging;
typedef struct _UMounterLoggingClass UMounterLoggingClass;
typedef struct _UMounterLoggingPrivate UMounterLoggingPrivate;

struct _UMounterLogging {
    GObject parent_instance;

    UMounterLoggingPrivate *priv;
};

struct _UMounterLoggingClass {
    GObjectClass parent_class;
};


////////////////////////////////////////////////////////////////////////////////
/* Function declaration... all this function are accessible form other source
files and therefore something like public. :) */

GType 
umounter_logging_get_type(void);

/* Create a new UMounterLogging instance and returns it. */
UMounterLogging* 
umounter_logging_new(void);

////////////////////////////////////////////////////////////////////////////////


#endif /* UMOUNTER_LOGGING_H */

