#pragma once
// Qv2ray build features.
//
// Always use libgRPC++ on windows platform.
#ifdef BACKEND_LIBQVB
    #ifdef _WIN32
        #error "libQvb is not supported on Windows Platform"
    #endif
#endif
