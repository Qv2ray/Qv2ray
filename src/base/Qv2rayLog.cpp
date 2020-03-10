#include "Qv2rayLog.hpp"

#include "base/GlobalInstances.hpp"

#include <iostream>

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
        auto funcPrepend = QString::fromStdString(func + ":" + to_string(line) + " ");

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
        cout << logString.toStdString() << endl;
        {
            QMutexLocker _(&__loggerMutex);
            __loggerBuffer->append(logString + NEWLINE);
        }
    } // namespace Qv2ray::base

    const QString readLastLog()
    {
        QMutexLocker _(&__purgerMutex);
        {
            QMutexLocker _(&__loggerMutex);
            __loggerBuffer.swap(__purgerBuffer);
        }
        auto result = __purgerBuffer->join("");
        __purgerBuffer->clear();
        return result;
    }
} // namespace Qv2ray::base
