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
    QString configPath = "";
    QString exeDefaultPath = "";
#if defined(__WIN32) || defined(__APPLE__)
    // For Windows and MacOS, there's no such 'installation' of a software
    // package, So as what ShadowSocks and v2rayX does, save config files next to
    // the executable.
    configPath = "./qv2ray.conf.d";
    exeDefaultPath = "./v2ray";
#else
    // However, for linux, this software can be and/or will be provided as a
    // package and install to whatever /usr/bin or /usr/local/bin or even /opt/
    // Thus we save config files in the user's home directory.
    configPath = QDir::homePath() + "/.qv2ray/";
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

    if (!QDir(QV2RAY_GENERATED_CONFIG_DIRPATH).exists()) {
        auto result2 = QDir().mkdir(QV2RAY_GENERATED_CONFIG_DIRPATH);

        if (result2) {
            LOG("Created config generation dir at: " + QV2RAY_GENERATED_CONFIG_DIRPATH.toStdString())
        } else {
            LOG("Failed to create config generation dir at: " + QV2RAY_GENERATED_CONFIG_DIRPATH.toStdString())
            return false;
        }
    }

    if (!Utils::CheckFile(ConfigDir, ".initialised")) {
        // This is first run!
        //
        // These below genenrated very basic global config.
        QvInbondSetting inHttp = QvInbondSetting(true, "127.0.0.1", 8080);
        QvInbondSetting inSocks = QvInbondSetting(true, "127.0.0.1", 1080);
        Qv2Config conf = Qv2Config("zh-CN", exeDefaultPath.toStdString(), "info", inHttp, inSocks);
        //
        // Save initial config.
        SetGlobalConfig(conf);
        SaveGlobalConfig();
        //
        // Create Placeholder for initialise indicator.
        QFile initPlaceHolder(QV2RAY_FIRSTRUN_IDENTIFIER);
        initPlaceHolder.open(QFile::WriteOnly);
        initPlaceHolder.close();
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
    QApplication _qApp(argc, argv);
    RunGuard guard("Qv2ray-Instance-Identifier");

    if (!guard.isSingleInstance()) {
        Utils::QvMessageBox(nullptr, QObject::tr("Qv2ray"), QObject::tr("AnotherInstanceRunning"));
        return -1;
    }

    // Qv2ray Initialize
    initializeQv();
    //

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

    // Show MainWindow
    MainWindow w;
    w.show();
    return _qApp.exec();
}
