#pragma once

#include "base/Qv2rayBase.hpp"
#include "ui/common/QvDialog.hpp"
#include "ui/messaging/QvMessageBus.hpp"
#include "ui_w_ImportConfig.h"

class ImportConfigWindow
    : public QvDialog
    , private Ui::ImportConfigWindow
{
    Q_OBJECT

  public:
    explicit ImportConfigWindow(QWidget *parent = nullptr);
    ~ImportConfigWindow();
    int PerformImportConnection();
    QMultiMap<QString, CONFIGROOT> SelectConnection(bool outboundsOnly);
    void processCommands(QString command, QStringList commands, QMap<QString, QString> args) override
    {
        const static QMap<QString, int> indexMap{
            { "link", 0 },    //
            { "qr", 1 },      //
            { "manual", 2 },  //
            { "advanced", 3 } //
        };
        nameTxt->setText(args["name"]);
        if (commands.isEmpty())
            return;
        if (command == "open")
        {
            const auto c = commands.takeFirst();
            tabWidget->setCurrentIndex(indexMap[c]);
        }
    }

  private:
    QvMessageBusSlotDecl;
  private slots:
    void on_selectFileBtn_clicked();
    void on_qrFromScreenBtn_clicked();
    void on_beginImportBtn_clicked();
    void on_selectImageBtn_clicked();
    void on_errorsList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void on_connectionEditBtn_clicked();
    void on_cancelImportBtn_clicked();
    void on_routeEditBtn_clicked();
    void on_hideQv2rayCB_stateChanged(int arg1);
    void on_jsonEditBtn_clicked();

  private:
    void updateColorScheme() override;
    QMap<QString, QString> linkErrors;
    //
    // Use hash here since the order is not important.
    QHash<GroupId, QMultiMap<QString, CONFIGROOT>> connectionsToExistingGroup;
    QMap<QString, QMultiMap<QString, CONFIGROOT>> connectionsToNewGroup;
};
