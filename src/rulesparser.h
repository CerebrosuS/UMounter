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

#ifndef UMOUNTER_RULESPARSER_H
#define UMOUNTER_RULESPARSER_H


#define UMOUNTER_TYPE_RULESPARSER (umounter_config_get_type())
#define UMOUNTER_RULESPARSER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), \
        UMOUNTER_TYPE_RULESPARSER, UMounterRulesParser))
#define UMOUNTER_IS_RULESPARSER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), \
        UMOUNTER_TYPE_RULESPARSER))
#define UMOUNTER_RULESPARSER_CLASS(cls) (G_TYPE_CHECK_CLASS_CAST((cls), \
        UMOUNTER_TYPE_RULESPARSER, UMounterRulesParserClass))
#define UMOUNTER_IS_RULESPARSER_CLASS(cls) (G_TYPE_CHECK_CLASS_TYPE((cls), \
        UMOUNTER_TYPE_RULESPARSER))
#define UMOUNTER_RULESPARSER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), \
        UMOUNTER_TYPE_RULESPARSER, UMounterRulesParserClass))
#define UMOUNTER_RULESPARSER_GET_PRIVATE(obj) \
        (G_TYPE_INSTANCE_GET_PRIVATE((obj), UMOUNTER_TYPE_RULESPARSER, \
        UMounterRulesParserPrivate))

#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>

#include "volumes.h"


typedef struct _UMounterRulesParser UMounterRulesParser;
typedef struct _UMounterRulesParserClass UMounterRulesParserClass;
typedef struct _UMounterRulesParserPrivate UMounterRulesParserPrivate;

struct _UMounterRulesParser {
    GObject parent_instance;

    UMounterRulesParserPrivate *priv;
};

struct _UMounterRulesParserClass {
    GObjectClass parent_class;
};

////////////////////////////////////////////////////////////////////////////////
/* Function declaration... all this function are accessible form other source
files and therefore something like public. :) */

GType 
umounter_rulesparser_get_type(void);

/* Create a new UMounterRulesParser instance and returns it. */
UMounterRulesParser* 
umounter_rulesparser_new(void);

/* Parse all the files in the given path. Returns the created volumes as a 
list. */
UMounterVolumes*
umounter_rulseparser_parse(UMounterRulesParser *self, const gchar *path);

////////////////////////////////////////////////////////////////////////////////


#endif /* UMOUNTER_RULESPARSER_H */

