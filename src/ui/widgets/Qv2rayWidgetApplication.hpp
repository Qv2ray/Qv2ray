#pragma once

#include "base/Qv2rayBaseApplication.hpp"

#include <QApplication>
#include <QDesktopServices>
#include <QMessageBox>
#include <QSystemTrayIcon>

#ifdef Q_OS_ANDROID
    #define QV2RAY_NO_SINGLEAPPLICATON
#endif

#define QV2RAY_WORKAROUND_MACOS_MEMLOCK 0

#if QV2RAY_WORKAROUND_MACOS_MEMLOCK
    #ifndef QV2RAY_NO_SINGLEAPPLICATION
        #define QV2RAY_NO_SINGLEAPPLICATON
    #endif
#endif

#ifndef QV2RAY_NO_SINGLEAPPLICATON
    #include <SingleApplication>
#endif

class MainWindow;

namespace Qv2ray
{
    const static inline QMap<MessageOptions, QMessageBox::StandardButton> MessageBoxButtonMap = //
        { { No, QMessageBox::No },
          { OK, QMessageBox::Ok },
          { Yes, QMessageBox::Yes },
          { Cancel, QMessageBox::Cancel },
          { Ignore, QMessageBox::Ignore } };

    class Qv2rayWidgetApplication
#ifdef QV2RAY_NO_SINGLEAPPLICATION
        : public QApplication
#else
        : public SingleApplication
#endif
        , public Qv2rayApplicationManagerInterface
    {
        Q_OBJECT

      public:
        //
        void QuitApplication(int retCode = 0);
        explicit Qv2rayWidgetApplication(int &argc, char *argv[]);
        bool FindAndCreateInitialConfiguration() override;
        Qv2raySetupStatus Initialize() override;
        Qv2rayExitCode RunQv2ray() override;

      public:
        void MessageBoxWarn(QWidget *parent, const QString &title, const QString &text, MessageOptions button = OK) override
        {
            QMessageBox::warning(parent, title, text, MessageBoxButtonMap[button]);
        }
        void MessageBoxInfo(QWidget *parent, const QString &title, const QString &text, MessageOptions button = OK) override
        {
            QMessageBox::information(parent, title, text, MessageBoxButtonMap[button]);
        }
        MessageOptions MessageBoxAsk(QWidget *parent, const QString &title, const QString &text, const QList<MessageOptions> &buttons) override
        {
            QFlags<QMessageBox::StandardButton> btns;
            for (const auto &b : buttons)
            {
                btns.setFlag(MessageBoxButtonMap[b]);
            }
            return MessageBoxButtonMap.key(QMessageBox::question(parent, title, text, btns));
        }
        QSystemTrayIcon **GetTrayIcon()
        {
            return &hTray;
        }
        void showMessage(const QString &m, const QIcon &icon, int msecs = 10000)
        {
            hTray->showMessage("Qv2ray", m, icon, msecs);
        }
        void showMessage(const QString &m, QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::Information, int msecs = 10000)
        {
            hTray->showMessage("Qv2ray", m, icon, msecs);
        }
        void OpenURL(const QString &url) override
        {
            QDesktopServices::openUrl(url);
        }
      private slots:
        void aboutToQuitSlot();

      private:
        static Qv2rayPreInitResult ParseCommandLine(QString *errorMessage, const QStringList &args);

      private:
#ifndef QV2RAY_NO_SINGLEAPPLICATON
        void onMessageReceived(quint32 clientID, QByteArray msg);
#endif
        bool initialized = false;
        bool LoadConfiguration();
        QSystemTrayIcon *hTray;
        MainWindow *mainWindow;
    };
} // namespace Qv2ray

#ifdef Qv2rayApplication
    #undef Qv2rayApplication
#endif
#define Qv2rayApplication Qv2rayWidgetApplication

#define QvWidgetApplication static_cast<Qv2rayWidgetApplication *>(qApp)
#define qvAppTrayIcon (*(QvWidgetApplication->GetTrayIcon()))

using namespace Qv2ray;
