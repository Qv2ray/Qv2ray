#pragma once

#include "QvPlugin/Common/CommonTypes.hpp"

#include <QAction>
#include <QObject>
#include <QSystemTrayIcon>

namespace Qv2ray::ui
{
    enum class MainWindowState
    {
        State_Shown,
        State_Hidden
    };

    class TrayManager : public QObject
    {
        Q_OBJECT

      public:
        TrayManager(QObject *parent = nullptr);
        ~TrayManager();

        void HideTrayIcon();
        void ShowTrayIcon();
        void SetTrayIcon(const QIcon &icon);
        void SetTrayTooltip(const QString &tooltip);
        void ShowTrayMessage(const QString &m, int msecs = 10000);
        void SetMainWindowCurrentState(MainWindowState s);
        void UpdateColorScheme();
        void Retranslate();

      signals:
        void TrayActivated(QSystemTrayIcon::ActivationReason);
        void VisibilityToggled();

      private:
        void ReloadRecentConnectionList();

        // Slots
        void OnConnected(const ProfileId &);
        void OnDisconnected(const ProfileId &);

      private:
        QSystemTrayIcon *hTray;
        QMenu *tray_RootMenu;
        QAction *tray_action_ToggleVisibility;
        QAction *tray_action_Preferences;
        QAction *tray_action_Quit;
        QAction *tray_action_Start;
        QAction *tray_action_Restart;
        QAction *tray_action_Stop;

        QMenu *tray_RecentConnectionsMenu;
        QAction *tray_ClearRecentConnectionsAction;
    };
} // namespace Qv2ray::ui
