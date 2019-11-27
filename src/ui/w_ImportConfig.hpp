﻿#ifndef IMPORTCONF_H
#define IMPORTCONF_H

#include <QDialog>
#include <QString>
#include <QJsonObject>
#include "ui_w_ImportConfig.h"

class ImportConfigWindow : public QDialog, private Ui::ImportConfigWindow
{
        Q_OBJECT

    public:
        explicit ImportConfigWindow(QWidget *parent = nullptr);
        ~ImportConfigWindow() { }
        QMap<QString, QJsonObject> OpenImport(bool outboundsOnly = false);
    private slots:
        void on_importSourceCombo_currentIndexChanged(int index);

        void on_selectFileBtn_clicked();

        void on_qrFromScreenBtn_clicked();
        void on_beginImportBtn_clicked();
        void on_selectImageBtn_clicked();
        void on_errorsList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

        void on_editFileBtn_clicked();

        void on_connectionEditBtn_clicked();

        void on_cancelImportBtn_clicked();

    private:
        QMap<QString, QJsonObject> connections;
        QMap<QString, QString> vmessErrors;
};

#endif // IMPORTCONF_H
