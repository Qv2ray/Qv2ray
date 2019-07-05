#include <QDebug>
#include <QFileInfo>
#include <QStandardPaths>
#include <QTranslator>
#include <iostream>

#include "HUtils.hpp"
#include "HConfigObjects.hpp"
#include "runguard.hpp"
#include "w_MainWindow.h"

using namespace std;
using namespace Hv2ray;
using namespace Hv2ray::Utils;
using namespace Hv2ray::HConfigModels;

bool initializeHv()
{
    /// Hv2ray Config Path and ends with "/"
    QString configPath = "";
#if defined(__WIN32) || defined(__APPLE__)
    // For Windows and MacOS, there's no such 'installation' of a software
    // package, So as what ShadowSocks and v2rayX does, save config files next to
    // the executable.
    configPath = HV2RAY_CONFIG_DIR_NAME;
#else
    // However, for linux, this software can be and/or will be provided as a
    // package and install to whatever /usr/bin or /usr/local/bin or even /opt/
    // Thus we save config files in the user's home directory.
    configPath = QDir::homePath() + HV2RAY_CONFIG_DIR_NAME;
#endif
    ConfigDir = QDir(configPath);

    if (!ConfigDir.exists()) {
        auto result = QDir().mkdir(configPath);

        if (result) {
            qDebug() << "Created hv2ray config file path at: " + configPath;
        } else {
            // We cannot continue as it failed to create a dir.
            qDebug() << "Failed to create config file folder under " + configPath;
            return false;
        }
    }

    QFile configFile(configPath + "hv2ray.conf");

    if (!Utils::hasFile(&ConfigDir, ".initialised")) {
        // This is first run!
        // These below genenrated very basic global config.
        HInbondSetting inHttp = HInbondSetting(true, "127.0.0.1", 8080);
        HInbondSetting inSocks = HInbondSetting(true, "127.0.0.1", 1080);
        Hv2Config conf = Hv2Config("zh-CN", "info", inHttp, inSocks);
        SetGlobalConfig(conf);
        SaveConfig(&configFile);
        // Create Placeholder for initialise indicator.
        QFile initPlaceHolder(configPath + ".initialised");
        initPlaceHolder.open(QFile::WriteOnly);
        initPlaceHolder.close();
    } else {
        LoadConfig(&configFile);
    }

    return true;
}

int main(int argc, char *argv[])
{
    QApplication _qApp(argc, argv);
    RunGuard guard("Hv2ray-Instance-Identifier");

    if (!guard.isSingleInstance()) {
        Utils::showWarnMessageBox(nullptr, QObject::tr("Hv2Ray"), QObject::tr("AnotherInstanceRunning"));
        return -1;
    }

    // Set file startup path as Path
    // WARNING: This may be changed in the future.
    QDir::setCurrent(QFileInfo(QCoreApplication::applicationFilePath()).path());
    // Hv2ray Initialize
    initializeHv();

    if (_qApp.installTranslator(getTranslator(GetGlobalConfig().language))) {
        cout << "Loaded translations " << GetGlobalConfig().language << endl;
    } else if (_qApp.installTranslator(getTranslator("en-US"))) {
        cout << "Loaded default translations" << endl;
    } else {
        showWarnMessageBox(
            nullptr, "Failed to load translations 无法加载语言文件",
            "Failed to load translations, user experience may be downgraded. \r\n"
            "无法加载语言文件，用户体验可能会降级.");
    }

    // Show MainWindow
    Ui::MainWindow w;
    w.show();
    return _qApp.exec();
}
