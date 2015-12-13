//
// Created by mones on 11/12/15.
//

#ifndef SPOP_TIDAL_H
#define SPOP_TIDAL_H

/* Init functions */
void session_init();
void session_login(const char* username, const char* password);
void session_logout();

/* Playlist management */
int playlists_len();
sp_playlist* playlist_get(int nb);
//sp_playlist* playlist_get_from_link(sp_link* lnk);
sp_playlist_type playlist_type(int nb);
/*gchar* playlist_folder_name(int nb);
sp_playlist_offline_status playlist_get_offline_status(sp_playlist* pl);
void playlist_set_offline_mode(sp_playlist* pl, gboolean mode);
int playlist_get_offline_download_completed(sp_playlist* pl);*/

#endif //SPOP_TIDAL_H
