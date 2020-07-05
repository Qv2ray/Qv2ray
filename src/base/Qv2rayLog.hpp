#pragma once

#include <QString>

/*
 * Tiny log module.
 */

namespace Qv2ray::base
{
    void __QV2RAY_LOG_FUNC__(int type, const std::string &func, int line, const QString &module, const QString &log);
    QString readLastLog();
} // namespace Qv2ray::base

#define NEWLINE "\r\n"

#define QV2RAY_LOG_NORMAL 0
#define QV2RAY_LOG_DEBUG 1

#define __LOG_IMPL(LEVEL, MODULE, MSG) ::Qv2ray::base::__QV2RAY_LOG_FUNC__(LEVEL, Q_FUNC_INFO, __LINE__, MODULE, MSG);

#define LOG(MODULE, MSG) __LOG_IMPL(QV2RAY_LOG_NORMAL, (MODULE), (MSG));
#define DEBUG(MODULE, MSG) __LOG_IMPL(QV2RAY_LOG_DEBUG, (MODULE), (MSG));

// Log modules used by Qv2ray
const inline QString MODULE_INIT = "INIT";
const inline QString MODULE_MESSAGING = "BASE-MESSAGING";
const inline QString MODULE_UI = "CORE-UI";
const inline QString MODULE_GRAPH = "CORE-UI-GRAPH";
const inline QString MODULE_SETTINGS = "CORE-SETTINGS";
const inline QString MODULE_VCORE = "CORE-VCORE";
//
const inline QString MODULE_CONNECTION = "CORE-CONNECTION";
const inline QString MODULE_SUBSCRIPTION = "CORE-SUBSCRIPTION";
const inline QString MODULE_IMPORT = "CORE-IMPORT";
const inline QString MODULE_EXPORT = "CORE-EXPORT";
//
const inline QString MODULE_NETWORK = "COMMON-NETWORK";
const inline QString MODULE_FILEIO = "COMMON-FILEIO";
//
const inline QString MODULE_PROXY = "COMPONENT-PROXY";
const inline QString MODULE_UPDATE = "COMPONENT-UPDATE";
const inline QString MODULE_PLUGINHOST = "COMPONENT-PLUGINHOST";
const inline QString MODULE_PLUGINCLIENT = "PLUGIN-CLIENT";
// ================================================================
const inline QString MODULE_CORE_HANDLER = "QV2RAY-CORE";
