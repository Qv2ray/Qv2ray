#ifndef W_JSONEDITOR_H
#define W_JSONEDITOR_H

#include <QDialog>
#include <QtCore>
#include "QJsonModel.h"

namespace Ui
{
    class JsonEditor;
}

class JsonEditor : public QDialog
{
        Q_OBJECT

    public:
        explicit JsonEditor(QJsonObject rootObject, QWidget *parent = nullptr);
        ~JsonEditor();
        QJsonObject OpenEditor();

    private slots:
        void on_jsonEditor_textChanged();

        void on_formatJsonBtn_clicked();

    private:
        QJsonModel model;
        QJsonObject original;
        QJsonObject result;
        Ui::JsonEditor *ui;
};

#endif // W_JSONEDITOR_H
