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
#include <signal.h>

#include "automounter.h"
#include "config.h"
#include "logging.h"
#include "string.h"


////////////////////////////////////////////////////////////////////////////////
/* Variables that are available for all functions in this file. */

static GMainLoop *main_loop;

////////////////////////////////////////////////////////////////////////////////
/* Function declaration... All functions being defined here are static and
therefore PRIVATE (can not be called outside this source file. */

static void signal_handler(int sigint);

////////////////////////////////////////////////////////////////////////////////
/* Main function... */

gint main(gint argc, gchar **argv) {
    GError *error;
    GOptionContext *context;
    
    UMounterLogging *logging;
    UMounterAutomounter *automounter;
    
    gchar* config_file;
    gchar* log_file_path;
    gboolean verbose;
    gboolean message;
    gboolean debug;
    gboolean log_to_file;
    

    /* We really need a minimum glib version... */
    if(!GLIB_CHECK_VERSION(2, 26, 0))
        g_error("GLib version 2.26.0 or above is needed");

    /* Init c signal handler. */
    signal(SIGINT, signal_handler);

    error = NULL;
    g_type_init();

    /* First we parse all arguments. */
    config_file = NULL;
    log_file_path = NULL;
    debug = FALSE;
    message = FALSE;
    verbose = FALSE;
    log_to_file = FALSE;
    GOptionEntry option_entries[] = {
        {"config", 0, 0, G_OPTION_ARG_STRING, &config_file,
            "Define a configuration file.", NULL},
        {"log-file", 0, 0, G_OPTION_ARG_STRING, &log_file_path,
            "Define a log file.", NULL},
        {"debug", 'd', 0, G_OPTION_ARG_NONE, &debug,
            "Show debug messages.", NULL},
        {"message", 'm', 0, G_OPTION_ARG_NONE, &message,
            "Show info messages.", NULL},
        {"verbose", 'v', 0, G_OPTION_ARG_NONE, &verbose,
            "Show all messages.", NULL},
        {"log-to-file", 'f', 0, G_OPTION_ARG_NONE, &log_to_file,
            "If true logging goes to a file.", NULL},
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
    if(TRUE == log_to_file && NULL == log_file_path)
        log_file_path = g_build_path("/", g_get_home_dir(), 
        ".umounter/umounter.log");

    logging = umounter_logging_new();
    g_object_set(G_OBJECT(logging), "debug", debug, "message", message, 
        "log_to_file", log_to_file, "log_file_path", log_file_path, "verbose",
        verbose, NULL);

    /* Get configuration... */
    UMounterConfig *config = umounter_config_new();

    /* If no filename for the configuration was given, we set a default name. */    
    if(NULL == config_file) {
        config_file = g_build_path("/", g_get_home_dir(), 
            ".umounter/umounter.conf", NULL);
    }

    if(!umounter_config_read(config, config_file, &error)) {
        g_warning("Can't read configuration: %s", error->message);
        g_error_free(error);
    }

    /* Create rulesparser... */
    UMounterRulesParser *rulesparser = umounter_rulesparser_new();

    /* Create the automounter object and start it... */
    automounter = umounter_automounter_new(config, rulesparser);

    /* Create a main loop an run it. */
    main_loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(main_loop);

    g_print("\nClean all for exit... ");

    /* Delete all objects... */
    g_object_unref(automounter);
    g_object_unref(logging);

    g_print("Ok!\n");

    return;
};

////////////////////////////////////////////////////////////////////////////////

static void signal_handler(int sigint) {
    g_main_loop_quit(main_loop);
}

