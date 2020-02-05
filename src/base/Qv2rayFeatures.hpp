#pragma once
// Qv2ray build features.
//
// Always use libgRPC++ on windows platform.
#ifndef WITH_LIB_GRPCPP
#   ifdef _WIN32
#       define WITH_LIB_GRPCPP
#   endif
#endif
