#pragma once

#include "ui_w_ExportConfig.h"
#include "3rdparty/qzxing/src/QZXing.h"

class ConfigExporter : public QDialog, private Ui::ExportConfigWindow
{
        Q_OBJECT

    public:
        explicit ConfigExporter(const QImage &img, QWidget *parent = nullptr);
        explicit ConfigExporter(const QString &data, QWidget *parent = nullptr);
        ~ConfigExporter();
        void OpenExport();
    protected:
        void changeEvent(QEvent *e);
    private slots:
        void on_closeBtn_clicked();

        void on_saveBtn_clicked();

        void on_copyImageBtn_clicked();

        void on_copyVMessBtn_clicked();

    private:
        explicit ConfigExporter(QWidget *parent);
        QZXing qzxing;
        QImage image;
        QString message;
};
