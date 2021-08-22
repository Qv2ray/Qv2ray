#include "TrayManager.hpp"

#include "Qv2rayApplication.hpp"
#include "Qv2rayBase/Common/ProfileHelpers.hpp"
#include "Qv2rayBase/Profile/KernelManager.hpp"
#include "Qv2rayBase/Profile/ProfileManager.hpp"
#include "ui/windows/w_PreferencesWindow.hpp"

#include <QMenu>

using namespace Qv2ray::ui;

TrayManager::TrayManager(QObject *parent) : QObject(parent)
{
    hTray = new QSystemTrayIcon();
    SetTrayTooltip("Qv2ray " QV2RAY_VERSION_STRING);

    connect(QvKernelManager, &Qv2rayBase::Profile::KernelManager::OnConnected, this, &TrayManager::OnConnected);
    connect(QvKernelManager, &Qv2rayBase::Profile::KernelManager::OnDisconnected, this, &TrayManager::OnDisconnected);

    {
        tray_RootMenu = new QMenu;
        tray_action_ToggleVisibility = new QAction(tray_RootMenu);
        tray_action_Preferences = new QAction(tray_RootMenu);
        tray_action_Quit = new QAction(tray_RootMenu);
        tray_action_Start = new QAction(tray_RootMenu);
        tray_action_Restart = new QAction(tray_RootMenu);
        tray_action_Stop = new QAction(tray_RootMenu);

        // Basic tray actions
        tray_action_Start->setEnabled(true);
        tray_action_Stop->setEnabled(false);
        tray_action_Restart->setEnabled(false);

        tray_RootMenu->addAction(tray_action_ToggleVisibility);
        tray_RootMenu->addSeparator();
        tray_RootMenu->addAction(tray_action_Preferences);

        tray_RootMenu->addSeparator();

        {
            tray_RecentConnectionsMenu = new QMenu;
            tray_ClearRecentConnectionsAction = new QAction(this);
            tray_RootMenu->addMenu(tray_RecentConnectionsMenu);
        }

        tray_RootMenu->addSeparator();
        tray_RootMenu->addAction(tray_action_Start);
        tray_RootMenu->addAction(tray_action_Stop);
        tray_RootMenu->addAction(tray_action_Restart);
        tray_RootMenu->addSeparator();
        tray_RootMenu->addAction(tray_action_Quit);
        hTray->setContextMenu(tray_RootMenu);
    }

    connect(tray_RecentConnectionsMenu, &QMenu::aboutToShow, this,
            [this]()
            {
                tray_RecentConnectionsMenu->clear();
                tray_RecentConnectionsMenu->addAction(tray_ClearRecentConnectionsAction);
                tray_RecentConnectionsMenu->addSeparator();
                for (const auto &conn : *GlobalConfig->appearanceConfig->RecentConnections)
                {
                    if (!QvProfileManager->IsValidId(conn))
                        continue;
                    const auto name = GetDisplayName(conn.connectionId) + " (" + GetDisplayName(conn.groupId) + ")";
                    tray_RecentConnectionsMenu->addAction(name, this, [=]() { emit QvProfileManager->StartConnection(conn); });
                }
            });
    connect(tray_ClearRecentConnectionsAction, &QAction::triggered,
            [this]()
            {
                GlobalConfig->appearanceConfig->RecentConnections->clear();
                ReloadRecentConnectionList();
                if (!GlobalConfig->behaviorConfig->QuietMode)
                    ShowTrayMessage(tr("Recent Connection list cleared."));
            });

    connect(hTray, &QSystemTrayIcon::activated, this, &TrayManager::TrayActivated);
    connect(tray_action_ToggleVisibility, &QAction::triggered, this, &TrayManager::VisibilityToggled);
    connect(tray_action_Preferences, &QAction::triggered, this, []() { PreferencesWindow().exec(); });
    connect(tray_action_Start, &QAction::triggered, [] { QvProfileManager->StartConnection(GlobalConfig->behaviorConfig->LastConnectedId); });
    connect(tray_action_Stop, &QAction::triggered, QvProfileManager, &Qv2rayBase::Profile::ProfileManager::StopConnection);
    connect(tray_action_Restart, &QAction::triggered, QvProfileManager, &Qv2rayBase::Profile::ProfileManager::RestartConnection);
    connect(tray_action_Quit, &QAction::triggered, this, &Qv2rayApplication::exit, Qt::QueuedConnection);
}

TrayManager::~TrayManager()
{
    hTray->hide();
    delete hTray;
    delete tray_RootMenu;
    delete tray_RecentConnectionsMenu;
}

void TrayManager::HideTrayIcon()
{
    hTray->hide();
}

void TrayManager::ReloadRecentConnectionList()
{
    QList<ProfileId> newRecentConnections;
    const auto iterateRange = std::min(*GlobalConfig->appearanceConfig->RecentJumpListSize, GlobalConfig->appearanceConfig->RecentConnections->count());
    for (auto i = 0; i < iterateRange; i++)
    {
        const auto &item = GlobalConfig->appearanceConfig->RecentConnections->at(i);
        if (newRecentConnections.contains(item) || item.isNull())
            continue;
        newRecentConnections << item;
    }
    GlobalConfig->appearanceConfig->RecentConnections = newRecentConnections;
}

void TrayManager::OnConnected(const ProfileId &id)
{
    SetTrayIcon(Q_TRAYICON("tray-connected"));
    tray_action_Start->setEnabled(false);
    tray_action_Stop->setEnabled(true);
    tray_action_Restart->setEnabled(true);
    GlobalConfig->behaviorConfig->LastConnectedId = id;
    ReloadRecentConnectionList();
}

void TrayManager::OnDisconnected(const ProfileId &id)
{
    SetTrayIcon(Q_TRAYICON("tray"));
    tray_action_Start->setEnabled(true);
    tray_action_Stop->setEnabled(false);
    tray_action_Restart->setEnabled(false);
    GlobalConfig->behaviorConfig->LastConnectedId = id;
    if (!GlobalConfig->behaviorConfig->QuietMode)
    {
        ShowTrayMessage(tr("Disconnected from: ") + GetDisplayName(id.connectionId));
    }
    SetTrayTooltip("Qv2ray " QV2RAY_VERSION_STRING);
}

void TrayManager::UpdateColorScheme()
{
    SetTrayIcon(QvKernelManager->CurrentConnection().isNull() ? Q_TRAYICON("tray") : Q_TRAYICON("tray-connected"));
    tray_action_ToggleVisibility->setIcon(QvApp->Qv2rayLogo);
}

void TrayManager::Retranslate()
{
    tray_RecentConnectionsMenu->setTitle(tr("Recent Connections"));
    tray_ClearRecentConnectionsAction->setText(tr("Clear Recent Connections"));
    //
    tray_action_ToggleVisibility->setText(tr("Hide"));
    tray_action_Preferences->setText(tr("Preferences"));
    tray_action_Quit->setText(tr("Quit"));
    tray_action_Start->setText(tr("Connect"));
    tray_action_Restart->setText(tr("Reconnect"));
    tray_action_Stop->setText(tr("Disconnect"));
}

void TrayManager::ShowTrayIcon()
{
    hTray->show();
}

void TrayManager::SetTrayIcon(const QIcon &icon)
{
    hTray->setIcon(icon);
}

void TrayManager::SetTrayTooltip(const QString &tooltip)
{
    hTray->setToolTip(tooltip);
}

void TrayManager::ShowTrayMessage(const QString &m, int msecs)
{
    hTray->showMessage(u"Qv2ray"_qs, m, QvApp->Qv2rayLogo, msecs);
}

void TrayManager::SetMainWindowCurrentState(MainWindowState s)
{
    switch (s)
    {
        case Qv2ray::ui::MainWindowState::State_Shown: tray_action_ToggleVisibility->setText(tr("Hide")); break;
        case Qv2ray::ui::MainWindowState::State_Hidden: tray_action_ToggleVisibility->setText(tr("Show")); break;
    }
}
