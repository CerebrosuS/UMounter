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

#ifndef UMOUNTER_VOLUME_PRIVATE_H
#define UMOUNTER_VOLUME_PRIVATE_H


struct _UMounterVolumePrivate {
    gchar *uuid;
    gchar *name;
    gchar *device;
    gboolean ignore_mount;
    gboolean is_mounted;
    gboolean is_available;
};


#endif /* UMOUNTER_VOLUME_PRIVATE_H */

