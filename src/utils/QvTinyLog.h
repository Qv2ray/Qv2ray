#ifndef QVTINYLOG_H
#define QVTINYLOG_H

#include <iostream>
#include <QtDebug>
using namespace std;
/*
 * Tiny log module.
 */
#define LOG(module, msg) cout << "[" << module << "]: " << msg << endl;

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
//
#ifdef QT_DEBUG
#define MODULE_DEBUG_INFO "__DEBUG__"
#endif
//
#define WARN "WARN"
#define INFO "INFO"
#define ERROR "ERROR"

#endif // QVTINYLOG_H
