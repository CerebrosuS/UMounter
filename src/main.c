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

#include "automounter.h"


gint main(gint argc, gchar **argv) {

    /* We really need a minimum glib version... */
    if(!GLIB_CHECK_VERSION(2, 26, 0))
        g_error("GLib version 2.26.0 or above is needed");

    g_type_init();

    /* Create the automounter object and start it... */
    UMounterAutomounter *automounter = umounter_automounter_new();

    return umounter_automounter_run(automounter);
};

