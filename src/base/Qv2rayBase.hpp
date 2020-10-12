#pragma once
//
#include <QMap>
#include <QtCore>
#include <algorithm>
#include <ctime>
#include <iostream>
#include <optional>
#include <vector>
// Base support.
#include "base/JsonHelpers.hpp"
#include "base/Qv2rayFeatures.hpp"
#include "base/Qv2rayLog.hpp"
// Code Models
#include "base/Qv2rayBaseApplication.hpp"
#include "base/models/CoreObjectModels.hpp"
#include "base/models/QvConfigIdentifier.hpp"
#include "base/models/QvRuntimeConfig.hpp"
#include "base/models/QvSafeType.hpp"
#include "base/models/QvSettingsObject.hpp"
#include "base/models/QvStartupConfig.hpp"

using namespace Qv2ray;
using namespace Qv2ray::base;
using namespace Qv2ray::base::safetype;
using namespace Qv2ray::base::config;
using namespace Qv2ray::base::objects;
using namespace Qv2ray::base::objects::protocol;
using namespace Qv2ray::base::objects::transfer;

#define GlobalConfig (qvApplicationInstance->ConfigObject)
#define isExiting (qvApplicationInstance->IsExiting)
#define Qv2rayConfigPath (qvApplicationInstance->ConfigPath)

#define QV2RAY_BUILD_INFO QString(_QV2RAY_BUILD_INFO_STR_)
#define QV2RAY_BUILD_EXTRA_INFO QString(_QV2RAY_BUILD_EXTRA_INFO_STR_)

// Base folder suffix.
#ifdef QT_DEBUG
    #define QV2RAY_CONFIG_DIR_SUFFIX "_debug/"
    #define _BOMB_ (static_cast<QObject *>(nullptr)->event(nullptr))
#else
    #define _BOMB_
    #define QV2RAY_CONFIG_DIR_SUFFIX "/"
#endif

#ifdef Q_OS_WIN
    #define QV2RAY_EXECUTABLE_SUFFIX ".exe"
#else
    #define QV2RAY_EXECUTABLE_SUFFIX ""
#endif

// Get Configured Config Dir Path
#define QV2RAY_CONFIG_DIR (Qv2rayConfigPath)
#define QV2RAY_CONFIG_FILE (QV2RAY_CONFIG_DIR + "Qv2ray.conf")
//
#define QV2RAY_CONNECTIONS_DIR (QV2RAY_CONFIG_DIR + "connections/")
//
#define QV2RAY_PLUGIN_SETTINGS_DIR (QV2RAY_CONFIG_DIR + "plugin_settings/")
//
#define QV2RAY_CONFIG_FILE_EXTENSION ".qv2ray.json"
#define QV2RAY_GENERATED_DIR (QV2RAY_CONFIG_DIR + "generated/")

#if !defined(QV2RAY_DEFAULT_VCORE_PATH) && !defined(QV2RAY_DEFAULT_VASSETS_PATH)
    #define QV2RAY_DEFAULT_VASSETS_PATH (QV2RAY_CONFIG_DIR + "vcore/")
    #define QV2RAY_DEFAULT_VCORE_PATH (QV2RAY_CONFIG_DIR + "vcore/v2ray" QV2RAY_EXECUTABLE_SUFFIX)
    #define QV2RAY_DEFAULT_VCTL_PATH (QV2RAY_CONFIG_DIR + "vcore/v2ctl" QV2RAY_EXECUTABLE_SUFFIX)
#elif defined(QV2RAY_DEFAULT_VCORE_PATH) && defined(QV2RAY_DEFAULT_VASSETS_PATH)
// ---- Using user-specified VCore and VAssets path
#else
    #error Both QV2RAY_DEFAULT_VCORE_PATH and QV2RAY_DEFAULT_VASSETS_PATH need to be presented when using manually specify the paths.
#endif

#define QV2RAY_TPROXY_VCORE_PATH (QV2RAY_CONFIG_DIR + "vcore/v2ray" QV2RAY_EXECUTABLE_SUFFIX)
#define QV2RAY_TPROXY_VCTL_PATH (QV2RAY_CONFIG_DIR + "vcore/v2ctl" QV2RAY_EXECUTABLE_SUFFIX)

constexpr auto QV2RAY_VCORE_LOG_DIRNAME = "logs/";
constexpr auto QV2RAY_VCORE_ACCESS_LOG_FILENAME = "access.log";
constexpr auto QV2RAY_VCORE_ERROR_LOG_FILENAME = "error.log";

#ifdef Q_OS_MACOS
    #define ACCESS_OPTIONAL_VALUE(obj) (*obj)
#else
    #define ACCESS_OPTIONAL_VALUE(obj) (obj.value())
#endif

#define QSTRN(num) QString::number(num)

#define OUTBOUND_TAG_BLACKHOLE "BLACKHOLE"
#define OUTBOUND_TAG_DIRECT "DIRECT"
#define OUTBOUND_TAG_PROXY "PROXY"
#define OUTBOUND_TAG_FORWARD_PROXY "_QV2RAY_FORWARD_PROXY_"

#define API_TAG_DEFAULT "_QV2RAY_API_"
#define API_TAG_INBOUND "_QV2RAY_API_INBOUND_"

#define QV2RAY_USE_FPROXY_KEY "_QV2RAY_USE_GLOBAL_FORWARD_PROXY_"

namespace Qv2ray
{
    inline QStringList Qv2rayAssetsPaths(const QString &dirName)
    {
#define makeAbs(p) QDir(p).absolutePath()
        // Configuration Path
        QStringList list;
        // This is the default behavior on Windows
        list << makeAbs(QCoreApplication::applicationDirPath() + "/" + dirName);
        list << makeAbs(QV2RAY_CONFIG_DIR + dirName);
        list << ":/" + dirName;
        //
        list << QStandardPaths::locateAll(QStandardPaths::AppDataLocation, dirName, QStandardPaths::LocateDirectory);
        list << QStandardPaths::locateAll(QStandardPaths::AppConfigLocation, dirName, QStandardPaths::LocateDirectory);

#ifdef Q_OS_LINUX
        // For AppImage?
        if (qEnvironmentVariableIsSet("APPIMAGE"))
            list << makeAbs(QCoreApplication::applicationDirPath() + "/../share/qv2ray/" + dirName);
        // For Snap
        if (qEnvironmentVariableIsSet("SNAP"))
            list << makeAbs(qEnvironmentVariable("SNAP") + "/usr/share/qv2ray/" + dirName);
        // Linux platform directories.
        list << makeAbs("/usr/local/lib/qv2ray/" + dirName);
        list << makeAbs("/usr/lib/qv2ray/" + dirName);
        list << makeAbs("/lib/qv2ray/" + dirName);
        //
        list << makeAbs("/usr/local/share/qv2ray/" + dirName);
        list << makeAbs("/usr/share/qv2ray/" + dirName);
#elif defined(Q_OS_MAC)
        // macOS platform directories.
        list << QDir(QCoreApplication::applicationDirPath() + "/../Resources/" + dirName).absolutePath();
#endif
        list.removeDuplicates();
        return list;
#undef makeAbs
    }

} // namespace Qv2ray
