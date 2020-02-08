#pragma once

#include "ui_w_ExportConfig.h"
#include "base/Qv2rayBase.hpp"
#include "3rdparty/qzxing/src/QZXing.h"
#include "ui/messaging/QvMessageBus.hpp"

class ConfigExporter : public QDialog, private Ui::ExportConfigWindow
{
        Q_OBJECT

    public:
        explicit ConfigExporter(const CONFIGROOT &root, const ConnectionIdentifier &alias, QWidget *parent = nullptr);
        ~ConfigExporter();
        void OpenExport();
    public slots:
        QvMessageBusSlotHeader
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
