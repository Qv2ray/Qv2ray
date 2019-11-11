#ifndef IMPORTCONF_H
#define IMPORTCONF_H

#include <QDialog>
#include "ui_w_ImportConfig.h"

class ImportConfigWindow : public QDialog, private Ui::ImportConfigWindow
{
        Q_OBJECT

    public:
        explicit ImportConfigWindow(QWidget *parent = nullptr);
        ~ImportConfigWindow() { }
    signals:
        void s_reload_config(bool need_restart);
    private slots:
        void on_importSourceCombo_currentIndexChanged(int index);

        void on_selectFileBtn_clicked();

        void on_qrFromScreenBtn_clicked();
        void on_beginImportBtn_clicked();
        void on_selectImageBtn_clicked();
        void on_errorsList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    private:
        QMap<QString, QString> vmessErrors;
};

#endif // IMPORTCONF_H
