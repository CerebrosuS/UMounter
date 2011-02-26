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

#include <glib.h>

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <stdlib.h>
#include <errno.h>

#include "automounter.h"
#include "config.h"
#include "logging.h"


gint main(gint argc, gchar **argv) {
    GError *error;
    GOptionContext *context;
    UMounterLogging *logging;
    
    gchar* config_file;
    gboolean info;
    gboolean debug;
    

    /* We really need a minimum glib version... */
    if(!GLIB_CHECK_VERSION(2, 26, 0))
        g_error("GLib version 2.26.0 or above is needed");

    error = NULL;
    g_type_init();

    /* First we parse all arguments. */
    config_file = g_build_path("/", g_get_home_dir(), 
        ".umounter/umounter.conf");
    debug = FALSE;
    info = FALSE;
    GOptionEntry option_entries[] = {
        {"config", 'c', 0, G_OPTION_ARG_STRING, &config_file, \
            "Define a configuration file.", NULL},
        {"debug", 'd', 0, G_OPTION_ARG_NONE, &debug, \
            "Show debug messages.", NULL},
        {"info", 'i', 0, G_OPTION_ARG_NONE, &info, \
            "Show info messages.", NULL},
        {NULL}    
    };

    context = g_option_context_new("");
    g_option_context_add_main_entries(context, option_entries, NULL);

    if(!g_option_context_parse(context, &argc, &argv, &error)) {
        g_error("Option parsing failed: %s\n", error->message);
        g_error_free(error);
        
        return 0;
    }

    /* Setting the logging object. */
    logging = umounter_logging_new();
    g_object_set(G_OBJECT(logging), "debug", debug, "info", info, NULL);

    /* Get configuration... */
    UMounterConfig *config = umounter_config_new();
    if(!umounter_config_read(config, "/home/christian/.umounter/umounter.conf", &error)) {
        g_warning("Can't read configuration: %s", error->message);
        g_error_free(error);
    }

    /* Create rulesparser... */
    UMounterRulesParser *rulesparser = umounter_rulesparser_new();

    /* Create the automounter object and start it... */
    UMounterAutomounter *automounter = umounter_automounter_new(config, 
        rulesparser);

    return umounter_automounter_run(automounter);
};

