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
            conf = UpgradeConfig(stoi(confVersion.toStdString()), QV2RAY_CONFIG_VERSION, conf);
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
        "Qv2ray Current Developer Copyright (C) 2019 Leroy.H.Y (@lhy0403)\r\n"
        "Hv2ray Initial Idea and Designs Copyright (C) 2019 Hork (@aliyuchang33)\r\n"
        "Hv2ray/Qv2ray HTTP Request Helper (partial) Copyright 2019 (C) SOneWinstone (@SoneWinstone)\r\n"
        "Qv2ray ArtWork Done By ArielAxionL (@axionl)\r\n"
        "\r\n"
        "Qv2ray " QV2RAY_VERSION_STRING " running on " + QSysInfo::prettyProductName().toStdString() + " " + QSysInfo::currentCpuArchitecture().toStdString() +
        "\r\n")
    //
#ifdef QT_DEBUG
    LOG("DEBUG", "============================== This is a debug build, many features are not stable enough. ==============================")
#endif
    QDirIterator it(":/translations");

    if (!it.hasNext()) {
        LOG(MODULE_UI, "FAILED to find any translations, THIS IS A BUILD ERROR.")
        QvMessageBox(nullptr, "Cannot load languages", "Qv2ray will run, but you are not able to select languages.");
    }

    while (it.hasNext()) {
        LOG(MODULE_UI, "Found Translator: " + it.next().toStdString())
    }

    //
    QApplication _qApp(argc, argv);
    // Qv2ray Initialize
    initQv();
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
    auto lang = GetGlobalConfig().language;
    auto qStringLang = QSTRING(lang);

    if (_qApp.installTranslator(getTranslator(&qStringLang)) || qStringLang == "en-US") {
        LOG(MODULE_UI, "Loaded Translator " + lang)
    } else {
        // Do not translate these.....
        QvMessageBox(
            nullptr, "Translation Failed",
            "We cannot load translation for " + qStringLang + ", English is now used.\r\n\r\n "
            "Please go to Prefrence Window to change or Report a Bug at: \r\n"
            "https://github.com/lhy0403/Qv2ray/issues/new");
    }

    RunGuard guard("Qv2ray-Instance-Identifier"
#ifdef QT_DEBUG
                   "DEBUG_VERSION"
#endif
                  );
    auto osslReqVersion = QSslSocket::sslLibraryBuildVersionString().toStdString();
    auto osslCurVersion = QSslSocket::sslLibraryVersionString().toStdString();
    LOG(MODULE_NETWORK, "Current OpenSSL version: " + osslCurVersion)

    if (!QSslSocket::supportsSsl()) {
        LOG(MODULE_NETWORK, "Required OpenSSL version: " + osslReqVersion)
        LOG(MODULE_NETWORK, "OpenSSL library MISSING, Quitting.")
        QvMessageBox(nullptr, QObject::tr("DependencyMissing"),
                     QObject::tr("Cannot find openssl libs") + "\r\n" +
                     QObject::tr("This could be caused by a missing of `openssl` package in your system. Or an AppImage issue.") + "\r\n" +
                     QObject::tr("If you are using AppImage, please report a bug."));
        return -2;
    }

    if (!guard.isSingleInstance()) {
        LOG(MODULE_INIT, "Another Instance running, Quit.")
        QvMessageBox(nullptr, "Qv2ray", QObject::tr("Another instance of Qv2ray is already running."));
        return -1;
    }

    // Show MainWindow
    MainWindow w;
    return _qApp.exec();
}
