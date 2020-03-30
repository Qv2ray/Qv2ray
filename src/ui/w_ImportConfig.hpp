#pragma once

#include "base/Qv2rayBase.hpp"
#include "ui/messaging/QvMessageBus.hpp"
#include "ui_w_ImportConfig.h"

#include <QDialog>
#include <QJsonObject>
#include <QString>

class ImportConfigWindow
    : public QDialog
    , private Ui::ImportConfigWindow
{
    Q_OBJECT

  public:
    explicit ImportConfigWindow(QWidget *parent = nullptr);
    ~ImportConfigWindow();
    int ImportConnection();
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
    QMap<QString, QMultiHash<QString, CONFIGROOT>> connections;
    QMap<QString, QString> linkErrors;
    void AddToGroup(const QString &groupName, const QString &alias, const CONFIGROOT &root)
    {
        if (connections.contains(groupName))
        {
            connections[groupName].insert(alias, root);
        }
        else
        {
            QMultiHash<QString, CONFIGROOT> temp;
            temp.insert(alias, root);
            connections.insert(groupName, temp);
        }
    }
};
