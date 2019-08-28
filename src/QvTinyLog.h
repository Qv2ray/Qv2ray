#ifndef QVTINYLOG_H
#define QVTINYLOG_H

#include <iostream>
using namespace std;
/*
 * Tiny log module.
 */
#define LOG(module, msg) cout << "[" << module << "]: " << msg << endl;

#define MODULE_INIT "INIT"
#define MODULE_UPDATE "UPDATE"
#define MODULE_VCORE "VCORE"
#define MODULE_CONFIG "CONFIG"
#define MODULE_CONNECTION_VMESS "CONNETION-VMESS"
#define MODULE_CONNECTION "CONNECTION"
#define MODULE_UI "UI"
#define MODULE_NETWORK "NETWORK"
#define MODULE_FILE "FILE"

#endif // QVTINYLOG_H
