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

#include "string.h"


gchar* 
umounter_string_replace_once(const gchar *source, const gchar *search, 
    const gchar *replace) {

    g_return_val_if_fail(NULL != source, NULL);
    g_return_val_if_fail(NULL != search, NULL);
    g_return_val_if_fail(NULL != replace, NULL);

    gchar *tmp_string, *find_address;
    gint source_length;


    find_address = g_strstr_len(source, sizeof(gchar) * strlen(source), search);
    if(NULL == find_address)
        return NULL;

    tmp_string = g_malloc(strlen(source) * sizeof(gchar));
    if(NULL == tmp_string)
        return NULL;

	/* Make a copy of the source string. */
	strcpy(tmp_string, source);
    tmp_string[(gint) (find_address - source)] = '\0';

    /* Now cut the string with a \0 at the found position. */
    strcat(tmp_string, replace);
    strcat(tmp_string, (gchar*) find_address + strlen(search));

    return tmp_string;
}


gchar* 
umounter_string_replace(const gchar *source, const gchar *search, 
    const gchar *replace) {

    g_return_val_if_fail(NULL != source, NULL);
    g_return_val_if_fail(NULL != search, NULL);
    g_return_val_if_fail(NULL != replace, NULL);

    gchar *tmp_string, *return_string;


    /* Init local values. */
    return_string = tmp_string = NULL;

    do {
        if(NULL == return_string)
            tmp_string = umounter_string_replace_once(source, search, 
                replace);
        else
            tmp_string = umounter_string_replace_once(return_string, search, 
                replace);
        if(NULL != tmp_string) {
            if(NULL != return_string)
                g_free(return_string);

            return_string = tmp_string;
        }

    } while(NULL != tmp_string);

    g_debug("FUNC(%s) solved command to: %s", __FUNCTION__, return_string);

    return return_string;
}

