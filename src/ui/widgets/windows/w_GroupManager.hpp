#pragma once

#include "base/Qv2rayBase.hpp"
#include "ui/common/QvMessageBus.hpp"
#include "ui/widgets/common/WidgetUIBase.hpp"
#include "ui_w_GroupManager.h"

#include <QMenu>

class DnsSettingsWidget;
class RouteSettingsMatrixWidget;

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
    void on_IncludeRelation_currentTextChanged(const QString &arg1);
    void on_ExcludeRelation_currentTextChanged(const QString &arg1);
    void on_IncludeKeywords_textChanged();
    void on_ExcludeKeywords_textChanged();
    void on_groupList_itemClicked(QListWidgetItem *item);
    void on_groupList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void on_subAddrTxt_textEdited(const QString &arg1);
    void on_updateIntervalSB_valueChanged(double arg1);
    void on_connectionsList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void on_groupIsSubscriptionGroup_clicked(bool checked);
    void on_groupNameTxt_textEdited(const QString &arg1);
    void onRCMDeleteConnectionTriggered();
    void onRCMExportConnectionTriggered();
    void on_deleteSelectedConnBtn_clicked();
    void on_exportSelectedConnBtn_clicked();
    void on_connectionsTable_customContextMenuRequested(const QPoint &pos);
    void on_subscriptionTypeCB_currentIndexChanged(int arg1);

  private:
    void updateColorScheme() override;
    void reloadConnectionsList(const GroupId &group);
    void onRCMActionTriggered_Move();
    void onRCMActionTriggered_Copy();
    void onRCMActionTriggered_Link();
    void reloadGroupRCMActions();
    //
    void exportConnectionFilter(CONFIGROOT &root);
    //
    DnsSettingsWidget *dnsSettingsWidget;
    RouteSettingsMatrixWidget *routeSettingsWidget;
    //
    QMenu *connectionListRCMenu = new QMenu(this);
    QAction *exportConnectionAction = new QAction(tr("Export Connection(s)"), connectionListRCMenu);
    QAction *deleteConnectionAction = new QAction(tr("Delete Connection(s)"), connectionListRCMenu);
    QMenu *connectionListRCMenu_CopyToMenu = new QMenu(tr("Copy to..."));
    QMenu *connectionListRCMenu_MoveToMenu = new QMenu(tr("Move to..."));
    QMenu *connectionListRCMenu_LinkToMenu = new QMenu(tr("Link to..."));
    bool isUpdateInProgress = false;
    GroupId currentGroupId = NullGroupId;
    ConnectionId currentConnectionId = NullConnectionId;
};
