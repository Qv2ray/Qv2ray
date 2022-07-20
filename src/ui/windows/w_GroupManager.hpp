#pragma once

#include "QvPlugin/Common/CommonTypes.hpp"
#include "MessageBus/MessageBus.hpp"
#include "ui/WidgetUIBase.hpp"
#include "ui_w_GroupManager.h"

#include <QMenu>

class DnsSettingsWidget;
class RouteSettingsMatrixWidget;
class ConfigurableEditor;

class GroupManager
    : public QvDialog
    , private Ui::w_GroupManager
{
    Q_OBJECT

  public:
    explicit GroupManager(QWidget *parent = nullptr);
    ~GroupManager();
    void processCommands(QString command, QStringList commands, QMap<QString, QString>) override
    {
        const static QMap<QString, int> indexMap{ { "connection", 0 },   //
                                                  { "subscription", 1 }, //
                                                  { "dns", 2 },          //
                                                  { "route", 3 } };
        if (commands.isEmpty())
            return;
        if (command == "open")
        {
            const auto c = commands.takeFirst();
            tabWidget->setCurrentIndex(indexMap[c]);
        }
    }

  private:
    QvMessageBusSlotDecl override;
  private slots:
    void on_addGroupButton_clicked();
    void on_updateButton_clicked();
    void on_removeGroupButton_clicked();
    void on_buttonBox_accepted();
    void on_groupList_itemSelectionChanged();
    void on_groupList_itemClicked(QListWidgetItem *item);
    void on_groupList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void on_connectionsList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void on_groupIsSubscriptionGroup_clicked(bool checked);
    void on_groupNameTxt_textEdited(const QString &arg1);
    void onRCMDeleteConnectionTriggered();
    void on_deleteSelectedConnBtn_clicked();
    void on_connectionsTable_customContextMenuRequested(const QPoint &pos);
    void on_subscriptionTypeCB_currentIndexChanged(int arg1);

  private:
    virtual void updateColorScheme() override;
    void reloadConnectionsList(const GroupId &group);
    void onRCMActionTriggered_Move();
    void onRCMActionTriggered_Copy();
    void onRCMActionTriggered_Link();
    void reloadGroupRCMActions();
    void SaveCurrentGroup();
    //
    void setupSubscriptionProviderSettingsWidget();
    //
    ConfigurableEditor *subscriptionProviderOptionsEditor = nullptr;
    DnsSettingsWidget *dnsSettingsWidget = nullptr;
    RouteSettingsMatrixWidget *routeSettingsWidget = nullptr;
    //
    QMenu *connectionListRCMenu = new QMenu(this);
    QAction *deleteConnectionAction = new QAction(tr("Delete Connection(s)"), connectionListRCMenu);
    QMenu *connectionListRCMenu_CopyToMenu = new QMenu(tr("Copy to..."));
    QMenu *connectionListRCMenu_MoveToMenu = new QMenu(tr("Move to..."));
    QMenu *connectionListRCMenu_LinkToMenu = new QMenu(tr("Link to..."));
    bool isUpdateInProgress = false;

    GroupId currentGroupId;
    ConnectionId currentConnectionId;
};
