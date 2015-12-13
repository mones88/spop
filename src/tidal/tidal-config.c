//
// Created by mones on 11/12/15.
//

#include <glib.h>
#include <config.h>
#include <stdlib.h>
#include <string.h>
#include "tidal-config.h"

#define TIDAL_API_BASE_URL "https://api.tidalhifi.com/v1/"

void tidal_config_init() {
    api_location = TIDAL_API_BASE_URL;
    if (config_get_bool_opt("use_flac", FALSE)) {
        api_token = "P5Xbeo5LFvESeDy6";
        quality = "LOSSLESS";
    }  else {
        api_token = "wdgaB1CilGA-S_s2";
        quality = "HIGH";
    }
}
