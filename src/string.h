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

#ifndef UMOUNTER_STRING_H
#define UMOUNTER_STRING_H


#include <glib.h>

#include <string.h>


////////////////////////////////////////////////////////////////////////////////
/* Function declaration... all this function are accessible form other source
files and therefore something like public. :) */

/* This function returns a new allocated string. The first finding search string
in the source string will be replaced with the replace string. */
gchar* 
umounter_string_replace_once(const gchar *source, const gchar *search, 
    const gchar *replace);

/* This function returns a new allocated string. The findings  of search string
in the source string will be replaced with the replace string. */
gchar* 
umounter_string_replace(const gchar *source, const gchar *search, 
    const gchar *replace);


#endif /* UMOUNTER_STRING_H */
