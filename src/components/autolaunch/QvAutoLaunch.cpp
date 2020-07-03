#include "QvAutoLaunch.hpp"

#include <QApplication>
#include <QDir>
#include <QSettings>
#include <QStandardPaths>
#include <QTextStream>
// macOS headers (possibly OBJ-c)
#if defined(Q_OS_MAC)
    #include <CoreFoundation/CoreFoundation.h>
    #include <CoreServices/CoreServices.h>
#endif
namespace Qv2ray::components::autolaunch
{
    //
    //  launchatlogin.cpp
    //  ShadowClash
    //
    //  Created by TheWanderingCoel on 2018/6/12.
    //  Copyright © 2019 Coel Wu. All rights reserved.
    //
    QString getUserAutostartDir_private()
    {
        QString config = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
        config += QLatin1String("/autostart/");
        return config;
    }

    bool GetLaunchAtLoginStatus()
    {
#ifdef Q_OS_WIN
        QString appName = QApplication::applicationName();
        QSettings reg("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
        return reg.contains(appName);
    }

#elif defined Q_OS_MAC
        // From
        // https://github.com/nextcloud/desktop/blob/master/src/common/utility_mac.cpp
        // this is quite some duplicate code with setLaunchOnStartup, at some
        // point we should fix this FIXME.
        bool returnValue = false;
        QString filePath = QDir(QCoreApplication::applicationDirPath() + QLatin1String("/../..")).absolutePath();
        CFStringRef folderCFStr = CFStringCreateWithCString(0, filePath.toUtf8().data(), kCFStringEncodingUTF8);
        CFURLRef urlRef = CFURLCreateWithFileSystemPath(0, folderCFStr, kCFURLPOSIXPathStyle, true);
        LSSharedFileListRef loginItems = LSSharedFileListCreate(0, kLSSharedFileListSessionLoginItems, 0);

        if (loginItems)
        {
            // We need to iterate over the items and check which one is "ours".
            UInt32 seedValue;
            CFArrayRef itemsArray = LSSharedFileListCopySnapshot(loginItems, &seedValue);
            CFStringRef appUrlRefString = CFURLGetString(urlRef); // no need for release

            for (int i = 0; i < CFArrayGetCount(itemsArray); i++)
            {
                LSSharedFileListItemRef item = (LSSharedFileListItemRef) CFArrayGetValueAtIndex(itemsArray, i);
                CFURLRef itemUrlRef = NULL;

                if (LSSharedFileListItemResolve(item, 0, &itemUrlRef, NULL) == noErr && itemUrlRef)
                {
                    CFStringRef itemUrlString = CFURLGetString(itemUrlRef);

                    if (CFStringCompare(itemUrlString, appUrlRefString, 0) == kCFCompareEqualTo)
                    {
                        returnValue = true;
                    }

                    CFRelease(itemUrlRef);
                }
            }

            CFRelease(itemsArray);
        }

        CFRelease(loginItems);
        CFRelease(folderCFStr);
        CFRelease(urlRef);
        return returnValue;
    }

#elif defined Q_OS_LINUX
        QString appName = QApplication::applicationName();
        QString desktopFileLocation = getUserAutostartDir_private() + appName + QLatin1String(".desktop");
        return QFile::exists(desktopFileLocation);
    }
#endif

    void SetLaunchAtLoginStatus(bool enable)
    {
#ifdef Q_OS_WIN
        QString appName = QApplication::applicationName();
        QSettings reg("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);

        if (enable)
        {
            QString strAppPath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
            reg.setValue(appName, strAppPath);
        }
        else
        {
            reg.remove(appName);
        }
    }

#elif defined Q_OS_MAC
        // From
        // https://github.com/nextcloud/desktop/blob/master/src/common/utility_mac.cpp
        QString filePath = QDir(QCoreApplication::applicationDirPath() + QLatin1String("/../..")).absolutePath();
        CFStringRef folderCFStr = CFStringCreateWithCString(0, filePath.toUtf8().data(), kCFStringEncodingUTF8);
        CFURLRef urlRef = CFURLCreateWithFileSystemPath(0, folderCFStr, kCFURLPOSIXPathStyle, true);
        LSSharedFileListRef loginItems = LSSharedFileListCreate(0, kLSSharedFileListSessionLoginItems, 0);

        if (loginItems && enable)
        {
            // Insert an item to the list.
            LSSharedFileListItemRef item = LSSharedFileListInsertItemURL(loginItems, kLSSharedFileListItemLast, 0, 0, urlRef, 0, 0);

            if (item)
                CFRelease(item);

            CFRelease(loginItems);
        }
        else if (loginItems && !enable)
        {
            // We need to iterate over the items and check which one is "ours".
            UInt32 seedValue;
            CFArrayRef itemsArray = LSSharedFileListCopySnapshot(loginItems, &seedValue);
            CFStringRef appUrlRefString = CFURLGetString(urlRef);

            for (int i = 0; i < CFArrayGetCount(itemsArray); i++)
            {
                LSSharedFileListItemRef item = (LSSharedFileListItemRef) CFArrayGetValueAtIndex(itemsArray, i);
                CFURLRef itemUrlRef = NULL;

                if (LSSharedFileListItemResolve(item, 0, &itemUrlRef, NULL) == noErr && itemUrlRef)
                {
                    CFStringRef itemUrlString = CFURLGetString(itemUrlRef);

                    if (CFStringCompare(itemUrlString, appUrlRefString, 0) == kCFCompareEqualTo)
                    {
                        LSSharedFileListItemRemove(loginItems, item); // remove it!
                    }

                    CFRelease(itemUrlRef);
                }
            }

            CFRelease(itemsArray);
            CFRelease(loginItems);
        }

        CFRelease(folderCFStr);
        CFRelease(urlRef);
    }

#elif defined Q_OS_LINUX
        //
        // For AppImage packaging.
        auto binPath = qEnvironmentVariableIsSet("APPIMAGE") ? qEnvironmentVariable("APPIMAGE") : QCoreApplication::applicationFilePath();
        //
        // From https://github.com/nextcloud/desktop/blob/master/src/common/utility_unix.cpp
        QString appName = QApplication::applicationName();
        QString userAutoStartPath = getUserAutostartDir_private();
        QString desktopFileLocation = userAutoStartPath + appName + QLatin1String(".desktop");

        if (enable)
        {
            if (!QDir().exists(userAutoStartPath) && !QDir().mkpath(userAutoStartPath))
            {
                // qCWarning(lcUtility) << "Could not create autostart folder"
                // << userAutoStartPath;
                return;
            }

            QFile iniFile(desktopFileLocation);

            if (!iniFile.open(QIODevice::WriteOnly))
            {
                // qCWarning(lcUtility) << "Could not write auto start entry" <<
                // desktopFileLocation;
                return;
            }

            QTextStream ts(&iniFile);
            ts.setCodec("UTF-8");
            ts << QLatin1String("[Desktop Entry]") << endl
               << QLatin1String("Name=") << QApplication::applicationName() << endl
               << QLatin1String("GenericName=") << QLatin1String("V2ray Frontend") << endl
               << QLatin1String("Exec=") << binPath << endl
               << QLatin1String("Terminal=") << "false" << endl
               << QLatin1String("Icon=") << "qv2ray" << endl // always use lowercase for icons
               << QLatin1String("Categories=") << "Network" << endl
               << QLatin1String("Type=") << "Application" << endl
               << QLatin1String("StartupNotify=") << "false" << endl
               << QLatin1String("X-GNOME-Autostart-enabled=") << "true" << endl;
        }
        else
        {
            if (!QFile::remove(desktopFileLocation))
            {
                // qCWarning(lcUtility) << "Could not remove autostart desktop
                // file";
            }
        }
    }
#endif
} // namespace Qv2ray::components::autolaunch
