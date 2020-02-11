#pragma once

#include <QDialog>
#include <QtCore>
#include "common/QJsonModel.hpp"
#include "base/Qv2rayBase.hpp"
#include "ui_w_JsonEditor.h"
#include "ui/messaging/QvMessageBus.hpp"

class JsonEditor : public QDialog, private Ui::JsonEditor
{
        Q_OBJECT

    public:
        explicit JsonEditor(QJsonObject rootObject, QWidget *parent = nullptr);
        ~JsonEditor();
        QJsonObject OpenEditor();
    public slots:
        QvMessageBusSlotHeader

    private slots:
        void on_jsonEditor_textChanged();

        void on_formatJsonBtn_clicked();

    private:
        QJsonModel model;
        QJsonObject original;
        QJsonObject final;
};
