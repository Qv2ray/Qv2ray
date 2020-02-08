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

#ifndef Q_OS_WIN
#define Qv2ray_PRETTY_FUNCTION __PRETTY_FUNCTION__
#else
#define Qv2ray_PRETTY_FUNCTION __FUNCSIG__
#endif

#define __LOG_IMPL(LEVEL, MODULE, MSG) __QV2RAY_LOG_FUNC__(LEVEL, Qv2ray_PRETTY_FUNCTION, __LINE__, MODULE, MSG);

#define LOG(MODULE, MSG) __LOG_IMPL(QV2RAY_LOG_NORMAL, (MODULE), (MSG));
#define DEBUG(MODULE, MSG) __LOG_IMPL(QV2RAY_LOG_DEBUG, (MODULE), (MSG));

// Log modules used by Qv2ray
const inline QString INIT           =   "INIT"                   ;
const inline QString MESSAGING      =   "BASE-MESSAGING"         ;
const inline QString UI             =   "CORE-UI"                ;
const inline QString GRAPH          =   "CORE-UI-GRAPH"          ;
const inline QString SETTINGS       =   "CORE-SETTINGS"          ;
const inline QString VCORE          =   "CORE-VCORE"             ;
//
const inline QString CONNECTION     =   "CORE-CONNECTION"        ;
const inline QString SUBSCRIPTION   =   "CORE-SUBSCRIPTION"      ;
const inline QString IMPORT         =   "CORE-IMPORT"            ;
const inline QString EXPORT         =   "CORE-EXPORT"            ;
//
const inline QString NETWORK        =   "COMMON-NETWORK"         ;
const inline QString FILEIO         =   "COMMON-FILEIO"          ;
//
const inline QString PROXY          =   "COMPONENT-PROXY"        ;
const inline QString UPDATE         =   "COMPONENT-UPDATE"       ;
const inline QString PLUGIN         =   "COMPONENT-PLUGIN"       ;
