#pragma once

#include <iostream>
#include <QtDebug>
#include <QBuffer>
using namespace std;

/*
 * Tiny log module.
 */
namespace Qv2ray::base
{
    void __QV2RAY_LOG_FUNC__(int type, const std::string &func, int line, const QString &module, const QString &log);
    const QString readLastLog();
}

#define NEWLINE "\r\n"

#define QV2RAY_LOG_NORMAL 0
#define QV2RAY_LOG_DEBUG  1

#define __LOG_IMPL(LEVEL, MODULE, MSG) __QV2RAY_LOG_FUNC__(LEVEL, __PRETTY_FUNCTION__, __LINE__, MODULE, MSG);

#define LOG(MODULE, MSG) __LOG_IMPL(QV2RAY_LOG_NORMAL, (MODULE), (MSG));
#define DEBUG(MODULE, MSG) __LOG_IMPL(QV2RAY_LOG_DEBUG, (MODULE), (MSG));

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
