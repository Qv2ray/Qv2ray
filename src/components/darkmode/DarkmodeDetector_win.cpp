#include "DarkmodeDetector.hpp"
#ifdef Q_OS_WIN32
    #include <QSettings>

namespace Qv2ray::components::darkmode
{
    // Referenced from github.com/keepassxreboot/keepassxc. Licensed under GPL2/3.
    // Copyright (C) 2020 KeePassXC Team <team@keepassxc.org>
    bool detectDarkmode()
    {
        QSettings settings(R"(HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Themes\Personalize)", QSettings::NativeFormat);
        return settings.value("AppsUseLightTheme", 1).toInt() == 0;
    }
} // namespace Qv2ray::components::darkmode

#endif
