#ifndef QVTINYLOG_H
#define QVTINYLOG_H

#include <iostream>
#include <QtDebug>
using namespace std;
/*
 * Tiny log module.
 */
#define LOG(module, msg) cout << "[" << module << "]: " << msg << endl;
#define XLOG(module, level, msg) LOG(module, level << msg)

#define MODULE_INIT "INIT"
#define MODULE_UPDATE "UPDATE"
#define MODULE_VCORE "VCORE"
#define MODULE_CORE "CORE"
#define MODULE_CONFIG "CONFIG"
#define MODULE_CONNECTION_VMESS "CONNETION-VMESS"
#define MODULE_CONNECTION "CONNECTION"
#define MODULE_UI "UI"
#define MODULE_NETWORK "NETWORK"
#define MODULE_FILE "FILE"
#define MODULE_SUBSCRIPTION "SUBSCRIPTION"
#define MODULE_CONNECTION_IMPORT "CONNETION-IMPORT"
#define MODULE_PLUGIN "PLUGIN"
//
#ifdef QT_DEBUG
#define MODULE_DEBUG_INFO "__DEBUG__"
#endif
//
#define LOG_WARN "WARN"
#define LOG_INFO "INFO"
#define LOG_ERROR "ERROR"

#endif // QVTINYLOG_H
