/*
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
 */

#include <glib.h>
#include <libspotify/api.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "spop.h"
#include "commands.h"
#include "queue.h"
#include "spotify.h"

/****************
 *** Commands ***
 ****************/
void list_playlists(GString* result) {
    int i, n, t;
    sp_playlist* pl;

    if (!container_loaded()) {
        g_string_assign(result, "- playlists container not loaded yet\n");
        return;
    }

    n = playlists_len();
    if (n == -1) {
        fprintf(stderr, "Could not determine the number of playlists\n");
        return;
    }
    if (g_debug)
        fprintf(stderr, "%d playlists\n", n);

    for (i=0; i<n; i++) {
        pl = playlist_get(i);
        if (!pl) {
            if (g_debug)
                fprintf(stderr, "Got NULL pointer when loading playlist %d.\n", i);
            continue;
        }
        if (!sp_playlist_is_loaded(pl)) {
            if (g_debug)
                fprintf(stderr, "Playlist %d is not loaded.\n", i);
            continue;
        }
        t = sp_playlist_num_tracks(pl);
        g_string_append_printf(result, "%d %s (%d)\n", i+1, sp_playlist_name(pl), t);
    }
}

void list_tracks(int idx, GString* result) {
    sp_playlist* pl;
    GArray* tracks;

    /* Get the playlist */
    pl = playlist_get(idx-1);
    if (!pl) {
        g_string_assign(result, "- invalid playlist\n");
        return;
    }
    
    /* Get the tracks array */
    tracks = tracks_get_playlist(pl);
    if (!tracks) {
        g_string_assign(result, "- playlist not laoded yet\n");
        return;
    }

    format_tracks_array(tracks, result);
    g_array_free(tracks, TRUE);
}

void list_queue(GString* result) {
    GArray* tracks;

    tracks = queue_tracks();
    if (!tracks) {
        fprintf(stderr, "Couldn't read queue.\n");
        exit(1);
    }

    format_tracks_array(tracks, result);
    g_array_free(tracks, TRUE);
}


void status(GString* result) {
    sp_track* track;
    int track_nb, total_tracks;
    queue_status qs;
    int track_min, track_sec, pos_min, pos_sec;
    const char* track_name;
    GString* track_artist = NULL;
    GString* track_album = NULL;
    GString* track_link = NULL;

    qs = queue_get_status(&track, &track_nb, &total_tracks);

    g_string_assign(result, "Status: ");
    if (qs == PLAYING) g_string_append(result, "playing");
    else if (qs == PAUSED) g_string_append(result, "paused");
    else g_string_append(result, "stopped");

    g_string_append_printf(result, "\nTotal tracks: %d\n", total_tracks);

    g_string_append(result, "Mode: ");
    if (queue_get_repeat()) {
        g_string_append(result, "repeat");
        if (queue_get_shuffle())
            g_string_append(result, " shuffle");
    }
    else {
        if (queue_get_shuffle())
            g_string_append(result, "shuffle");
        else
            g_string_append(result, "normal");
    }
    g_string_append(result, "\n");

    if (qs != STOPPED) {
        g_string_append_printf(result, "Current track: %d\n", track_nb+1);

        track_get_data(track, &track_name, &track_artist, &track_album, &track_link, &track_min, &track_sec);
        pos_sec = session_play_time();
        pos_min = pos_sec / 60;
        pos_sec %= 60;

        g_string_append_printf(result, "Artist: %s\nTitle: %s\nAlbum: %s\n",
                               track_artist->str, track_name, track_album->str);
        g_string_append_printf(result, "Duration: %d:%02d\nPosition: %d:%02d\nURI: %s\n",
                               track_min, track_sec, pos_min, pos_sec, track_link->str);
        g_string_free(track_artist, TRUE);
        g_string_free(track_album, TRUE);
        g_string_free(track_link, TRUE);
    }
}

void repeat(GString* result) {
    gboolean r = queue_get_repeat();
    queue_set_repeat(TRUE, !r);
    status(result);
}

void shuffle(GString* result) {
    gboolean s = queue_get_shuffle();
    queue_set_shuffle(TRUE, !s);
    status(result);
}

void play_playlist(int idx, GString* result) {
    sp_playlist* pl;

    /* First get the playlist */
    pl = playlist_get(idx-1);

    if (!pl) {
        g_string_assign(result, "- invalid playlist\n");
        return;
    }

    /* Load it and play it */
    queue_set_playlist(FALSE, pl);
    queue_play(TRUE);

    status(result);
}

void play_track(int pl_idx, int tr_idx, GString* result) {
    sp_playlist* pl;
    sp_track* tr;
    GArray* tracks;

    /* First get the playlist */
    pl = playlist_get(pl_idx-1);
    if (!pl) {
        g_string_assign(result, "- invalid playlist\n");
        return;
    }

    /* Then get the track itself */
    tracks = tracks_get_playlist(pl);
    if (!tracks) {
        g_string_assign(result, "- playlist not loaded yet.\n");
        return;
    }
    if ((tr_idx <= 0) || (tr_idx > tracks->len)) {
        g_string_assign(result, "- invalid track number\n");
        g_array_free(tracks, TRUE);
        return;
    }

    tr = g_array_index(tracks, sp_track*, tr_idx-1);
    g_array_free(tracks, TRUE);

    /* Load it and play it */
    queue_set_track(FALSE, tr);
    queue_play(TRUE);

    status(result);
}

void add_playlist(int idx, GString* result) {
    sp_playlist* pl;
    int tot;

    /* First get the playlist */
    pl = playlist_get(idx-1);

    if (!pl) {
        g_string_assign(result, "- invalid playlist\n");
        return;
    }

    /* Load it */
    queue_add_playlist(TRUE, pl);

    queue_get_status(NULL, NULL, &tot);
    g_string_printf(result, "Total tracks: %d\n", tot);
}

void add_track(int pl_idx, int tr_idx, GString* result) {
    sp_playlist* pl;
    sp_track* tr;
    GArray* tracks;
    int tot;

    /* First get the playlist */
    pl = playlist_get(pl_idx-1);
    if (!pl) {
        g_string_assign(result, "- invalid playlist\n");
        return;
    }

    /* Then get the track itself */
    tracks = tracks_get_playlist(pl);
    if (!tracks) {
        g_string_assign(result, "- playlist not loaded yet.\n");
        return;
    }
    if ((tr_idx <= 0) || (tr_idx > tracks->len)) {
        g_string_assign(result, "- invalid track number\n");
        g_array_free(tracks, TRUE);
        return;
    }

    tr = g_array_index(tracks, sp_track*, tr_idx-1);

    /* Load it */
    queue_add_track(TRUE, tr);

    queue_get_status(NULL, NULL, &tot);
    g_string_printf(result, "Total tracks: %d\n", tot);
}

void play(GString* result) {
    queue_play(TRUE);
    status(result);
}
void stop(GString* result) {
    queue_stop(TRUE);
    status(result);
}
void toggle(GString* result) {
    queue_toggle(TRUE);
    status(result);
}
void seek(int pos, GString* result) {
    queue_seek(pos);
    status(result);
}

void goto_next(GString* result) {
    queue_next(TRUE);
    status(result);
}
void goto_prev(GString* result) {
    queue_prev(TRUE);
    status(result);
}
void goto_nb(GString* result, int nb) {
    queue_goto(TRUE, nb-1);
    status(result);
}


/************************
 *** Helper functions ***
 ************************/
void format_tracks_array(GArray* tracks, GString* dst) {
    int i;
    sp_track* track;

    bool track_available;
    int track_min, track_sec;
    const char* track_name;
    GString* track_artist = NULL;
    GString* track_album = NULL;
    GString* track_link = NULL;

    /* If the playlist is empty, just add a newline (an empty string would mean "error") */
    if (tracks->len == 0) {
        g_string_assign(dst, "\n");
        return;
    }

    /* For each track, add a line to the dst string */
    for (i=0; i < tracks->len; i++) {
        track = g_array_index(tracks, sp_track*, i);
        if (!sp_track_is_loaded(track)) continue;

        track_available = sp_track_is_available(track);
        track_get_data(track, &track_name, &track_artist, &track_album, &track_link, &track_min, &track_sec);

        g_string_append_printf(dst, "%d%s %s -- \"%s\" -- \"%s\" (%d:%02d) URI:%s\n",
                               i+1, (track_available ? "" : "-"), track_artist->str,
                               track_album->str, track_name, track_min, track_sec, 
                               track_link->str);
        g_string_free(track_artist, TRUE);
        g_string_free(track_album, TRUE);
        g_string_free(track_link, TRUE);
    }
}

