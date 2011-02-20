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

#ifndef UMOUNTER_AUTOMOUNTER_H
#define UMOUNTER_AUTOMOUNTER_H


#define UMOUNTER_TYPE_AUTOMOUNTER (umounter_automounter_get_type())
#define UMOUNTER_AUTOMOUNTER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), \
        UMOUNTER_TYPE_AUTOMOUNTER, UMounterAutomounter))
#define UMOUNTER_IS_AUTOMOUNTER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), \
        UMOUNTER_TYPE_AUTOMOUNTER))
#define UMOUNTER_AUTOMOUNTER_CLASS(cls) (G_TYPE_CHECK_CLASS_CAST((cls), \
        UMOUNTER_TYPE_AUTOMOUNTER, UMounterAutomounterClass))
#define UMOUNTER_IS_AUTOMOUNTER_CLASS(cls) (G_TYPE_CHECK_CLASS_TYPE((cls), \
        UMOUNTER_TYPE_AUTOMOUNTER))
#define UMOUNTER_AUTOMOUNTER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), \
        UMOUNTER_TYPE_AUTOMOUNTER, UMounterAutomounterClass))
#define UMOUNTER_AUTOMOUNTER_GET_PRIVATE(obj) \
        (G_TYPE_INSTANCE_GET_PRIVATE((obj), UMOUNTER_TYPE_AUTOMOUNTER, \
        UMounterAutomounterPrivate))

#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>

#include "config.h"


typedef struct _UMounterAutomounter UMounterAutomounter;
typedef struct _UMounterAutomounterClass UMounterAutomounterClass;
typedef struct _UMounterAutomounterPrivate UMounterAutomounterPrivate;

struct _UMounterAutomounter {
    GObject parent_instance;

    UMounterAutomounterPrivate *priv;
};

struct _UMounterAutomounterClass {
    GObjectClass parent_class;
};


////////////////////////////////////////////////////////////////////////////////
/* Function declaration... all this function are accessible form other source
files and therefore something like public. :) */

GType 
umounter_automounter_get_type(void);

/* Create a new UMounterAutomounter instance and returns it. */
UMounterAutomounter* 
umounter_automounter_new(UMounterConfig *config);

/* Running the automounter... into the main loop. */
gboolean 
umounter_automounter_run(UMounterAutomounter *self);

/* Running the automounter... into the main loop, but in a own thread, so the
calling function can go one. NOTE: If u close the application, this thread
will be closed too. It is a better way to stop the UMounterAutomounter with
the right function, so it can clean all things that he change on the file 
system. */
gboolean
umounter_automounter_run_as_thread(UMounterAutomounter *self);

////////////////////////////////////////////////////////////////////////////////


#endif /* UMOUNTER_AUTOMOUNTER_H */

