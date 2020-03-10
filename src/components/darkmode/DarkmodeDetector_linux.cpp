#include "DarkmodeDetector.hpp"
#ifdef Q_OS_LINUX
    #include <QApplication>
    #include <QStyle>

namespace Qv2ray::components::darkmode
{
    // Referenced from github.com/keepassxreboot/keepassxc. Licensed under GPL2/3.
    // Copyright (C) 2020 KeePassXC Team <team@keepassxc.org>
    bool detectDarkmode()
    {
        if (!qApp || !qApp->style())
        {
            return false;
        }
        return qApp->style()->standardPalette().color(QPalette::Window).toHsl().lightness() < 110;
    }
} // namespace Qv2ray::components::darkmode

#endif
