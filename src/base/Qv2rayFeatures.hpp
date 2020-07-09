#pragma once
#include <QtGlobal>
// Qv2ray build features.

#ifdef Q_OS_LINUX
    #define NativeDarkmode 0
#elif defined(Q_OS_MAC)
    #define NativeDarkmode 0
#elif defined(Q_OS_WIN)
    #define NativeDarkmode 1
#endif

#define QvHasFeature(feat) ((feat / 1) == 1)
