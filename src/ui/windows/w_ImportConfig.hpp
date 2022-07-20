#pragma once

#include "ui/WidgetUIBase.hpp"
#include "ui_w_ImportConfig.h"

class ImportConfigWindow
    : public QvDialog
    , private Ui::ImportConfigWindow
{
    Q_OBJECT

  public:
    explicit ImportConfigWindow(QWidget *parent = nullptr);
    ~ImportConfigWindow();
    std::pair<GroupId, QMultiMap<QString, ProfileContent>> DoImportConnections();
    void processCommands(QString command, QStringList commands, QMap<QString, QString> args) override;

  private:
    QvMessageBusSlotDecl override;

  private slots:
    void on_beginImportBtn_clicked();
    void on_cancelImportBtn_clicked();
    void on_jsonEditBtn_clicked();
    void on_groupCombo_currentIndexChanged(int index);

  private:
    void updateColorScheme() override{};

    // Use hash here since the order is not important.
    GroupId selectedGroup = DefaultGroupId;
    QMultiMap<QString, ProfileContent> connections;
};
