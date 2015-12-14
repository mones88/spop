//
// Created by mones on 11/12/15.
//

#include "tidal-session.h"
#include "tidal-config.h"
#include "utils.h"
#include <libsoup/soup.h>
#include <string.h>
#include <json-glib/json-glib.h>
#include <stdlib.h>
#include <stdio.h>
#include <libsoup/soup-message-body.h>

static SoupSession *session;
typedef void (MapperFunc)(void* result, SoupMessage *message, JsonNode *rootNode, JsonReader *jsonReader);

const void call_service(const char *http_method, const char *resource, const char *url_params, const char *body_params, MapperFunc mapper, void* result) {
    char url[255];
    snprintf(url, 255, "%s%s?sessionId=%s&countryCode=%s&limit=999&%s", api_location, resource, session_id, country_code, url_params ? url_params : "");
    g_info(url);

    SoupMessage *msg = soup_message_new(http_method, url);
    if (body_params) {
        soup_message_set_request(msg, "application/x-www-form-urlencoded", SOUP_MEMORY_COPY, body_params,
                                 strlen(body_params));
    }

    GInputStream *stream;
    GError *error = NULL;
    stream = soup_session_send (session, msg, NULL, &error);
    JsonParser *parser = json_parser_new();
    if (!json_parser_load_from_stream(parser, stream, NULL, NULL))
        g_error("can't read json response");

    JsonNode *rootNode = json_parser_get_root(parser);
    JsonReader *reader = json_reader_new(rootNode);

    mapper(result, msg, rootNode, reader);

    g_object_unref(stream);
    g_object_unref(reader);
    g_object_unref(parser);
}

void tidal_session_init() {
    session = soup_session_new();
}

void tidal_session_login(const char *username, const char *password) {
    char url[256];
    strcpy(url, api_location);
    strcat(url, "login/username?token=");
    strcat(url, api_token);
    char * formdata = encode_post_body_params(4, "username", username, "password", password);
    SoupMessage *msg = soup_message_new("POST", url);
    soup_message_set_request(msg, "application/x-www-form-urlencoded",
                              SOUP_MEMORY_COPY, formdata, strlen (formdata));
    GInputStream *stream;
    GError *error = NULL;
    stream = soup_session_send (session, msg, NULL, &error);
    int statusCode = msg->status_code;
    if (statusCode != 200)
        g_error("can't login to TIDAL! Status code: %d", statusCode);

    JsonParser *parser = json_parser_new();
    if (!json_parser_load_from_stream(parser, stream, NULL, NULL))
        g_error("can't read json response");

    JsonNode *rootNode = json_parser_get_root(parser);
    JsonReader *reader = json_reader_new(rootNode);

    user_id = json_helper_get_int(reader, "userId");
    session_id = json_helper_get_string(reader, "sessionId");
    country_code = json_helper_get_string(reader, "countryCode");

    g_object_unref(stream);
    g_object_unref(reader);
    g_object_unref(parser);
    free(formdata);
}

void tidal_session_end() {
    free(session);
    free(country_code);
    free(session_id);
}

void map_playlist_count(void* result, SoupMessage *message, JsonNode *rootNode, JsonReader *jsonReader) {


    json_reader_read_member(jsonReader, "items");

    /*JsonNode *itemsNode = json_reader_get_value(jsonReader);
    JsonArray *itemsArray = json_node_get_array(itemsNode);*/
    gint total = json_reader_count_elements(jsonReader);
    for (u_int i = 0; i < total; ++i) {
        if (json_reader_read_element(jsonReader, i)) {
            TidalPlaylist *playlist = tidal_playlist_create(
                    json_helper_get_string(jsonReader, "uuid"),
                    json_helper_get_string(jsonReader, "title"),
                    json_helper_get_string(jsonReader, "description"),
                    json_helper_get_string(jsonReader, "type"),
                    json_helper_get_bool(jsonReader, "publicPlaylist"),
                    json_helper_get_string(jsonReader, "created"),
                    json_helper_get_string(jsonReader, "lastUpdated"),
                    (int)json_helper_get_int(jsonReader, "numberOfTracks"),
                    (int)json_helper_get_int(jsonReader, "duration"));
            tidal_service_result_add_item(result, playlist);
        }
    }
    json_reader_end_member(jsonReader);
}

TidalServiceResult *tidal_session_get_user_playlists() {
    char resource[255];
    snprintf(resource, 255, "users/%li/playlists", user_id);
    TidalServiceResult *result = tidal_service_result_create();
    call_service("GET", resource, NULL, NULL, map_playlist_count, result);
    return result;
}
