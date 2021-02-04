#pragma once

#include "3rdparty/QJsonStruct/macroexpansion.hpp"
#include "base/Qv2rayBaseApplication.hpp"
#include "base/models/QvStartupConfig.hpp"

#include <QPair>
#include <QString>
#include <QTextStream>
#include <iostream>

#ifdef Q_OS_ANDROID
#include <android/log.h>
#endif

#define NEWLINE "\r\n"
#define ___LOG_EXPAND(___x) , QPair<std::string, decltype(___x)>(std::string(#___x), [&] { return ___x; }())
#define A(...) FOREACH_CALL_FUNC(___LOG_EXPAND, __VA_ARGS__)

#ifdef QT_DEBUG
#define QV2RAY_IS_DEBUG true
// __FILE__ ":" QT_STRINGIFY(__LINE__),
#define QV2RAY_LOG_PREPEND_CONTENT Q_FUNC_INFO,
#else
#define QV2RAY_IS_DEBUG false
#define QV2RAY_LOG_PREPEND_CONTENT
#endif

#define _LOG_ARG_(...) QV2RAY_LOG_PREPEND_CONTENT "[" QV_MODULE_NAME "]", __VA_ARGS__

#define LOG(...) Qv2ray::base::log_internal<QV2RAY_LOG_NORMAL>(_LOG_ARG_(__VA_ARGS__))
#define DEBUG(...) Qv2ray::base::log_internal<QV2RAY_LOG_DEBUG>(_LOG_ARG_(__VA_ARGS__))

enum QvLogType
{
    QV2RAY_LOG_NORMAL = 0,
    QV2RAY_LOG_DEBUG = 1
};

Q_DECLARE_METATYPE(const char *)

namespace Qv2ray::base
{
    inline QString logBuffer;
    inline QString tempBuffer;
    inline QTextStream logStream{ &logBuffer };
    inline QTextStream tempStream{ &tempBuffer };

    inline QString ReadLog()
    {
        return logStream.readAll();
    }

    template<QvLogType t, typename... T>
    inline void log_internal(T... v)
    {
        ((logStream << v << " "), ...);
        ((tempStream << v << " "), ...);
        logStream << NEWLINE;
#ifndef QT_DEBUG
        // We only process DEBUG log in Release mode
        // Prevent QvCoreApplication nullptr
        // TODO: Move log function inside QvCoreApplication
        if (t == QV2RAY_LOG_DEBUG && QvCoreApplication && !QvCoreApplication->StartupArguments.debugLog)
        {
            // Discard debug log in non-debug Qv2ray version with
            // no-debugLog mode.
            return;
        }
#endif

        const auto logString = tempStream.readAll();
#ifdef Q_OS_ANDROID
        __android_log_write(ANDROID_LOG_INFO, "Qv2ray", logString.toStdString().c_str());
#else
        std::cout << logString.toStdString() << std::endl;
#endif
    }
} // namespace Qv2ray::base

template<typename TKey, typename TVal>
QTextStream &operator<<(QTextStream &stream, const QPair<TKey, TVal> &pair)
{
    return stream << pair.first << ": " << pair.second;
}

inline QTextStream &operator<<(QTextStream &stream, const std::string &ss)
{
    return stream << ss.data();
}

template<typename TKey, typename TVal>
QTextStream &operator<<(QTextStream &stream, const QMap<TKey, TVal> &map)
{
    stream << "{ ";
    for (const auto &[k, v] : map.toStdMap())
        stream << QPair<TKey, TVal>(k, v) << "; ";
    stream << "}";
    return stream;
}

template<typename TVal>
QTextStream &operator<<(QTextStream &stream, const std::initializer_list<TVal> &init_list)
{
    for (const auto &x : init_list)
        stream << x;
    return stream;
}
