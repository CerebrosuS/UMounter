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


static gint get_user_id(const gchar *user);
static gint get_group_id(const gchar *group);


gint main(gint argc, gchar **argv) {

    /* We really need a minimum glib version... */
    if(!GLIB_CHECK_VERSION(2, 26, 0))
        g_error("GLib version 2.26.0 or above is needed");

    g_type_init();

    /* Get configuration... */
    GError *error = NULL;
    UMounterConfig *config = g_object_new(UMOUNTER_TYPE_CONFIG, NULL);
    if(!umounter_config_read(config, "./umounter.conf", &error)) {
        g_warning("Can't read configuration: %s", error->message);
        g_error_free(error);
    }

    /* Set the group and user of the actual process. */
    gchar *user;
    gchar *group;
    
    g_object_get(G_OBJECT(config), "user", &user, "group", &group, NULL);

    gint user_id = get_user_id(user);
    if(user_id < 0)
        g_error("User %s does not exist.", user);
    
    gint group_id = get_group_id(user);
    if(group_id < 0)
        g_error("Group %s does not exist.", group);

    g_free(user);
    g_free(group);

    if(setregid(group_id, group_id) < 0 || setreuid(user_id, user_id) < 0 ) {
        g_error("Can't change the process user id to %i and/or group id to %i.",
            user_id, group_id);
    }

    /* Running fork() to times, cause else we don't get a full user/group
    change. (filehandlers) */
//    if(fork() < 0 || fork() < 0) {
//        g_error("Can't change to the child process.");
//    }

    /* Create the automounter object and start it... */
    UMounterAutomounter *automounter = umounter_automounter_new(config);

    return umounter_automounter_run(automounter);
};

gint
get_user_id(const gchar *user) {
    struct passwd *user_passwd = NULL;
    user_passwd = getpwnam(user);
    if(NULL == user_passwd)
        return -1;
    
    gint user_id = user_passwd->pw_uid;
    
    return user_id;
}

gint
get_group_id(const gchar *group) {
    struct group *group_info = NULL;
    group_info = getgrnam(group);
    if(NULL == group_info)
        return -1;
    
    gint group_id = group_info->gr_gid;
    
    return group_id;
}

