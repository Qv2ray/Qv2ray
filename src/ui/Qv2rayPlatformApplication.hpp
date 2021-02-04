#pragma once

#include "base/Qv2rayBaseApplication.hpp"
#include "components/translations/QvTranslator.hpp"
#include "core/handler/ConfigHandler.hpp"
#include "core/handler/RouteHandler.hpp"
#include "core/settings/SettingsBackend.hpp"
#include "utils/QvHelpers.hpp"

#ifndef QV2RAY_NO_SINGLEAPPLICATON
#ifdef Q_OS_ANDROID
// No SingleApplication on Android platform
#define QV2RAY_NO_SINGLEAPPLICATON
#elif QV2RAY_WORKAROUND_MACOS_MEMLOCK
// No SingleApplication on macOS locking error
#define QV2RAY_NO_SINGLEAPPLICATON
#endif
#endif

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#ifdef QV2RAY_GUI
#include <QApplication>
#include <QMessageBox>
const static inline QMap<MessageOpt, QMessageBox::StandardButton> MessageBoxButtonMap //
    = { { No, QMessageBox::No },
        { OK, QMessageBox::Ok },
        { Yes, QMessageBox::Yes },
        { Cancel, QMessageBox::Cancel },
        { Ignore, QMessageBox::Ignore } };
#else
#include <QCoreApplication>
#endif

#ifndef QV2RAY_NO_SINGLEAPPLICATON
#include <SingleApplication>
#define QVBASEAPPLICATION SingleApplication
#define QVBASEAPPLICATION_CTORARGS argc, argv, true, User | ExcludeAppPath | ExcludeAppVersion
#else
#define QVBASEAPPLICATION QAPPLICATION_CLASS
#define QVBASEAPPLICATION_CTORARGS argc, argv
#endif

class Qv2rayPlatformApplication
    : public QVBASEAPPLICATION
    , public Qv2rayApplicationInterface
{
    Q_OBJECT
  public:
    Qv2rayPlatformApplication(int &argc, char *argv[]) : QVBASEAPPLICATION(QVBASEAPPLICATION_CTORARGS), Qv2rayApplicationInterface(){};
    virtual ~Qv2rayPlatformApplication(){};
    virtual Qv2rayExitReason GetExitReason() const final
    {
        return _exitReason;
    }

    virtual QStringList CheckPrerequisites() final;
    virtual bool Initialize() final;
    virtual Qv2rayExitReason RunQv2ray() final;

  protected:
    virtual QStringList checkPrerequisitesInternal() = 0;
    virtual Qv2rayExitReason runQv2rayInternal() = 0;
    virtual void terminateUIInternal() = 0;
    virtual void SetExitReason(Qv2rayExitReason r) final
    {
        _exitReason = r;
    }

#ifndef QV2RAY_NO_SINGLEAPPLICATON
    virtual void onMessageReceived(quint32 clientId, QByteArray msg) = 0;
#endif

  private:
    void quitInternal();
    Qv2rayExitReason _exitReason;
    bool parseCommandLine(QString *errorMessage, bool *canContinue);
};
