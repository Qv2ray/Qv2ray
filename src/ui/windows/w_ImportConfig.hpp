#pragma once

#include "base/Qv2rayBase.hpp"
#include "ui/messaging/QvMessageBus.hpp"
#include "ui_w_ImportConfig.h"

#include <QDialog>

class ImportConfigWindow
    : public QDialog
    , private Ui::ImportConfigWindow
{
    Q_OBJECT

  public:
    explicit ImportConfigWindow(QWidget *parent = nullptr);
    ~ImportConfigWindow();
    int PerformImportConnection();
    QMultiHash<QString, CONFIGROOT> SelectConnection(bool outboundsOnly);

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

    void on_subscriptionButton_clicked();

    void on_routeEditBtn_clicked();

    void on_hideQv2rayCB_stateChanged(int arg1);

    void on_jsonEditBtn_clicked();

  private:
    void UpdateColorScheme();
    QMap<QString, QString> linkErrors;
    //
    QHash<GroupId, QMultiHash<QString, CONFIGROOT>> connectionsToExistingGroup;
    QHash<QString, QMultiHash<QString, CONFIGROOT>> connectionsToNewGroup;
};
