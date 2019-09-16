#include <QFileInfo>
#include <QStandardPaths>
#include <QTranslator>

#include "QvUtils.h"
#include "Qv2rayBase.h"
#include "QvRunguard.h"
#include "w_MainWindow.h"

using namespace Qv2ray;
using namespace Qv2ray::Utils;
using namespace Qv2ray::QvConfigModels;

bool initQv()
{
#ifdef QT_DEBUG
    QString configPath = QDir::homePath() + "/.qv2ray_debug";
#else
    QString configPath = QDir::homePath() + "/.qv2ray";
#endif
    /// Qv2ray Config Path and ends with "/"
    QString exeDefaultPath = configPath + "/vcore/v2ray";
    QString v2AssetsPath = configPath + "/vcore";
    //
#if defined(__WIN32)
    exeDefaultPath = exeDefaultPath + ".exe";
#elif defined(__linux__)
    // Special case for GNU/Linux
    //
    // Unused these values
    Q_UNUSED(v2AssetsPath)
    Q_UNUSED(exeDefaultPath)
    v2AssetsPath = "/etc/v2ray";
    exeDefaultPath = "/bin/v2ray";
#endif
    //
    SetConfigDirPath(configPath);
    auto ConfigDir = new QDir(configPath);

    if (!ConfigDir->exists()) {
        auto result = QDir().mkdir(QV2RAY_CONFIG_DIR_PATH);

        if (result) {
            LOG(MODULE_INIT, "Created Qv2ray config dir at: " + QV2RAY_CONFIG_DIR_PATH.toStdString())
        } else {
            LOG(MODULE_INIT, "Failed to create config dir at: " + QV2RAY_CONFIG_DIR_PATH.toStdString())
            return false;
        }
    }

    auto genPath = QV2RAY_CONFIG_DIR_PATH + "generated/";

    if (!QDir(genPath).exists()) {
        auto result2 = QDir().mkdir(genPath);

        if (result2) {
            LOG(MODULE_INIT, "Created config generation dir at: " + genPath.toStdString())
        } else {
            LOG(MODULE_INIT, "Failed to create config generation dir at: " + genPath.toStdString())
            return false;
        }
    }

    QFile configFile(QV2RAY_CONFIG_FILE_PATH);

    if (!configFile.exists()) {
        // This is first run!
        //
        // These below genenrated very basic global config.
        Qv2rayBasicInboundsConfig inboundSetting = Qv2rayBasicInboundsConfig("127.0.0.1", 1080, 8000);
        Qv2rayConfig conf = Qv2rayConfig("zh-CN", exeDefaultPath.toStdString(), v2AssetsPath.toStdString(), 2, inboundSetting);
        //
        // Save initial config.
        SetGlobalConfig(conf);
        SaveGlobalConfig();
        //
        LOG(MODULE_INIT, "Created initial config file.")
    } else {
        // Some config file upgrades.
        auto conf = JSONFromString(StringFromFile(&configFile));
        auto confVersion = conf["config_version"].toVariant().toString();
        auto newVersion = QSTRING(to_string(QV2RAY_CONFIG_VERSION));

        if (QString::compare(confVersion, newVersion) != 0) {
            conf = UpgradeConfig(stoi(conf["config_version"].toString().toStdString()), QV2RAY_CONFIG_VERSION, conf);
        }

        auto confObject = StructFromJSONString<Qv2rayConfig>(JSONToString(conf));
        SetGlobalConfig(confObject);
        SaveGlobalConfig();
        LOG(MODULE_INIT, "Loaded config file.")
    }

    return true;
}

int main(int argc, char *argv[])
{
    LOG("LICENCE", "\r\nThis program comes with ABSOLUTELY NO WARRANTY.\r\n"
        "This is free software, and you are welcome to redistribute it\r\n"
        "under certain conditions.\r\n"
        "\r\n"
        "Hv2ray Copyright (C) 2019 aliyuchang33\r\n"
        "Hv2ray/Qv2ray (partial) Copyright 2019 (C) SoneWinstone\r\n"
        "Qv2ray Copyright (C) 2019 Leroy.H.Y\r\n"
        "\r\n"
        "Qv2ray " QV2RAY_VERSION_STRING " running on " + QSysInfo::prettyProductName().toStdString() + " " + QSysInfo::currentCpuArchitecture().toStdString() +
        "\r\n")
    //
#ifdef QT_DEBUG
    LOG("DEBUG", "============================== This is a debug build, many features are not stable enough. ==============================")
#endif
    //
    QApplication _qApp(argc, argv);
    //
    // Qv2ray Initialize
    initQv();
    //
#ifdef _WIN32
    // Set special font in Windows
    QFont font;
    font.setPointSize(9);
    font.setFamily("微软雅黑");
    _qApp.setFont(font);
#endif
#ifdef __APPLE__
    _qApp.setStyle("fusion");
#endif
    // ----------------------------------------------------------- BEGIN FIND TRANSLATIONS
    LOG(MODULE_UI, "Obtaining UI Translation list...")
    QDirIterator it(":/translations");

    if (!it.hasNext()) {
        LOG(MODULE_UI, "FAILED to find any translations, please check your build script.")
        QvMessageBox(nullptr, "Cannot load languages", "Default English is used.");
    }

    while (it.hasNext()) {
        LOG(MODULE_UI, "Translations: " + it.next().toStdString())
    }

    // ----------------------------------------------------------- END FIND TRANSLATIONS
    //
    auto lang = GetGlobalConfig().language;
    auto qStringLang = QSTRING(lang);

    if (_qApp.installTranslator(getTranslator(&qStringLang))) {
        LOG(MODULE_UI, "Loaded translations " + lang)
    } else {
        QvMessageBox(
            nullptr, "Failed to load selected language.",
            "You may want to select another language in the Prefrences Window.\r\n");
    }

    RunGuard guard("Qv2ray-Instance-Identifier"
#ifdef QT_DEBUG
                   "DEBUG_VERSION"
#endif
                  );
#ifndef __APPLE__
    auto osslReqVersion = QSslSocket::sslLibraryBuildVersionString().toStdString();
    auto osslCurVersion = QSslSocket::sslLibraryVersionString().toStdString();
    LOG(MODULE_NETWORK, "Current OpenSSL version: " + osslCurVersion)

    if (!QSslSocket::supportsSsl()) {
        LOG(MODULE_NETWORK, "Required OpenSSL version: " + osslReqVersion)
        QvMessageBox(nullptr, QObject::tr("DependencyMissing"), QObject::tr("osslDependMissing,PleaseReDownload"));
        LOG(MODULE_NETWORK, "OpenSSL library MISSING, Quitting.")
        return -2;
    }

#endif

    if (!guard.isSingleInstance()) {
        LOG(MODULE_INIT, "Another Instance running, Quit.")
        QvMessageBox(nullptr, "Qv2ray", QObject::tr("#AnotherInstanceRunning"));
        return -1;
    }

    // Show MainWindow
    MainWindow w;
    return _qApp.exec();
}
