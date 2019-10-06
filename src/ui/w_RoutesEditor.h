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
        explicit RouteEditor(QJsonObject connection, const QString alias, QWidget *parent = nullptr);
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

    private:
        QJsonArray inbounds;
        QJsonArray outbounds;
        RoutingObject routes;
        QJsonObject root;
        QString rootAlias;
        Ui::RouteEditor *ui;
};

#endif // W_QVOUTBOUNDEDITOR_H
