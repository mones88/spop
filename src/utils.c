/*
 * Copyright (C) 2010, 2011, 2012, 2013, 2014, 2015 The spop contributors
 *
 * This file is part of spop.
 *
 * spop is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * spop is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * spop. If not, see <http://www.gnu.org/licenses/>.
 *
 * Additional permission under GNU GPL version 3 section 7
 *
 * If you modify this Program, or any covered work, by linking or combining it
 * with libspotify (or a modified version of that library), containing parts
 * covered by the terms of the Libspotify Terms of Use, the licensors of this
 * Program grant you additional permission to convey the resulting work.
 */

#include <glib.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "utils.h"

/* Replace all occurences of old by new in str
 * (from https://bugzilla.gnome.org/show_bug.cgi?id=65987) */
void g_string_replace(GString* str, const char* old, const gchar* new) {
    gchar *new_str, **arr;

    arr = g_strsplit(str->str, old, -1);
    if (arr != NULL && arr[0] != NULL)
        new_str = g_strjoinv(new, arr);
    else
        new_str = g_strdup(new);
    g_strfreev(arr);

    g_string_assign(str, new_str);
}

/* Add a line number with a fixed width determined by the greatest possible value */
void g_string_append_line_number(GString* str, int nb, int max_nb) {
    gchar fs[10];
    int nb_digits = 0;
    while (max_nb > 0) { max_nb /= 10; nb_digits += 1; }
    g_snprintf(fs, sizeof(fs), "%%%dd", nb_digits);
    g_string_append_printf(str, fs, nb);
}

char * encode_post_body_params(int count, ...) {
    char *result = malloc(sizeof(char));
    size_t resultSize = 1; //space for NULL termination
    va_list ap;
    va_start(ap, count);
    for (int i = 0; i < count / 2; i++) {
        char *key = va_arg(ap, char*);
        char *value = va_arg(ap, char*);
        size_t expandBy = strlen(key) + strlen(value) + 2; //key=value&
        resultSize += expandBy;
        result = realloc(result, resultSize * sizeof(char));
        strcat(result, key);
        strcat(result, "=");
        strcat(result, value);
        strcat(result, "&");
    }
    strcat(result, "\0");
    return result;
}

char * string_copy_new(const char *src) {
    if (src == NULL)
        return NULL;

    size_t srcLen = strlen(src);
    char *copy = malloc(sizeof(char) * (srcLen + 1));
    snprintf(copy, srcLen, "%s", src);
    return copy;
}

char * json_helper_get_string(JsonReader *reader, const char *elementName) {
    json_reader_read_member(reader, elementName);
    const char * value = json_reader_get_string_value(reader);
    char * copiedValue = malloc(sizeof(char) * (strlen(value) + 1));
    strcpy(copiedValue, value);
    json_reader_end_member(reader);
    return copiedValue;
}

gint64 json_helper_get_int(JsonReader *reader, const char *elementName) {
    json_reader_read_member(reader, elementName);
    gint64 value = json_reader_get_int_value(reader);
    json_reader_end_member(reader);
    return value;
}

gboolean json_helper_get_bool(JsonReader *reader, const char *elementName) {
    json_reader_read_member(reader, elementName);
    gboolean value = json_reader_get_boolean_value(reader);
    json_reader_end_member(reader);
    return value;
}
