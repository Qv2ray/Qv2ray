#include <QFileInfo>
#include <QStandardPaths>
#include <QTranslator>

#include "QvUtils.h"
#include "QvGUIConfigObjects.h"
#include "QvRunguard.h"
#include "w_MainWindow.h"

using namespace Qv2ray;
using namespace Qv2ray::Utils;
using namespace Qv2ray::QvConfigModels;

bool initializeQv()
{
    /// Qv2ray Config Path and ends with "/"
    QString configPath;
    QString exeDefaultPath;
    QString v2AssetsPath;
    //
#if defined(__WIN32)
    // For Windows, there's no such 'installation' of a software
    // package, So as what ShadowSocks and v2rayX does, save config files next to
    // the executable.
    configPath = QDir::homePath() + "/.qv2ray";
    exeDefaultPath = configPath + "/vcore/v2ray.exe";
    v2AssetsPath = configPath + "/vcore";
#else // NOT WINDOWS (*nix)
    // Global config dir.
    configPath = QDir::homePath() + "/.qv2ray";
    exeDefaultPath = configPath + "/vcore/v2ray";
    v2AssetsPath = configPath + "/vcore";
#endif
#ifdef __linux__
    // Special case for GNU/Linux
    // Unused these values
    Q_UNUSED(v2AssetsPath)
    Q_UNUSED(exeDefaultPath)
    v2AssetsPath = "/etc/v2ray";
    exeDefaultPath = "/bin/v2ray";
#endif
    SetConfigDirPath(configPath);
    auto ConfigDir = new QDir(configPath);

    if (!ConfigDir->exists()) {
        auto result = QDir().mkdir(QV2RAY_CONFIG_PATH);

        if (result) {
            LOG("Created Qv2ray config dir at: " + QV2RAY_CONFIG_PATH.toStdString())
        } else {
            LOG("Failed to create config dir at: " + QV2RAY_CONFIG_PATH.toStdString())
            return false;
        }
    }

    auto genPath = QV2RAY_CONFIG_PATH + "generated/";

    if (!QDir(genPath).exists()) {
        auto result2 = QDir().mkdir(genPath);

        if (result2) {
            LOG("Created config generation dir at: " + genPath.toStdString())
        } else {
            LOG("Failed to create config generation dir at: " + genPath.toStdString())
            return false;
        }
    }

    if (!QFile(QV2RAY_GUI_CONFIG_PATH).exists()) {
        // This is first run!
        //
        // These below genenrated very basic global config.
        QvBasicInboundSetting inboundSetting = QvBasicInboundSetting("127.0.0.1", 1080, 8000);
        Qv2Config conf = Qv2Config("zh-CN", exeDefaultPath.toStdString(), v2AssetsPath.toStdString(), 2, inboundSetting);
        //
        // Save initial config.
        SetGlobalConfig(conf);
        SaveGlobalConfig();
        //
        LOG("Created initial default config file.")
    } else {
        LoadGlobalConfig();
        LOG("Loaded config file.")
    }

    return true;
}

int main(int argc, char *argv[])
{
    LOG("Hv2ray Copyright (C) 2019 aliyuchang33 \r\n"
        "Hv2ray/Qv2ray (partial) Copyright (C) SoneWinstone (jianwenzhen@qq.com) \r\n"
        "Qv2ray Copyright (C) 2019 Leroy.H.Y \r\n"
        "\r\n"
        "This program comes with ABSOLUTELY NO WARRANTY.\r\n"
        "This is free software, and you are welcome to redistribute it\r\n"
        "under certain conditions.\r\n")
    QApplication _qApp(argc, argv);
    //
    // Qv2ray Initialize
    initializeQv();
    //
#ifdef _WIN32
    // Set special font in Windows
    QFont font;
    font.setPointSize(9);
    font.setFamily("微软雅黑");
    _qApp.setFont(font);
#endif

    if (_qApp.installTranslator(getTranslator(QString::fromStdString(GetGlobalConfig().language)))) {
        LOG("Loaded translations " + GetGlobalConfig().language)
    } else if (_qApp.installTranslator(getTranslator("en-US"))) {
        LOG("Loaded default translations")
    } else {
        QvMessageBox(
            nullptr, "Failed to load translations 无法加载语言文件",
            "Failed to load translations, user experience may be downgraded. \r\n"
            "无法加载语言文件，用户体验可能会降级.");
    }

#ifndef QT_NO_DEBUG
    QvMessageBox(nullptr, "Warning", "This is a debug build.");
    LOG("DEBUG BUILD!")
#else
    RunGuard guard("Qv2ray-Instance-Identifier");

    if (!guard.isSingleInstance()) {
        Utils::QvMessageBox(nullptr, QObject::tr("Qv2ray"), QObject::tr("#AnotherInstanceRunning"));
        return -1;
    }

#endif
    // Show MainWindow
    MainWindow w;
    return _qApp.exec();
}
