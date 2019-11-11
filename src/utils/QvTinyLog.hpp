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

#ifdef QT_DEBUG
#define DEBUG(module, msg) LOG(module, msg)
#else
#define DEBUG(module, msg)
#endif

#define MODULE_INIT "INIT"
#define MODULE_UPDATE "UPDATE"
#define MODULE_VCORE "VCORE"
#define MODULE_CONFIG "CONFIG"
#define MODULE_UI "UI"
#define MODULE_NETWORK "NETWORK"
#define MODULE_FILE "FILE"
#define MODULE_SUBSCRIPTION "SUBSCRIPTION"
#define MODULE_CONNECTION "CONNECTION"
#define MODULE_IMPORT "IMPORT"
#define MODULE_CONNECTION_VMESS "CONNETION-VMESS"
#define MODULE_PLUGIN "PLUGIN"

#endif // QVTINYLOG_H
