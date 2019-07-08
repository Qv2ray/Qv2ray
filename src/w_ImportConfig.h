#ifndef IMPORTCONF_H
#define IMPORTCONF_H

#include <QDialog>
#include "ui_w_ImportConfig.h"

namespace Ui
{
    class ImportConfigWindow;
}

class ImportConfigWindow : public QDialog
{
        Q_OBJECT

    public:
        explicit ImportConfigWindow(QWidget *parent = nullptr);
        ~ImportConfigWindow();
    signals:
        void s_reload_config();
    private slots:
        void on_importSourceCombo_currentIndexChanged(int index);

        void on_selectFileBtn_clicked();

        void on_buttonBox_clicked(QAbstractButton *button);

    private:
        Ui::ImportConfigWindow *ui;

};

#endif // IMPORTCONF_H
