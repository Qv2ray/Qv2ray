#ifndef W_QVOUTBOUNDEDITOR_H
#define W_QVOUTBOUNDEDITOR_H

#include <list>
#include <QDialog>
#include <QJsonArray>
#include <QJsonObject>
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

        void on_routesTable_cellClicked(int row, int column);

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
        Ui::RouteEditor *ui;
};

#endif // W_QVOUTBOUNDEDITOR_H
