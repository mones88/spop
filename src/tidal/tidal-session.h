//
// Created by mones on 11/12/15.
//

#ifndef SPOP_TIDAL_SESSION_H
#define SPOP_TIDAL_SESSION_H

#include "tidal-models.h"

char *session_id;
char *country_code;
long user_id;

void tidal_session_init();
void tidal_session_login(const char* username, const char* password);
void tidal_session_end();

TidalServiceResult *tidal_session_get_user_playlists();

#endif //SPOP_TIDAL_SESSION_H
