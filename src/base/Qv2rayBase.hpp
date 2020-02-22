#pragma once
//
#ifndef XTOSTRUCT_QT
#   define XTOSTRUCT_QT
#endif
//
#include <QtCore>
#include <QtGui>
#include <QApplication>
#include <QMap>
#include <vector>
#include <algorithm>
#include <iostream>
#include <ctime>
// Base support.
#include "base/Qv2rayLog.hpp"
#include "base/Qv2rayFeatures.hpp"
#include "base/JsonHelpers.hpp"
#include "base/GlobalInstances.hpp"
// Code Models
#include "base/models/QvSafeType.hpp"
#include "base/models/CoreObjectModels.hpp"
#include "base/models/QvConfigModel.hpp"
#include "base/models/QvConfigIdentifier.hpp"
#include "base/models/QvStartupConfig.hpp"
#include "base/models/QvRuntimeConfig.hpp"

using namespace std;
using namespace std::chrono;
using namespace Qv2ray;
using namespace Qv2ray::base;
using namespace Qv2ray::base::safetype;
using namespace Qv2ray::base::config;
using namespace Qv2ray::base::objects;
using namespace Qv2ray::base::objects::protocol;
using namespace Qv2ray::base::objects::transfer;

// Linux users and DEs should handle the darkMode UI themselves.
#ifndef QV2RAY_USE_BUILTIN_DARKTHEME
# ifndef Q_OS_LINUX
#  define QV2RAY_USE_BUILTIN_DARKTHEME
# endif
#endif

#define QV2RAY_BUILD_INFO QString(_QV2RAY_BUILD_INFO_STR_)
#define QV2RAY_BUILD_EXTRA_INFO QString(_QV2RAY_BUILD_EXTRA_INFO_STR_)

// Base folder suffix.
#ifdef QT_DEBUG
# define QV2RAY_CONFIG_DIR_SUFFIX "_debug/"
#else
# define QV2RAY_CONFIG_DIR_SUFFIX "/"
#endif

// Get Configured Config Dir Path
#define QV2RAY_CONFIG_DIR (Qv2ray::Qv2rayConfigPath)
#define QV2RAY_CONFIG_FILE (QV2RAY_CONFIG_DIR + "Qv2ray.conf")
#define QV2RAY_CONNECTIONS_DIR (QV2RAY_CONFIG_DIR + "connections/")
#define QV2RAY_SUBSCRIPTION_DIR (QV2RAY_CONFIG_DIR + "subscriptions/")

// Get GFWList and PAC file path.
#define QV2RAY_RULES_DIR (QV2RAY_CONFIG_DIR + "rules/")
#define QV2RAY_RULES_GFWLIST_PATH (QV2RAY_RULES_DIR + "gfwList.txt")
#define QV2RAY_RULES_PAC_PATH (QV2RAY_RULES_DIR + "pac.txt")

#define QV2RAY_CONFIG_FILE_EXTENSION ".qv2ray.json"
#define QV2RAY_GENERATED_DIR (QV2RAY_CONFIG_DIR + "generated/")
#define QV2RAY_GENERATED_FILE_PATH (QV2RAY_GENERATED_DIR + "config.gen.json")

#if ! defined (QV2RAY_DEFAULT_VCORE_PATH) && ! defined (QV2RAY_DEFAULT_VASSETS_PATH)
#   define QV2RAY_DEFAULT_VASSETS_PATH (QV2RAY_CONFIG_DIR + "vcore/")
#   ifdef Q_OS_WIN
#       define QV2RAY_DEFAULT_VCORE_PATH (QV2RAY_CONFIG_DIR + "vcore/v2ray.exe")
#   else
#       define QV2RAY_DEFAULT_VCORE_PATH (QV2RAY_CONFIG_DIR + "vcore/v2ray")
#   endif
#elif defined (QV2RAY_DEFAULT_VCORE_PATH) && defined (QV2RAY_DEFAULT_VASSETS_PATH)
// ---- Using user-specified VCore and VAssets path
#else
#   error Both QV2RAY_DEFAULT_VCORE_PATH and QV2RAY_DEFAULT_VASSETS_PATH need to be presented when using manually specify the paths.
#endif

#ifdef Q_OS_WIN
///    There's no tProxy thing on Windows....
//#    define QV2RAY_TPROXY_VCORE_PATH (QV2RAY_CONFIG_DIR + "vcore/v2ray.exe")
//#    define QV2RAY_TPROXY_VCTL_PATH (QV2RAY_CONFIG_DIR + "vcore/v2ctl.exe")
#else
#    define QV2RAY_TPROXY_VCORE_PATH (QV2RAY_CONFIG_DIR + "vcore/v2ray")
#    define QV2RAY_TPROXY_VCTL_PATH (QV2RAY_CONFIG_DIR + "vcore/v2ctl")
#endif

#define QV2RAY_VCORE_LOG_DIRNAME "logs/"
#define QV2RAY_VCORE_ACCESS_LOG_FILENAME "access.log"
#define QV2RAY_VCORE_ERROR_LOG_FILENAME "error.log"

// GUI TOOLS
#define RED(obj)                               \
    auto _temp = obj->palette();               \
    _temp.setColor(QPalette::Text, Qt::red);   \
    obj->setPalette(_temp);

#define BLACK(obj)                             \
    obj->setPalette(this->palette());

#define QV2RAY_UI_RESOURCES_ROOT ((GlobalConfig.uiConfig.useDarkTheme) ? QStringLiteral(":/assets/icons/ui_dark/") : QStringLiteral(":/assets/icons/ui_light/"))
#define QICON_R(file) QIcon(QV2RAY_UI_RESOURCES_ROOT + file)

#define QSTRN(num) QString::number(num)

#define OUTBOUND_TAG_DIRECT "outBound_DIRECT"
#define OUTBOUND_TAG_PROXY "outBound_PROXY"
#define OUTBOUND_TAG_FORWARD_PROXY "_QV2RAY_FORWARD_PROXY_"

#define API_TAG_DEFAULT "_QV2RAY_API_"
#define API_TAG_INBOUND "_QV2RAY_API_INBOUND_"

#define QV2RAY_USE_FPROXY_KEY "_QV2RAY_USE_GLOBAL_FORWARD_PROXY_"

#define JSON_ROOT_TRY_REMOVE(obj) if (root.contains(obj)) { root.remove(obj); }

namespace Qv2ray
{
    // Extra header for QvConfigUpgrade.cpp
    QJsonObject UpgradeConfig(int fromVersion, int toVersion, QJsonObject root);

    inline void ExitQv2ray()
    {
        isExiting = true;
        QApplication::quit();
    }
}
