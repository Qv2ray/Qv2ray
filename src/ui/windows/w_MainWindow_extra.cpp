#include "Qv2rayApplication.hpp"
#include "Qv2rayBase/Common/Utils.hpp"
#include "Qv2rayBase/Profile/KernelManager.hpp"
#include "Qv2rayBase/Profile/ProfileManager.hpp"
#include "components/GuiPluginHost/GuiPluginHost.hpp"
#include "ui/WidgetUIBase.hpp"
#include "ui/windows/editors/w_JsonEditor.hpp"
#include "w_MainWindow.hpp"

#ifdef Q_OS_MAC
#include <ApplicationServices/ApplicationServices.h>
#endif

#include <QClipboard>

void MainWindow::MWToggleVisibility()
{
    if (isHidden() || isMinimized())
        MWShowWindow();
    else
        MWHideWindow();
}

void MainWindow::MWShowWindow()
{
#ifdef Q_OS_WIN
    setWindowState(Qt::WindowNoState);
#elif defined(Q_OS_MAC)
    ProcessSerialNumber psn = { 0, kCurrentProcess };
    TransformProcessType(&psn, kProcessTransformToForegroundApplication);
#endif
    QvApp->GetTrayManager()->SetMainWindowCurrentState(MainWindowState::State_Shown);
    this->show();
    this->raise();
    this->activateWindow();
}

void MainWindow::MWHideWindow()
{
    this->hide();
#ifdef Q_OS_MAC
    ProcessSerialNumber psn = { 0, kCurrentProcess };
    TransformProcessType(&psn, kProcessTransformToUIElementApplication);
#endif
    QvApp->GetTrayManager()->SetMainWindowCurrentState(MainWindowState::State_Hidden);
}

bool MainWindow::TryStartAutoConnectionEntry()
{
    if (QvApp->StartupArguments.noAutoConnection)
        return false;
    switch (GlobalConfig->behaviorConfig->AutoConnectBehavior)
    {
        case Qv2rayBehaviorConfig::AUTOCONNECT_NONE: return false;
        case Qv2rayBehaviorConfig::AUTOCONNECT_FIXED: return QvProfileManager->StartConnection(GlobalConfig->behaviorConfig->AutoConnectProfileId);
        case Qv2rayBehaviorConfig::AUTOCONNECT_LAST_CONNECTED: return QvProfileManager->StartConnection(GlobalConfig->behaviorConfig->LastConnectedId);
    }
    Q_UNREACHABLE();
}

void MainWindow::CheckForSubscriptionsUpdate()
{
    QList<std::pair<QString, GroupId>> updateList;
    QStringList updateNamesList;

    for (const auto &entry : QvProfileManager->GetGroups())
    {
        const auto info = QvProfileManager->GetGroupObject(entry);
        if (!info.subscription_config.isSubscription)
            continue;

        // The update is ignored.
        if (info.subscription_config.updateInterval == 0.0f)
            continue;

        if (info.updated + info.subscription_config.updateInterval * 24h <= system_clock::now())
        {
            updateList << std::pair{ info.name, entry };
            updateNamesList << info.name;
            qInfo() << "Subscription update:" << info.name << TimeToString(info.updated) << info.subscription_config.updateInterval;
        }
    }

    if (updateList.isEmpty())
        return;

    Qv2rayBase::MessageOpt result;
    if (GlobalConfig->behaviorConfig->QuietMode)
    {
        result = Qv2rayBase::MessageOpt::Yes;
    }
    else
    {
        const auto options = { Qv2rayBase::MessageOpt::Yes, Qv2rayBase::MessageOpt::No, Qv2rayBase::MessageOpt::Ignore };
        result = QvBaselib->Ask(tr("Update Subscriptions"), tr("Do you want to update these subscriptions?") + NEWLINE + updateNamesList.join('\n'), options);
    }

    for (const auto &[name, id] : updateList)
    {
        if (result == Qv2rayBase::MessageOpt::Yes)
        {
            qInfo() << "Updating subscription:" << name;
            QvProfileManager->UpdateSubscription(id, true);
        }
        else if (result == Qv2rayBase::MessageOpt::Ignore)
        {
            qInfo() << "Ignored subscription update:" << name;
            QvProfileManager->IgnoreSubscriptionUpdate(id);
        }
    }
}

void MainWindow::updateColorScheme()
{
    if (GlobalConfig->appearanceConfig->ShowTrayIcon)
        QvApp->GetTrayManager()->ShowTrayIcon();
    else
        QvApp->GetTrayManager()->HideTrayIcon();

    QvApp->GetTrayManager()->UpdateColorScheme();
    coreLogHighlighter->loadRules(StyleManager->isDarkMode());

    importConfigButton->setIcon(QIcon(STYLE_RESX("add")));
    updownImageBox->setStyleSheet("image: url(" + STYLE_RESX("netspeed_arrow") + ")");
    updownImageBox_2->setStyleSheet("image: url(" + STYLE_RESX("netspeed_arrow") + ")");

    connActions.Start->setIcon(QIcon(STYLE_RESX("start")));
    connActions.Edit->setIcon(QIcon(STYLE_RESX("edit")));
    connActions.editAsActions.Json->setIcon(QIcon(STYLE_RESX("code")));
    connActions.DuplicateConnection->setIcon(QIcon(STYLE_RESX("copy")));
    connActions.DeleteConnection->setIcon(QIcon(STYLE_RESX("ashbin")));
    connActions.TestLatency->setIcon(QIcon(STYLE_RESX("ping_gauge")));

    clearChartBtn->setIcon(QIcon(STYLE_RESX("ashbin")));
    clearlogButton->setIcon(QIcon(STYLE_RESX("ashbin")));

    locateBtn->setIcon(QIcon(STYLE_RESX("map")));
    sortBtn->setIcon(QIcon(STYLE_RESX("arrow-down-filling")));
    collapseGroupsBtn->setIcon(QIcon(STYLE_RESX("arrow-up")));
}

void MainWindow::RetranslateMenuActions()
{
    QvApp->GetTrayManager()->Retranslate();
    connActions.Start->setText(tr("Connect to this"));
    connActions.SetAutoConnection->setText(tr("Set as automatically connected"));
    connActions.EditAsMenu->setTitle(tr("Edit Using..."));
    connActions.editAsActions.Json->setText(tr("JSON Editor"));
    connActions.CopyMenu->setTitle(tr("Copy..."));
    connActions.copyActions.Link->setText(tr("Share Link"));
    connActions.UpdateSubscription->setText(tr("Update Subscription"));
    connActions.RenameConnection->setText(tr("Rename"));
    connActions.Edit->setText(tr("Edit"));
    connActions.DuplicateConnection->setText(tr("Duplicate to the Same Group"));
    connActions.TestLatency->setText(tr("Test Latency"));
    connActions.ResetStats->setText(tr("Clear Usage Data"));
    connActions.DeleteConnection->setText(tr("Delete Connection"));

    sortMenu->setTitle(tr("Sort connection list."));
    sortActions.SortByName_Asc->setText(tr("By connection name, A-Z"));
    sortActions.SortByName_Dsc->setText(tr("By connection name, Z-A"));
    sortActions.SortByPing_Asc->setText(tr("By latency, Ascending"));
    sortActions.SortByPing_Dsc->setText(tr("By latency, Descending"));
    sortActions.SortByData_Asc->setText(tr("By data, Ascending"));
    sortActions.SortByData_Dsc->setText(tr("By data, Descending"));

    graphAction_CopyGraph->setText(tr("Copy graph as image."));
    logAction_CopyRecentLogs->setText(tr("Copy latest logs."));
    logAction_CopySelected->setText(tr("Copy selected."));
}

void MainWindow::LoadPluginMainWindowWidgets()
{
    for (const auto &[metadata, guiInterface] : GUIPluginHost->QueryByGuiComponent(Qv2rayPlugin::GUI_COMPONENT_MAIN_WINDOW_ACTIONS))
    {
        auto mainWindowWidgetPtr = guiInterface->GetMainWindowWidget();
        if (!mainWindowWidgetPtr)
            continue;
        const auto index = pluginWidgets.count();
        {
            // Let Qt manage the ownership.
            auto widget = mainWindowWidgetPtr.release();
            pluginWidgets.append(widget);
        }
        auto btn = new QPushButton(metadata.Name, this);
        connect(btn, &QPushButton::clicked, this, &MainWindow::OnPluginButtonClicked);
        btn->setProperty(BUTTON_PROP_PLUGIN_MAINWIDGETITEM_INDEX, index);
        topButtonsLayout->addWidget(btn);
    }
}

// ================================================================== Connection Actions ==================================================================
#define CheckCurrentWidget                                                                                                                                               \
    auto widget = GetIndexWidget(connectionTreeView->currentIndex());                                                                                                    \
    if (widget == nullptr)                                                                                                                                               \
        return;

#define GetIndexWidget(item) (qobject_cast<ConnectionItemWidget *>(connectionTreeView->indexWidget(item)))

void MainWindow::Action_Start()
{
    CheckCurrentWidget;
    if (widget->IsConnection())
        widget->BeginConnection();
}

void MainWindow::Action_SetAutoConnection()
{
    const auto current = connectionTreeView->currentIndex();
    if (current.isValid())
    {
        const auto widget = GetIndexWidget(current);
        const auto identifier = widget->Profile();
        GlobalConfig->behaviorConfig->AutoConnectProfileId = identifier;
        GlobalConfig->behaviorConfig->AutoConnectBehavior = Qv2rayBehaviorConfig::AUTOCONNECT_FIXED;
        if (!GlobalConfig->behaviorConfig->QuietMode)
        {
            QvApp->GetTrayManager()->ShowTrayMessage(tr("%1 has been set to be auto connected.").arg(GetDisplayName(identifier.connectionId)));
        }
        QvApp->SaveQv2raySettings();
    }
}

void MainWindow::Action_UpdateSubscription()
{
    auto current = connectionTreeView->currentIndex();
    if (current.isValid())
    {
        auto widget = GetIndexWidget(current);
        if (widget)
        {
            if (widget->IsConnection())
                return;
            const auto gid = widget->Profile().groupId;
            if (QvProfileManager->GetGroupObject(gid).subscription_config.isSubscription)
                QvProfileManager->UpdateSubscription(gid, true);
            else
                QvBaselib->Info(tr("Update Subscription"), tr("Selected group is not a subscription"));
        }
    }
}

void MainWindow::Action_Edit()
{
    CheckCurrentWidget;
    OnEditRequested(widget->Profile().connectionId);
}

void MainWindow::Action_EditJson()
{
    CheckCurrentWidget;
    OnEditJsonRequested(widget->Profile().connectionId);
}

void MainWindow::Action_Copy_Link()
{
    QStringList links;
    for (const auto &current : connectionTreeView->selectionModel()->selectedIndexes())
    {
        if (!current.isValid())
            continue;
        const auto widget = GetIndexWidget(current);
        if (!widget)
            continue;
        if (widget->IsConnection())
        {
            if (const auto shareLink = ConvertConfigToString(widget->Profile().connectionId); shareLink)
                links << *shareLink;
        }
        else
        {
            const auto conns = QvProfileManager->GetConnections(widget->Profile().groupId);
            links.reserve(conns.size());
            for (const auto &connection : conns)
                if (const auto link = ConvertConfigToString(connection); link)
                    links.append(*link);
        }
    }
    links.removeDuplicates();
    qApp->clipboard()->setText(links.join('\n'));
}

void MainWindow::Action_RenameConnection()
{
    CheckCurrentWidget;
    widget->BeginRename();
}

void MainWindow::Action_DuplicateConnection()
{
    QList<ProfileId> connlist;
    connlist.reserve(connectionTreeView->selectionModel()->selectedIndexes().size());
    for (const auto &item : connectionTreeView->selectionModel()->selectedIndexes())
    {
        auto widget = GetIndexWidget(item);
        if (widget->IsConnection())
            connlist.append(widget->Profile());
    }

    qInfo() << "Selected" << connlist.count() << "items.";

    const auto strDupConnTitle = tr("Duplicating Connection(s)", "", connlist.count());
    const auto strDupConnContent = tr("Are you sure to duplicate these connection(s)?", "", connlist.count());

    if (connlist.count() > 1 && QvBaselib->Ask(strDupConnTitle, strDupConnContent) != Qv2rayBase::MessageOpt::Yes)
        return;

    for (const auto &conn : connlist)
    {
        const auto profile = QvProfileManager->GetConnection(conn.connectionId);
        QvProfileManager->CreateConnection(profile, GetDisplayName(conn.connectionId) + tr(" (Copy)"), conn.groupId);
    }
}

void MainWindow::Action_TestLatency()
{
    for (const auto &current : connectionTreeView->selectionModel()->selectedIndexes())
    {
        if (!current.isValid())
            continue;
        const auto widget = GetIndexWidget(current);
        if (!widget)
            continue;
        if (widget->IsConnection())
            QvProfileManager->StartLatencyTest(widget->Profile().connectionId, GlobalConfig->behaviorConfig->DefaultLatencyTestEngine);
        else
            QvProfileManager->StartLatencyTest(widget->Profile().groupId, GlobalConfig->behaviorConfig->DefaultLatencyTestEngine);
    }
}

void MainWindow::Action_ResetStats()
{
    auto current = connectionTreeView->currentIndex();
    if (current.isValid())
    {
        auto widget = GetIndexWidget(current);
        if (widget)
        {
            if (widget->IsConnection())
                QvProfileManager->ClearConnectionUsage(widget->Profile());
            else
                QvProfileManager->ClearGroupUsage(widget->Profile().groupId);
        }
    }
}

void MainWindow::Action_DeleteConnections()
{
    QList<ProfileId> connlist;
    QList<GroupId> groupsList;

    for (const auto &item : connectionTreeView->selectionModel()->selectedIndexes())
    {
        const auto widget = GetIndexWidget(item);
        if (!widget)
            continue;

        const auto identifier = widget->Profile();
        if (widget->IsConnection())
        {
            // Simply add the connection id
            connlist.append(identifier);
            continue;
        }

        for (const auto &conns : QvProfileManager->GetConnections(identifier.groupId))
            connlist.append(ProfileId{ conns, identifier.groupId });

        const auto message = tr("Do you want to remove groups as well?") + NEWLINE + tr("Group: ") + GetDisplayName(identifier.groupId);
        if (QvBaselib->Ask(tr("Removing Connection(s)"), message) == Qv2rayBase::MessageOpt::Yes)
            groupsList << identifier.groupId;
    }

    const auto strRemoveConnTitle = tr("Removing Connection(s)");
    const auto strRemoveConnContent = tr("Are you sure to remove selected connection(s)?");

    if (QvBaselib->Ask(strRemoveConnTitle, strRemoveConnContent) != Qv2rayBase::MessageOpt::Yes)
        return;

    for (const auto &conn : connlist)
        QvProfileManager->RemoveFromGroup(conn.connectionId, conn.groupId);

    for (const auto &group : groupsList)
        QvProfileManager->DeleteGroup(group, false);
}
