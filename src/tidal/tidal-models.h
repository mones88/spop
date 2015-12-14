//
// Created by mones on 13/12/15.
//

#ifndef SPOP_TIDAL_MODELS_H
#define SPOP_TIDAL_MODELS_H

#include <glib.h>

///
/// GENERIC RESULT
///

struct _tidal_list_item {
    void *item;
    void *next;
};

typedef struct _tidal_list_item TidalListItem;

struct _tidal_result {
    uint total;
    TidalListItem *firstItem;
};

typedef struct _tidal_result TidalServiceResult;

TidalServiceResult * tidal_service_result_create();
void tidal_service_result_free(TidalServiceResult *result);
void tidal_service_result_add_item(TidalServiceResult *result, void *item);

///
/// PLAYLIST
///

struct _tidal_playlist {
    char *uuid;
    char *name;
    char *description;
    char *type;
    gboolean is_public;
    char *created;
    char *last_updated;
    int num_tracks;
    int duration;

};

typedef struct _tidal_playlist TidalPlaylist;

TidalPlaylist *tidal_playlist_create(const char *uuid, const char *name, const char *desc, const char *type,
                                     gboolean public, const char *createdOn, const char *updatedOn, int audioTracks,
                                     int duration);

void tidal_playlist_free(TidalPlaylist *playlist);

#endif //SPOP_TIDAL_MODELS_H
