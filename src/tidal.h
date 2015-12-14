//
// Created by mones on 11/12/15.
//

#ifndef SPOP_TIDAL_H
#define SPOP_TIDAL_H

/* Init functions */
void session_init();
void session_login(const char* username, const char* password);
void session_logout();

//to be deleted
sp_playlist* playlist_get(int nb);
sp_playlist_type playlist_type(int nb);

#endif //SPOP_TIDAL_H
