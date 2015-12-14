//
// Created by mones on 13/12/15.
//

#include <stdlib.h>
#include "tidal-models.h"
#include "utils.h"

///
/// GENERIC RESULT
///

TidalServiceResult * tidal_service_result_create() {
    TidalServiceResult *result = malloc(sizeof(TidalServiceResult));
    result->firstItem = NULL;
    result->total = 0;
    return result;
}

void tidal_service_result_free(TidalServiceResult *result) {
    if (result->firstItem != NULL) {
        TidalListItem *item = result->firstItem;
        TidalListItem *tmp;
        while (item != NULL) {
            tmp = item;
            item = item->next;
            free(tmp);
        }
    }
    free(result);
}

void tidal_service_result_add_item(TidalServiceResult *result, void *item) {
    TidalListItem *newItem = malloc(sizeof(TidalListItem));
    newItem->next = NULL;
    newItem->item = item;

    if (result->firstItem == NULL) {
        result->firstItem = newItem;
    } else {
        TidalListItem *lastItem = result->firstItem;
        while (lastItem->next != NULL) {
            lastItem = lastItem->next;
        }
        lastItem->next = newItem;
    }
    result->total++;
}

///
/// PLAYLIST
///

void tidal_playlist_free(TidalPlaylist *playlist) {
    free(playlist->uuid);
    free(playlist->uuid);
    free(playlist->uuid);
    free(playlist->uuid);
    free(playlist->uuid);
    free(playlist->uuid);
    free(playlist);
}

TidalPlaylist * tidal_playlist_create(const char *uuid, const char *name, const char *desc, const char *type,
                                      gboolean public, const char *createdOn, const char *updatedOn, int audioTracks,
                                      int duration) {
    TidalPlaylist *result = malloc(sizeof(TidalPlaylist));
    result->uuid = string_copy_new(uuid);
    result->name = string_copy_new(name);
    result->description = string_copy_new(desc);
    result->type = string_copy_new(type);
    result->is_public = public;
    result->created = string_copy_new(createdOn);
    result->last_updated = string_copy_new(updatedOn);
    result->num_tracks = audioTracks;
    result->duration = duration;
    return result;
}
