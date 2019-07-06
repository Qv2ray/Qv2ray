#ifndef IMPORTCONF_H
#define IMPORTCONF_H

#include <QDialog>
#include "ui_w_ImportConfig.h"
#include "QvUtils.hpp"

namespace Ui
{
    class ImportConfigWindow;
}

class ImportConfigWindow : public QDialog
{
        Q_OBJECT

    public:
        explicit ImportConfigWindow(QWidget *parent = nullptr);
        void savefromFile(QString path, QString alias);
        ~ImportConfigWindow();
    signals:
        void s_reload_config();
    private slots:
        void on_pushButton_clicked();
        void on_buttonBox_accepted();
        void on_verifyVMessBtn_clicked();

        void on_verifyFileBtn_clicked();

    signals:
        void updateConfTable();

    private:
        RootObject SavedConfig;
        Ui::ImportConfigWindow *ui;
};

#endif // IMPORTCONF_H
