#ifndef IMPORTCONF_H
#define IMPORTCONF_H

#include <QDialog>
#include "ui_w_ImportConfig.h"

class ImportConfigWindow : public QDialog, private Ui::ImportConfigWindow
{
        Q_OBJECT

    public:
        explicit ImportConfigWindow(QWidget *parent = nullptr);
        ~ImportConfigWindow();
        void OpenImporter();
    signals:
        void s_reload_config(bool need_restart);
    private slots:
        void on_importSourceCombo_currentIndexChanged(int index);

        void on_selectFileBtn_clicked();

        void on_buttonBox_accepted();

        void on_qrFromScreenBtn_clicked();
};

#endif // IMPORTCONF_H
