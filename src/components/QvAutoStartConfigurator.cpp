#include "QvAutoStartConfigurator.hpp"
#include <QSettings>
#include <QApplication>

namespace Qv2ray
{
    namespace Components
    {
        bool InstallAutoStart()
        {
#ifdef Q_OS_WIN
            QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
            auto path = qApp->applicationFilePath();

            // Make it suitable for Windows.
            if (path.contains(" ")) {
                path = "\"" + path + "\"";
            }

            path = path.replace('/', '\\');
            LOG(MODULE_CONFIG, "Qv2ray executable path: " + path.toStdString())
            settings.setValue("Qv2ray", path);
            return true;
#else
            return false;
#endif
        }
        bool RemoveAutoStart()
        {
#ifdef Q_OS_WIN
            QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
            settings.remove("Qv2ray");
            return true;
#else
            return false;
#endif
        }
    }
}
