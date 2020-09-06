#pragma once

#include "base/Qv2rayBaseApplication.hpp"
#include "components/translations/QvTranslator.hpp"
#include "core/settings/SettingsBackend.hpp"
#include "utils/QvHelpers.hpp"

#ifdef Q_OS_ANDROID
    // No SingleApplication on Android platform
    #define QV2RAY_NO_SINGLEAPPLICATON
#elif QV2RAY_WORKAROUND_MACOS_MEMLOCK
    // No SingleApplication on macOS locking error
    #define QV2RAY_NO_SINGLEAPPLICATON
#endif

#ifdef Q_OS_WIN
    #include <winbase.h>
#endif

#ifdef QV2RAY_GUI
    #include <QApplication>
    #include <QFont>
    #include <QMessageBox>
const static inline QMap<MessageOpt, QMessageBox::StandardButton> MessageBoxButtonMap = //
    { { No, QMessageBox::No },
      { OK, QMessageBox::Ok },
      { Yes, QMessageBox::Yes },
      { Cancel, QMessageBox::Cancel },
      { Ignore, QMessageBox::Ignore } };
#else
    #include <QCoreApplication>
#endif

#ifndef QV2RAY_NO_SINGLEAPPLICATON
    #include <SingleApplication>
    #define QV2RAY_BASE_APPLICATION_CLASS SingleApplication
    #define QV2RAY_BASE_CLASS_CONSTRUCTOR_ARGS argc, argv, true, User | ExcludeAppPath | ExcludeAppVersion
#else
    #define QV2RAY_BASE_APPLICATION_CLASS QApplication
    #define QV2RAY_BASE_CLASS_CONSTRUCTOR_ARGS argc, argv
#endif

class Qv2rayPlatformApplication
    : public QV2RAY_BASE_APPLICATION_CLASS
    , public Qv2rayApplicationManager
{
    Q_OBJECT
  public:
    Qv2rayPlatformApplication(int &argc, char *argv[])
        : QV2RAY_BASE_APPLICATION_CLASS(QV2RAY_BASE_CLASS_CONSTRUCTOR_ARGS), Qv2rayApplicationManager(argc, argv){};

    void QuitApplication(int retCode = 0)
    {
        isExiting = true;
        QCoreApplication::exit(retCode);
    }

  protected:
    Qv2raySetupStatus InitializeInternal()
    {
        connect(this, &Qv2rayPlatformApplication::aboutToQuit, this, &Qv2rayPlatformApplication::QuitInternal);
#ifndef QV2RAY_NO_SINGLEAPPLICATON
        connect(this, &SingleApplication::receivedMessage, this, &Qv2rayPlatformApplication::onMessageReceived, Qt::QueuedConnection);
        if (isSecondary())
        {
            if (Qv2rayProcessArgument.arguments.isEmpty())
                Qv2rayProcessArgument.arguments << Qv2rayProcessArguments::NORMAL;
            sendMessage(JsonToString(Qv2rayProcessArgument.toJson(), QJsonDocument::Compact).toUtf8());
            return SINGLE_APPLICATION;
        }
#endif
#ifdef Q_OS_LINUX
        setFallbackSessionManagementEnabled(false);
        connect(this, &QGuiApplication::commitDataRequest, [] {
            RouteManager->SaveRoutes();
            ConnectionManager->SaveConnectionConfig();
            PluginHost->SavePluginSettings();
            SaveGlobalSettings();
            LOG(MODULE_INIT, "Saving settings triggered by session manager.")
        });
#endif
#ifdef Q_OS_WIN
        SetCurrentDirectory(applicationDirPath().toStdWString().c_str());
        // Set special font in Windows
        QFont font;
        font.setPointSize(9);
        font.setFamily("Microsoft YaHei");
        setFont(font);
#endif
        // Install a default translater. From the OS/DE
        Qv2rayTranslator = std::make_unique<QvTranslator>();
        Qv2rayTranslator->InstallTranslation(QLocale::system().name());
        return NORMAL;
    }
    bool RunInternal()
    {
        PluginHost = new QvPluginHost();
        RouteManager = new RouteHandler();
        ConnectionManager = new QvConfigHandler();
        return true;
    }
    void QuitInternal()
    {
        LOG(MODULE_INIT, "Terminating connections and saving data.")
        // Do not change the order.
        ConnectionManager->StopConnection();
        RouteManager->SaveRoutes();
        ConnectionManager->SaveConnectionConfig();
        PluginHost->SavePluginSettings();
        SaveGlobalSettings();
        TerminateUI();
        delete ConnectionManager;
        delete RouteManager;
        delete PluginHost;
        ConnectionManager = nullptr;
        RouteManager = nullptr;
        PluginHost = nullptr;
    }

    virtual void TerminateUI() = 0;
#ifndef QV2RAY_NO_SINGLEAPPLICATON
    virtual void onMessageReceived(quint32 clientId, QByteArray msg) = 0;
#endif
};
