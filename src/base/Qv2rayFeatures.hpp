#pragma once
#include <QtGlobal>
// Qv2ray build features.

#ifdef Q_OS_LINUX
    #define CanHasLibQvb 1
    #define NativeDarkmode 0
#elif defined(Q_OS_MAC)
    #define CanHasLibQvb 1
    #define NativeDarkmode 0
#elif defined(Q_OS_WIN)
    #define CanHasLibQvb 0
    #define NativeDarkmode 1
#endif

#ifdef BACKEND_LIBQVB
    #if !QvHasFeature(CanHasLibQvb)
        #error Qv2ray API backend libQvb is not supported on this platform
    #endif
#endif

#define QvHasFeature(feat) ((feat / 1) == 1)
