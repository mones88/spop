//
// Created by mones on 11/12/15.
//

#include <errno.h>
#include <fcntl.h>
#include <glib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "spop.h"
#include "config.h"
#include "plugin.h"
#include "queue.h"
#include "tidal.h"
#include "tidal/tidal-config.h"
#include "tidal/tidal-session.h"

void session_init() {
    tidal_config_init();
    tidal_session_init();
}

void session_login(const char* username, const char* password) {
    g_debug("Logging in...");
    tidal_session_login(username, password);
}

void session_logout() {
    g_debug("Logging out...");
    tidal_session_end();
}

sp_playlist* playlist_get(int nb) {
    return NULL;
}
sp_playlist_type playlist_type(int nb) {
    return SP_PLAYLIST_TYPE_PLAYLIST;
}