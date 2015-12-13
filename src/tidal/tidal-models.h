//
// Created by mones on 13/12/15.
//

#ifndef SPOP_TIDAL_MODELS_H
#define SPOP_TIDAL_MODELS_H

#include <glib.h>

struct _tidal_playlist {
    char name[255];
    char description[255];
    char creator[255];
    char type[255];
    gboolean is_public;
    char created[255];
    char last_updated[255];
    int num_tracks;
    int duration;

};

typedef struct _tidal_playlist tidal_playlist;

tidal_playlist tidal_playlist_create();

void tidal_playlist_free(tidal_playlist *playlist);

#endif //SPOP_TIDAL_MODELS_H
