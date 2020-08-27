#include "DarkmodeDetector.hpp"

#include "base/Qv2rayBase.hpp"

#include <QApplication>
#include <QStyle>
#ifdef Q_OS_LINUX
#elif defined(Q_OS_WIN32)
    #include <QSettings>
#else
// TODO: macOS headers.
#endif

namespace Qv2ray::components::darkmode
{
    // Referenced from github.com/keepassxreboot/keepassxc. Licensed under GPL2/3.
    // Copyright (C) 2020 KeePassXC Team <team@keepassxc.org>
    bool isDarkMode()
    {
        if (!qApp || !qApp->style())
        {
            return false;
        }
        return qApp->style()->standardPalette().color(QPalette::Window).toHsl().lightness() < 110;
#if defined(Q_OS_WIN32)
        QSettings settings(R"(HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Themes\Personalize)", QSettings::NativeFormat);
        return settings.value("AppsUseLightTheme", 1).toInt() == 0;
#endif
    }

} // namespace Qv2ray::components::darkmode
