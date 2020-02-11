#include "Qv2rayLog.hpp"
#include "GlobalInstances.hpp"

namespace Qv2ray::base
{
    // Forwarded from QvTinyLog
    static QQueue<QString> __loggerBuffer;
    static QMutex mutex;

    void __QV2RAY_LOG_FUNC__(int type, const std::string &func, int line, const QString &module, const QString &log)
    {
        auto logString = "[" + module + "]: " + log;
        auto funcPrepend = QString::fromStdString(func + ":" + to_string(line) + " ");

        if (isDebugBuild) {
            // Debug build version, we only print info for DEBUG logs and print ALL info when debugLog presents,
            if (type == QV2RAY_LOG_DEBUG || StartupOption.debugLog) {
                logString = logString.prepend(funcPrepend);
            }
        } else {
            // We only process DEBUG log in Release mode
            if (type == QV2RAY_LOG_DEBUG) {
                if (StartupOption.debugLog) {
                    logString = logString.prepend(funcPrepend);
                } else {
                    // Discard debug log in non-debug Qv2ray version with no-debugLog mode.
                    return;
                }
            }
        }

        mutex.lock();
        cout << logString.toStdString() << endl;
        __loggerBuffer.enqueue(logString + NEWLINE);
        mutex.unlock();
    }

    const QString readLastLog()
    {
        QString result;

        mutex.lock();
        while (!__loggerBuffer.isEmpty()) {
            auto str = __loggerBuffer.dequeue();

            if (!str.trimmed().isEmpty()) {
                result += str;
            }
        }
        mutex.unlock();

        return result;
    }
}
