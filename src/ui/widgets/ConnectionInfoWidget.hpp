#pragma once

#include <QWidget>
#include "ui_ConnectionInfoWidget.h"
#include "core/handler/ConnectionHandler.hpp"

class ConnectionInfoWidget : public QWidget, private Ui::ConnectionInfoWidget
{
        Q_OBJECT

    public:
        explicit ConnectionInfoWidget(QWidget *parent = nullptr);
        void ShowDetails(const tuple<GroupId, ConnectionId> &_identifier);
        ~ConnectionInfoWidget();

    private slots:
        void on_connectBtn_clicked();
        void on_editBtn_clicked();
        void on_editJsonBtn_clicked();
        void on_deleteBtn_clicked();

    protected:
        bool eventFilter(QObject *object, QEvent *event) override;

    private slots:
        void OnConnected(const ConnectionId &id);
        void OnDisConnected(const ConnectionId &id);

        void on_duplicateBtn_clicked();

        void on_latencyBtn_clicked();

    private:
        ConnectionId connectionId = NullConnectionId;
        GroupId groupId = NullGroupId;
};

