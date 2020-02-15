#pragma once

#include <QWidget>
#include "ui_ConnectionWidget.h"
#include "core/handler/ConnectionHandler.hpp"

class ConnectionWidget : public QWidget, private Ui::ConnectionWidget
{
        Q_OBJECT

    public:
        explicit ConnectionWidget(const ConnectionId &id, QWidget *parent = nullptr);
        ~ConnectionWidget();

    private slots:
        void on_editBtn_clicked();

        void on_latencyBtn_clicked();

    private:
        const ConnectionId _id;
        const QvConnectionObject &connection;
};
