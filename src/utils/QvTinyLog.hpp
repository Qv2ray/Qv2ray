#ifndef QVTINYLOG_H
#define QVTINYLOG_H

#include <iostream>
#include <QtDebug>
#include <QBuffer>
using namespace std;

/*
 * Tiny log module.
 */

void _LOG(const std::string &func, const std::string &module, const std::string &log);
const QString readLastLog();

#ifdef QV2RAY_LOG_WITH_FUNCTION_NAME
# define _QV2RAY_LOG_FUNCSTR __PRETTY_FUNCTION__
#else
# define _QV2RAY_LOG_FUNCSTR ""
#endif

#define LOG(module, msg) _LOG(_QV2RAY_LOG_FUNCSTR, module, msg);

#ifdef QT_DEBUG
#define DEBUG(module, msg) _LOG(__PRETTY_FUNCTION__, module, msg);
#else
#define DEBUG(module, msg)
#endif

// Log modules used by Qv2ray
#define MODULE_INIT              "INIT"
#define MODULE_UPDATE            "UPDATE"
#define MODULE_VCORE             "VCORE"
#define MODULE_CONFIG            "CONFIG"
#define MODULE_PROXY             "PROXY"
#define MODULE_UI                "UI"
#define MODULE_GRAPH             "GRAPH-NODE"
#define MODULE_NETWORK           "NETWORK"
#define MODULE_FILE              "FILE"
#define MODULE_SUBSCRIPTION      "SUBSCRIPTION"
#define MODULE_CONNECTION        "CONNECTION"
#define MODULE_IMPORT            "IMPORT"
#define MODULE_PLUGIN            "PLUGIN"

#endif // QVTINYLOG_H
