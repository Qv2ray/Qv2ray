#include "Qv2rayLog.hpp"

#include "Qv2rayBase.hpp"

#include <iostream>

#ifdef Q_OS_ANDROID
    #include <android/log.h>
#endif

Qv2rayConfigObject _qv2ray_global_config_impl_details::_GlobalConfig;
bool _qv2ray_global_config_impl_details::_isExiting;
QString _qv2ray_global_config_impl_details::_Qv2rayConfigPath;

namespace Qv2ray::base
{
    // Forwarded from QvTinyLog
    static auto __loggerBuffer = std::make_unique<QStringList>();
    static auto __purgerBuffer = std::make_unique<QStringList>();
    static QMutex __loggerMutex;
    static QMutex __purgerMutex;

    void __QV2RAY_LOG_FUNC__(int type, const std::string &func, int line, const QString &module, const QString &log)
    {
        auto logString = QString("[" % module % "]: " % log);
        auto funcPrepend = QString::fromStdString(func + ":" + std::to_string(line) + " ");

#ifdef QT_DEBUG
        // Debug build version, we only print info for DEBUG logs and print
        // ALL info when debugLog presents,
        if (type == QV2RAY_LOG_DEBUG || StartupOption.debugLog)
        {
            logString.prepend(funcPrepend);
        }
#else
        // We only process DEBUG log in Release mode
        if (type == QV2RAY_LOG_DEBUG)
        {
            if (StartupOption.debugLog)
            {
                logString.prepend(funcPrepend);
            }
            else
            {
                // Discard debug log in non-debug Qv2ray version with
                // no-debugLog mode.
                return;
            }
        }
#endif
#ifdef Q_OS_ANDROID
        __android_log_write(ANDROID_LOG_INFO, "Qv2ray", logString.toStdString().c_str());
#else
        std::cout << logString.toStdString() << std::endl;
#endif
        {
            QMutexLocker _(&__loggerMutex);
            __loggerBuffer->append(logString + NEWLINE);
        }
    } // namespace Qv2ray::base

    QString readLastLog()
    {
        QMutexLocker _(&__purgerMutex);
        {
            QMutexLocker locker(&__loggerMutex);
            __loggerBuffer.swap(__purgerBuffer);
        }
        auto result = __purgerBuffer->join("");
        __purgerBuffer->clear();
        return result;
    }
} // namespace Qv2ray::base
