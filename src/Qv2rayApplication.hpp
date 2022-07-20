#pragma once

#include "Qv2rayBase/Qv2rayBaseLibrary.hpp"
#include "QvPlugin/Utils/JsonConversion.hpp"
#include "models/SettingsModels.hpp"
#include "ui/TrayManager.hpp"

#include <QApplication>
#include <QMenu>
#include <QMessageBox>
#include <SingleApplication>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

class QSystemTrayIcon;
class MainWindow;

// clang-format off
namespace Qv2ray { namespace components { }; }
using namespace Qv2ray::components;
// clang-format on

struct Qv2rayStartupArguments
{
    enum Argument
    {
        NORMAL = 0,
        QV2RAY_LINK = 1,
        EXIT = 2,
        RECONNECT = 3,
        DISCONNECT = 4
    };
    QList<Argument> arguments;
    QString version;
    QString data;
    QList<QString> links;
    QList<QString> fullArgs;
    //
    bool noAPI;
    bool noAutoConnection;
    bool debugLog;
    bool noPlugins;
    bool exitQv2ray;
    //
    QString _qvNewVersionPath;
    QJS_JSON(F(arguments, data, version, links, fullArgs))
};

const static inline QMap<Qv2rayBase::MessageOpt, QMessageBox::StandardButton> MessageBoxButtonMap = {
    { Qv2rayBase::MessageOpt::No, QMessageBox::No },         //
    { Qv2rayBase::MessageOpt::OK, QMessageBox::Ok },         //
    { Qv2rayBase::MessageOpt::Yes, QMessageBox::Yes },       //
    { Qv2rayBase::MessageOpt::Cancel, QMessageBox::Cancel }, //
    { Qv2rayBase::MessageOpt::Ignore, QMessageBox::Ignore }, //
};

enum Qv2rayExitReason
{
    EXIT_NORMAL = 0,
    EXIT_NEW_VERSION_TRIGGER = EXIT_NORMAL + 1,
    EXIT_SECONDARY_INSTANCE = EXIT_NORMAL + 2,
    EXIT_INITIALIZATION_FAILED = -1,
    EXIT_PRECONDITION_FAILED = -2,
};

class Qv2rayApplication
    : public SingleApplication
    , public Qv2rayBase::Interfaces::IUserInteractionInterface
{
  public:
    Qv2rayStartupArguments StartupArguments;

  public:
    Qv2rayApplication(int &argc, char *argv[]);
    virtual ~Qv2rayApplication();

    QPixmap Qv2rayLogo;

    Qv2rayExitReason GetExitReason() const;
    bool Initialize();
    Qv2rayExitReason RunQv2ray();
    void SaveQv2raySettings();

    MainWindow *GetMainWindow() const;
    Qv2ray::ui::TrayManager *GetTrayManager() const;

  public:
    virtual void p_MessageBoxWarn(const QString &title, const QString &text) override;
    virtual void p_MessageBoxInfo(const QString &title, const QString &text) override;
    virtual Qv2rayBase::MessageOpt p_MessageBoxAsk(const QString &title, const QString &text, const QList<Qv2rayBase::MessageOpt> &options) override;
    virtual void p_OpenURL(const QUrl &url) override;

    virtual bool eventFilter(QObject *watched, QEvent *event) override;

  private:
    void quitInternal();
    bool parseCommandLine(QString *errorMessage, bool *canContinue);
    void onMessageReceived(quint32 clientId, const QByteArray &msg);
    Qv2rayExitReason exitReason = EXIT_NORMAL;
    MainWindow *mainWindow;
    Qv2ray::ui::TrayManager *trayManager;
    Qv2rayBase::Qv2rayBaseLibrary *baseLibrary;
};

inline Qv2ray::Models::Qv2rayApplicationConfigObject *GlobalConfig;

#define QvApp static_cast<Qv2rayApplication *>(qApp)
