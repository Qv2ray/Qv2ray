#pragma once

#include "ui/Qv2rayPlatformApplication.hpp"

#include <QApplication>
#include <QDesktopServices>
#include <QMessageBox>
#include <QSystemTrayIcon>

class MainWindow;

namespace Qv2ray
{
    class Qv2rayWidgetApplication : public Qv2rayPlatformApplication
    {
        Q_OBJECT
      public:
        explicit Qv2rayWidgetApplication(int &argc, char *argv[]);
        Qv2raySetupStatus Initialize() override;
        Qv2rayExitCode RunQv2ray() override;

      public:
        void MessageBoxWarn(QWidget *parent, const QString &title, const QString &text, MessageOpt button = OK) override
        {
            QMessageBox::warning(parent, title, text, MessageBoxButtonMap[button]);
        }
        void MessageBoxInfo(QWidget *parent, const QString &title, const QString &text, MessageOpt button = OK) override
        {
            QMessageBox::information(parent, title, text, MessageBoxButtonMap[button]);
        }
        MessageOpt MessageBoxAsk(QWidget *parent, const QString &title, const QString &text, const QList<MessageOpt> &buttons) override
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
        void ShowTrayMessage(const QString &m, const QIcon &icon, int msecs = 10000)
        {
            hTray->showMessage("Qv2ray", m, icon, msecs);
        }
        void ShowTrayMessage(const QString &m, QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::Information, int msecs = 10000)
        {
            hTray->showMessage("Qv2ray", m, icon, msecs);
        }
        void OpenURL(const QString &url) override
        {
            QDesktopServices::openUrl(url);
        }

      private:
        void TerminateUI() override;
#ifndef QV2RAY_NO_SINGLEAPPLICATON
        void onMessageReceived(quint32 clientID, QByteArray msg) override;
#endif
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
