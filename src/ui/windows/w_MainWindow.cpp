#include "w_MainWindow.hpp"

#include "GuiPluginHost/GuiPluginHost.hpp"
#include "Qv2rayApplication.hpp"
#include "Qv2rayBase/Common/ProfileHelpers.hpp"
#include "Qv2rayBase/Common/Utils.hpp"
#include "Qv2rayBase/Profile/KernelManager.hpp"
#include "Qv2rayBase/Profile/ProfileManager.hpp"
#include "components/UpdateChecker/UpdateChecker.hpp"
#include "ui/WidgetUIBase.hpp"
#include "ui/widgets/ConnectionInfoWidget.hpp"
#include "ui/windows/editors/w_JsonEditor.hpp"
#include "ui/windows/editors/w_OutboundEditor.hpp"
#include "ui/windows/w_AboutWindow.hpp"
#include "ui/windows/w_GroupManager.hpp"
#include "ui/windows/w_ImportConfig.hpp"
#include "ui/windows/w_PluginManager.hpp"
#include "ui/windows/w_PreferencesWindow.hpp"

#include <QClipboard>
#include <QInputDialog>
#include <QScrollBar>
#include <QToolTip>

#define CheckCurrentWidget                                                                                                                                               \
    auto widget = GetIndexWidget(connectionTreeView->currentIndex());                                                                                                    \
    if (widget == nullptr)                                                                                                                                               \
        return;

#define GetIndexWidget(item) (qobject_cast<ConnectionItemWidget *>(connectionTreeView->indexWidget(item)))
#define NumericString(i) (QString("%1").arg(i, 30, 10, QLatin1Char('0')))

QvMessageBusSlotImpl(MainWindow)
{
    switch (msg)
    {
        MBUpdateColorSchemeDefaultImpl;
    }
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setupUi(this);
    this->setWindowIcon(QvApp->Qv2rayLogo);
    QvMessageBusConnect();

    connectionModelHelper = new ConnectionListHelper(connectionTreeView);

    {
        //
        // ConnectionInfo widget
        //
        connectionInfoWidget = new ConnectionInfoWidget(this);
        connectionInfoLayout->addWidget(connectionInfoWidget);

        const auto searchHandler = [this](QString tag)
        {
            if (tag.isEmpty())
                return;

            if (tag[0].isDigit() || tag.contains(' ') || tag.contains('"'))
            {
                tag.replace('"', uR"(\")"_qs);
                tag = '"' + tag + '"';
            }
            tag.prepend(u"> tags="_qs);
            connectionFilterTxt->setText(tag);
            on_connectionFilterTxt_textEdited(tag);
        };

        connect(connectionInfoWidget, &ConnectionInfoWidget::OnTagSearchRequested, this, searchHandler);
        connect(connectionInfoWidget, &ConnectionInfoWidget::OnEditRequested, this, &MainWindow::OnEditRequested);
        connect(connectionInfoWidget, &ConnectionInfoWidget::OnJsonEditRequested, this, &MainWindow::OnEditJsonRequested);
    }

    {
        //
        // Speed Chart
        //
        speedChartWidget = new SpeedWidget(this);
        speedChart->addWidget(speedChartWidget);
        graphAction_CopyGraph = new QAction(speedChartWidget);
        speedChartWidget->addAction(graphAction_CopyGraph);
        speedChartWidget->setContextMenuPolicy(Qt::ActionsContextMenu);
        connect(graphAction_CopyGraph, &QAction::triggered, this, &MainWindow::Action_CopyGraphAsImage);
    }

    {
        //
        // Splitter
        //
        constexpr auto sizeRatioA = 0.382;
        constexpr auto sizeRatioB = 1 - sizeRatioA;
        splitter->setSizes({ (int) (width() * sizeRatioA), (int) (width() * sizeRatioB) });
    }

    {
        //
        // Log Browser
        //
        coreLogHighlighter = new LogHighlighter::LogHighlighter(logBrowser->document());

        logAction_CopySelected = new QAction(logBrowser);
        logAction_CopyRecentLogs = new QAction(logBrowser);
        connect(logAction_CopyRecentLogs, &QAction::triggered, this, &MainWindow::Action_CopyRecentLogs);
        connect(logAction_CopySelected, &QAction::triggered, logBrowser, &QTextBrowser::copy);

        logBrowser->addActions({ logAction_CopySelected, logAction_CopyRecentLogs });
        connect(logBrowser->verticalScrollBar(), &QSlider::valueChanged, this, [this](int v) { logAutoScoll = logBrowser->verticalScrollBar()->maximum() == v; });

        auto font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
        font.setPointSize(9);
        logBrowser->setFont(font);
        logBrowser->document()->setDefaultFont(font);
    }

    {
        //
        // Actions for right click the connection list
        //
        connActions.Start = new QAction(connMenu);
        connActions.Edit = new QAction(connMenu);
        connActions.EditAsMenu = new QMenu(connMenu);
        connActions.editAsActions.Json = new QAction(connActions.EditAsMenu);
        connActions.CopyMenu = new QMenu(connMenu);
        connActions.copyActions.Link = new QAction(connActions.CopyMenu);
        connActions.TestLatency = new QAction(connMenu);
        connActions.SetAutoConnection = new QAction(connMenu);
        connActions.RenameConnection = new QAction(connMenu);
        connActions.DuplicateConnection = new QAction(connMenu);
        connActions.ResetStats = new QAction(connMenu);
        connActions.UpdateSubscription = new QAction(connMenu);
        connActions.DeleteConnection = new QAction(connMenu);

        connect(connActions.Start, &QAction::triggered, this, &MainWindow::Action_Start);
        connect(connActions.Edit, &QAction::triggered, this, &MainWindow::Action_Edit);
        connect(connActions.editAsActions.Json, &QAction::triggered, this, &MainWindow::Action_EditJson);
        connect(connActions.copyActions.Link, &QAction::triggered, this, &MainWindow::Action_Copy_Link);
        connect(connActions.TestLatency, &QAction::triggered, this, &MainWindow::Action_TestLatency);
        connect(connActions.SetAutoConnection, &QAction::triggered, this, &MainWindow::Action_SetAutoConnection);
        connect(connActions.RenameConnection, &QAction::triggered, this, &MainWindow::Action_RenameConnection);
        connect(connActions.DuplicateConnection, &QAction::triggered, this, &MainWindow::Action_DuplicateConnection);
        connect(connActions.ResetStats, &QAction::triggered, this, &MainWindow::Action_ResetStats);
        connect(connActions.UpdateSubscription, &QAction::triggered, this, &MainWindow::Action_UpdateSubscription);
        connect(connActions.DeleteConnection, &QAction::triggered, this, &MainWindow::Action_DeleteConnections);

        connMenu->addAction(connActions.Start);
        connMenu->addSeparator();
        connMenu->addAction(connActions.Edit);
        {
            connActions.EditAsMenu->addAction(connActions.editAsActions.Json);
        }
        connMenu->addMenu(connActions.EditAsMenu);
        connMenu->addSeparator();
        {
            connActions.CopyMenu->addAction(connActions.copyActions.Link);
        }
        connMenu->addMenu(connActions.CopyMenu);
        connMenu->addSeparator();
        connMenu->addAction(connActions.TestLatency);
        connMenu->addAction(connActions.SetAutoConnection);
        connMenu->addAction(connActions.RenameConnection);
        connMenu->addAction(connActions.DuplicateConnection);
        connMenu->addAction(connActions.ResetStats);
        connMenu->addSeparator();
        connMenu->addAction(connActions.UpdateSubscription);
        connMenu->addSeparator();

        connMenu->addAction(connActions.DeleteConnection);
    }

    {
        //
        // Sort Menu
        //
        sortActions.SortByName_Asc = new QAction(sortMenu);
        sortActions.SortByName_Dsc = new QAction(sortMenu);
        sortActions.SortByPing_Asc = new QAction(sortMenu);
        sortActions.SortByPing_Dsc = new QAction(sortMenu);
        sortActions.SortByData_Asc = new QAction(sortMenu);
        sortActions.SortByData_Dsc = new QAction(sortMenu);

        const auto SortConnectionList = [this](ConnectionInfoRole byCol, bool asending)
        {
            connectionModelHelper->Sort(byCol, asending ? Qt::AscendingOrder : Qt::DescendingOrder);
            on_locateBtn_clicked();
        };

        connect(sortActions.SortByName_Asc, &QAction::triggered, [=] { SortConnectionList(ROLE_DISPLAYNAME, true); });
        connect(sortActions.SortByName_Dsc, &QAction::triggered, [=] { SortConnectionList(ROLE_DISPLAYNAME, false); });
        connect(sortActions.SortByData_Asc, &QAction::triggered, [=] { SortConnectionList(ROLE_DATA_USAGE, true); });
        connect(sortActions.SortByData_Dsc, &QAction::triggered, [=] { SortConnectionList(ROLE_DATA_USAGE, false); });
        connect(sortActions.SortByPing_Asc, &QAction::triggered, [=] { SortConnectionList(ROLE_LATENCY, true); });
        connect(sortActions.SortByPing_Dsc, &QAction::triggered, [=] { SortConnectionList(ROLE_LATENCY, false); });
        sortMenu->addAction(sortActions.SortByName_Asc);
        sortMenu->addAction(sortActions.SortByName_Dsc);
        sortMenu->addSeparator();
        sortMenu->addAction(sortActions.SortByData_Asc);
        sortMenu->addAction(sortActions.SortByData_Dsc);
        sortMenu->addSeparator();
        sortMenu->addAction(sortActions.SortByPing_Asc);
        sortMenu->addAction(sortActions.SortByPing_Dsc);
        sortBtn->setMenu(sortMenu);
    }

    {
        //
        // Kernel and Profile event handlers
        //
        const auto crashHandler = [this](const ProfileId &, const QString &reason)
        {
            MWShowWindow();
            QvBaselib->Warn(tr("Kernel terminated."), tr("The kernel terminated unexpectedly:") + NEWLINE + reason + NEWLINE + NEWLINE +
                                                          tr("To solve the problem, read the kernel log in the log text browser."));
        };
        connect(QvKernelManager, &Qv2rayBase::Profile::KernelManager::OnCrashed, crashHandler);
        connect(QvKernelManager, &Qv2rayBase::Profile::KernelManager::OnConnected, this, &MainWindow::OnConnected);
        connect(QvKernelManager, &Qv2rayBase::Profile::KernelManager::OnDisconnected, this, &MainWindow::OnDisconnected);
        connect(QvKernelManager, &Qv2rayBase::Profile::KernelManager::OnStatsDataAvailable, this, &MainWindow::OnStatsAvailable);
        connect(QvKernelManager, &Qv2rayBase::Profile::KernelManager::OnKernelLogAvailable, this, &MainWindow::OnKernelLogAvailable);
        connect(QvProfileManager, &Qv2rayBase::Profile::ProfileManager::OnSubscriptionUpdateFinished,
                [](const GroupId &gid)
                {
                    if (!GlobalConfig->behaviorConfig->QuietMode)
                        QvApp->GetTrayManager()->ShowTrayMessage(tr("Subscription \"%1\" has been updated").arg(GetDisplayName(gid)));
                });
    }

    //
    // Find and start if there is an auto-connection
    const auto connectionStarted = TryStartAutoConnectionEntry();

    // Select the first connection.
    if (!connectionStarted && !QvProfileManager->GetConnections().isEmpty())
    {
        const auto groups = QvProfileManager->GetGroups();
        if (!groups.isEmpty())
        {
            const auto connections = QvProfileManager->GetConnections(groups.first());
            if (!connections.empty())
            {
                const auto index = connectionModelHelper->GetConnectionPairIndex({ connections.first(), groups.first() });
                connectionTreeView->setCurrentIndex(index);
                on_connectionTreeView_clicked(index);
            }
        }
    }

    CheckForSubscriptionsUpdate();
    LoadPluginMainWindowWidgets();

    updateColorScheme();
    RetranslateMenuActions();

    if ((connectionStarted || GlobalConfig->behaviorConfig->QuietMode) && GlobalConfig->appearanceConfig->ShowTrayIcon)
        MWHideWindow();
    else
        MWShowWindow();

    UpdateChecker::CheckUpdate();
}

void MainWindow::OnPluginButtonClicked()
{
    const auto senderWidget = qobject_cast<QPushButton *>(sender());
    if (!senderWidget)
        return;

    bool ok = false;
    const auto index = senderWidget->property(BUTTON_PROP_PLUGIN_MAINWIDGETITEM_INDEX).toInt(&ok);
    if (!ok)
        return;

    const auto widget = pluginWidgets.at(index);
    if (!widget)
        return;

    widget->setVisible(!widget->isVisible());
}

void MainWindow::ProcessCommand(const QString &command, QStringList commands, const QMap<QString, QString> &args)
{
    if (commands.isEmpty())
        return;
    if (command == u"open"_qs)
    {
        const auto subcommand = commands.takeFirst();
        QvDialog *w;
        if (subcommand == u"preference"_qs)
            w = new PreferencesWindow();
        else if (subcommand == u"plugin"_qs)
            w = new PluginManageWindow();
        else if (subcommand == u"group"_qs)
            w = new GroupManager();
        else if (subcommand == u"import"_qs)
            w = new ImportConfigWindow();
        else
            return;
        w->processCommands(command, commands, args);
        w->exec();
        delete w;
    }
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    if (focusWidget() == connectionTreeView)
    {
        CheckCurrentWidget;
        if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)
        {
            // If pressed enter or return on connectionListWidget. Try to connect to the selected connection.
            if (widget->IsConnection())
            {
                widget->BeginConnection();
            }
            else
            {
                connectionTreeView->expand(connectionTreeView->currentIndex());
            }
        }
        else if (e->key() == Qt::Key_F2)
        {
            widget->BeginRename();
        }
        else if (e->key() == Qt::Key_Delete)
        {
            Action_DeleteConnections();
        }
    }

    if (e->key() == Qt::Key_Escape)
    {
        auto widget = GetIndexWidget(connectionTreeView->currentIndex());
        // Check if this key was accpted by the ConnectionItemWidget
        if (widget && widget->IsRenaming())
        {
            widget->CancelRename();
            return;
        }
        else if (this->isActiveWindow())
        {
            this->close();
        }
    }
    // Ctrl + Q = Exit
    else if (e->modifiers() & Qt::ControlModifier && e->key() == Qt::Key_Q)
    {
        if (QvBaselib->Ask(tr("Quit Qv2ray"), tr("Are you sure to exit Qv2ray?")) == Qv2rayBase::MessageOpt::Yes)
            Action_Exit();
    }
    // Control + W = Close Window
    else if (e->modifiers() & Qt::ControlModifier && e->key() == Qt::Key_W)
    {
        if (this->isActiveWindow())
            this->close();
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *e)
{
    // Workaround of QtWidget not grabbing KeyDown and KeyUp in keyPressEvent
    if (e->key() == Qt::Key_Up || e->key() == Qt::Key_Down)
    {
        if (focusWidget() == connectionTreeView)
        {
            CheckCurrentWidget;
            on_connectionTreeView_clicked(connectionTreeView->currentIndex());
        }
    }
}

void MainWindow::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::WindowStateChange)
        QvApp->GetTrayManager()->SetMainWindowCurrentState((isHidden() || isMinimized()) ? MainWindowState::State_Hidden : MainWindowState::State_Shown);
}

MainWindow::~MainWindow()
{
    delete connectionModelHelper;
    for (auto &widget : pluginWidgets)
        widget->accept();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (GlobalConfig->appearanceConfig->ShowTrayIcon)
    {
        MWHideWindow();
        event->ignore();
    }
    else if (QvBaselib->Ask(tr("Quit Qv2ray"), tr("Are you sure to exit Qv2ray?")) == Qv2rayBase::MessageOpt::Yes)
        Action_Exit();
    else
        event->ignore();
}

void MainWindow::OnTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
#ifdef Q_OS_MACOS
    // special arrangement needed for macOS
    const auto toggleTriggerEvent = QSystemTrayIcon::DoubleClick;
#else
    const auto toggleTriggerEvent = QSystemTrayIcon::Trigger;
#endif
    if (reason == toggleTriggerEvent)
        MWToggleVisibility();
}

void MainWindow::on_clearlogButton_clicked()
{
    logBrowser->document()->clear();
}

void MainWindow::on_connectionTreeView_customContextMenuRequested(QPoint pos)
{
    Q_UNUSED(pos)

    const auto _pos = QCursor::pos();
    const auto item = connectionTreeView->indexAt(connectionTreeView->mapFromGlobal(_pos));
    if (item.isValid())
    {
        bool isConnection = GetIndexWidget(item)->IsConnection();
        // Disable connection-specific settings.
        connActions.Start->setEnabled(isConnection);
        connActions.SetAutoConnection->setEnabled(isConnection);
        connActions.Edit->setEnabled(isConnection);
        connActions.EditAsMenu->setEnabled(isConnection);
        connActions.RenameConnection->setEnabled(isConnection);
        connActions.DuplicateConnection->setEnabled(isConnection);
        connActions.UpdateSubscription->setEnabled(!isConnection);
        connMenu->popup(_pos);
    }
}

void MainWindow::on_importConfigButton_clicked()
{
    ImportConfigWindow w(this);
    const auto &[group, connections] = w.DoImportConnections();
    for (auto it = connections.keyValueBegin(); it != connections.constKeyValueEnd(); it++)
        QvProfileManager->CreateConnection(it->second, it->first, group);
}

void MainWindow::on_subsButton_clicked()
{
    GroupManager().exec();
}

void MainWindow::OnDisconnected(const ProfileId &id)
{
    Q_UNUSED(id)
    locateBtn->setEnabled(false);
    netspeedLabel->setText("0.00 B/s" NEWLINE "0.00 B/s");
    dataamountLabel->setText("0.00 B" NEWLINE "0.00 B");
    connetionStatusLabel->setText(tr("Not Connected"));
}

void MainWindow::OnConnected(const ProfileId &id)
{
    Q_UNUSED(id)
    locateBtn->setEnabled(true);
    on_clearlogButton_clicked();
    auto name = GetDisplayName(id.connectionId);
    if (!GlobalConfig->behaviorConfig->QuietMode)
    {
        QvApp->GetTrayManager()->ShowTrayMessage(tr("Connected: ") + name);
    }
    QvApp->GetTrayManager()->SetTrayTooltip("Qv2ray " QV2RAY_VERSION_STRING NEWLINE + tr("Connected: ") + name);
    connetionStatusLabel->setText(tr("Connected: ") + name);

    GlobalConfig->appearanceConfig->RecentConnections->removeAll(id);
    GlobalConfig->appearanceConfig->RecentConnections->push_front(id);

    on_locateBtn_clicked();
}

void MainWindow::on_connectionFilterTxt_textEdited(const QString &arg1)
{
    if (arg1.startsWith('>'))
    {
        try
        {
            auto command = arg1;
            command = command.remove(0, 1);
            const auto prog = QueryParser::ParseProgram(command);
            connectionModelHelper->Filter(prog);
            BLACK(connectionFilterTxt);
        }
        catch (std::runtime_error e)
        {
            RED(connectionFilterTxt);
            QToolTip::showText(connectionFilterTxt->mapToGlobal(connectionFilterTxt->pos()), e.what());
        }
    }
    else
    {
        connectionModelHelper->Filter(arg1);
    }
}

void MainWindow::OnStatsAvailable(const ProfileId &id, const StatisticsObject &data)
{
    if (!QvProfileManager->IsConnected(id))
        return;

    // This may not be, or may not precisely be, speed per second if the backend
    // has "any" latency. (Hope not...)
    QMap<SpeedWidget::GraphType, long> pointData;
    pointData[SpeedWidget::OUTBOUND_PROXY_UP] = data.proxyUp;
    pointData[SpeedWidget::OUTBOUND_PROXY_DOWN] = data.proxyDown;
    pointData[SpeedWidget::OUTBOUND_DIRECT_UP] = data.directUp;
    pointData[SpeedWidget::OUTBOUND_DIRECT_DOWN] = data.directDown;

    speedChartWidget->AddPointData(pointData);

    const auto &[totalUp, totalDown] = GetConnectionUsageAmount(id.connectionId, StatisticsObject::PROXY);
    auto totalDataUp = FormatBytes(totalUp);
    auto totalDataDown = FormatBytes(totalDown);
    auto totalSpeedUp = FormatBytes(data.proxyUp) + "/s";
    auto totalSpeedDown = FormatBytes(data.proxyDown) + "/s";

    netspeedLabel->setText(totalSpeedUp + NEWLINE + totalSpeedDown);
    dataamountLabel->setText(totalDataUp + NEWLINE + totalDataDown);

    const static auto format = u"Qv2ray %1\nConnected: %2\nUp: %3 Down: %4"_qs;
    QvApp->GetTrayManager()->SetTrayTooltip(format.arg(QV2RAY_VERSION_STRING, GetDisplayName(id.connectionId), totalSpeedUp, totalSpeedDown));
}

void MainWindow::OnKernelLogAvailable(const ProfileId &id, const QString &log)
{
    Q_UNUSED(id);
    FastAppendTextDocument(log.trimmed(), logBrowser->document());

    // From https://gist.github.com/jemyzhang/7130092
    const auto maxLines = GlobalConfig->appearanceConfig->MaximizeLogLines;
    auto block = logBrowser->document()->begin();

    while (block.isValid())
    {
        if (logBrowser->document()->blockCount() > maxLines)
        {
            QTextCursor cursor(block);
            block = block.next();
            cursor.select(QTextCursor::BlockUnderCursor);
            cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
            cursor.removeSelectedText();
            continue;
        }

        break;
    }
}

void MainWindow::OnEditRequested(const ConnectionId &id)
{
    const auto original = QvProfileManager->GetConnection(id);
    if (IsComplexConfig(id))
    {
        JsonEditor editor(original.toJson(), this);
        const auto root = ProfileContent::fromJson(editor.OpenEditor());
        if (editor.result() == QDialog::Accepted)
            QvProfileManager->UpdateConnection(id, root);
    }
    else
    {
        qInfo() << "INFO: Opening single connection edit window.";
        OutboundObject out;
        if (!original.outbounds.isEmpty())
            out = original.outbounds.constFirst();
        OutboundEditor editor(out, this);
        ProfileContent root{ editor.OpenEditor() };
        if (editor.result() == QDialog::Accepted)
            QvProfileManager->UpdateConnection(id, root);
    }
}

void MainWindow::OnEditJsonRequested(const ConnectionId &id)
{
    JsonEditor w(QvProfileManager->GetConnection(id).toJson(), this);
    const auto newRoot = ProfileContent::fromJson(w.OpenEditor());
    if (w.result() == QDialog::Accepted)
        QvProfileManager->UpdateConnection(id, newRoot);
}

void MainWindow::on_locateBtn_clicked()
{
    const auto id = QvKernelManager->CurrentConnection();
    if (id.isNull())
        return;
    const auto index = connectionModelHelper->GetConnectionPairIndex(id);
    connectionTreeView->setCurrentIndex(index);
    connectionTreeView->scrollTo(index);
    on_connectionTreeView_clicked(index);
}

void MainWindow::on_chartVisibilityBtn_clicked()
{
    speedChartHolderWidget->setVisible(!speedChartWidget->isVisible());
}

void MainWindow::on_logVisibilityBtn_clicked()
{
    logBrowser->setVisible(!logBrowser->isVisible());
}

void MainWindow::on_clearChartBtn_clicked()
{
    speedChartWidget->Clear();
}

void MainWindow::on_logBrowser_textChanged()
{
    if (!logAutoScoll)
        return;
    auto bar = logBrowser->verticalScrollBar();
    bar->setValue(bar->maximum());
}

void MainWindow::on_pluginsBtn_clicked()
{
    PluginManageWindow(this).exec();
}

void MainWindow::on_newConnectionBtn_clicked()
{
    OutboundEditor w(this);
    const ProfileContent root{ w.OpenEditor() };
    if (w.result() == QDialog::Accepted)
    {
        const auto alias = w.GetFriendlyName();
        const auto item = connectionTreeView->currentIndex();
        const auto id = item.isValid() ? GetIndexWidget(item)->Profile().groupId : DefaultGroupId;
        QvProfileManager->CreateConnection(root, alias, id);
    }
}

void MainWindow::on_newComplexConnectionBtn_clicked()
{
    JsonEditor w({}, this);
    const auto root = ProfileContent::fromJson(w.OpenEditor());
    if (w.result() == QDialog::Accepted)
    {
        const auto item = connectionTreeView->currentIndex();
        const auto id = item.isValid() ? GetIndexWidget(item)->Profile().groupId : DefaultGroupId;
        QvProfileManager->CreateConnection(root, u"New Connection"_qs, id);
    }
}

void MainWindow::on_collapseGroupsBtn_clicked()
{
    connectionTreeView->collapseAll();
}

void MainWindow::on_connectionTreeView_doubleClicked(const QModelIndex &index)
{
    const auto widget = GetIndexWidget(index);
    if (widget == nullptr)
        return;
    if (widget->IsConnection())
        widget->BeginConnection();
}

void MainWindow::on_connectionTreeView_clicked(const QModelIndex &index)
{
    const auto widget = GetIndexWidget(index);
    if (widget == nullptr)
        return;
    connectionInfoWidget->ShowDetails(widget->Profile());
}

void MainWindow::on_preferencesBtn_clicked()
{
    PreferencesWindow(this).exec();
}

void MainWindow::on_aboutBtn_clicked()
{
    AboutWindow(this).exec();
}

// ================================================================== General Actions ==================================================================

void MainWindow::Action_Exit()
{
    QvApp->quit();
}

void MainWindow::Action_CopyGraphAsImage()
{
    const auto image = speedChartWidget->grab();
    qApp->clipboard()->setImage(image.toImage());
}

void MainWindow::Action_CopyRecentLogs()
{
    const auto lines = SplitLines(logBrowser->document()->toPlainText());
    bool accepted = false;
    const auto line = QInputDialog::getInt(this, tr("Copy latest logs"), tr("Number of lines of logs to copy"), 20, 0, 2500, 1, &accepted);
    if (!accepted)
        return;
    const auto totalLinesCount = lines.count();
    const auto linesToCopy = std::min((int) totalLinesCount, line);
    QStringList result;
    result.reserve(linesToCopy);
    for (auto i = totalLinesCount - linesToCopy; i < totalLinesCount; i++)
        result.append(lines[i]);
    qApp->clipboard()->setText(result.join('\n'));
}
