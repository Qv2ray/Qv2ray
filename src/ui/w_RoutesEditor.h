#ifndef W_QVOUTBOUNDEDITOR_H
#define W_QVOUTBOUNDEDITOR_H

#include <list>
#include <QDialog>
#include <QJsonArray>
#include <QJsonObject>
#include <QListWidgetItem>
#include "QvUtils.h"

namespace Ui
{
    class RouteEditor;
}

class RouteEditor : public QDialog
{
        Q_OBJECT

    public:
        explicit RouteEditor(QJsonObject connection, QWidget *parent = nullptr);
        ~RouteEditor();
        QJsonObject OpenEditor();
    private slots:
        void on_buttonBox_accepted();

        void on_outboundsList_currentRowChanged(int currentRow);

        void on_inboundsList_currentRowChanged(int currentRow);

        void on_editOutboundBtn_clicked();

        void on_insertDirectBtn_clicked();

        void on_editInboundBtn_clicked();

        void on_routeProtocolHTTPCB_stateChanged(int arg1);

        void on_routeProtocolTLSCB_stateChanged(int arg1);

        void on_routeProtocolBTCB_stateChanged(int arg1);

        void on_balabcerAddBtn_clicked();

        void on_balancerDelBtn_clicked();

        void on_hostList_textChanged();

        void on_ipList_textChanged();

        void on_routePortTxt_textEdited(const QString &arg1);

        void on_routeUserTxt_textEdited(const QString &arg1);

        void on_routesTable_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);

        void on_addRouteBtn_clicked();

        void on_changeIOBtn_clicked();

        void on_routesTable_cellChanged(int row, int column);

        void on_netBothRB_clicked();

        void on_netUDPRB_clicked();

        void on_netTCPRB_clicked();

        void on_routeUserTxt_textChanged();

        void on_sourceIPList_textChanged();

        void on_enableBalancerCB_stateChanged(int arg1);

        void on_routeOutboundSelector_currentIndexChanged(int index);

        void on_inboundsList_itemChanged(QListWidgetItem *item);

        void on_delRouteBtn_clicked();

    private:
        void ShowRuleDetail(RuleObject rule);
        int currentRuleIndex;
        QString DomainStrategy;
        QMap<QString, QStringList> Balancers;
        QList<RuleObject> rules;
        //
        QJsonArray inbounds;
        QJsonArray outbounds;
        QJsonObject root;
        QJsonObject original;
        Ui::RouteEditor *ui;
};

#endif // W_QVOUTBOUNDEDITOR_H
