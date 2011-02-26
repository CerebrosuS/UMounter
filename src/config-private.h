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

#ifndef UMOUNTER_CONFIG_PRIVATE_H
#define UMOUNTER_CONFIG_PRIVATE_H


struct _UMounterConfigPrivate {
    GKeyFile *config_key_file;
    gchar *config_path;

    /* UMounter config variables. */

    /* The path for the rules. */
    gchar *rules_path;
    gboolean automount;
};


#endif /* UMOUNTER_CONFIG_PRIVATE_H */

