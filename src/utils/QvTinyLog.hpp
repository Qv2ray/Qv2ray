#ifndef QVTINYLOG_H
#define QVTINYLOG_H

#include <iostream>
#include <QtDebug>
#include <QBuffer>
using namespace std;

/*
 * Tiny log module.
 */

void _LOG(const std::string &module, const std::string &log);
const QString readLastLog();

#define LOG(module, msg) _LOG(module, msg);

#ifdef QT_DEBUG
#define DEBUG(module, msg) LOG("[DEBUG] - " module, msg)
#else
#define DEBUG(module, msg)
#endif

#define MODULE_INIT              "INIT"
#define MODULE_UPDATE            "UPDATE"
#define MODULE_VCORE             "VCORE"
#define MODULE_CONFIG            "CONFIG"
#define MODULE_PROXY             "PROXY"
#define MODULE_UI                "UI"
#define MODULE_NETWORK           "NETWORK"
#define MODULE_FILE              "FILE"
#define MODULE_SUBSCRIPTION      "SUBSCRIPTION"
#define MODULE_CONNECTION        "CONNECTION"
#define MODULE_IMPORT            "IMPORT"
#define MODULE_PLUGIN            "PLUGIN"

#endif // QVTINYLOG_H
