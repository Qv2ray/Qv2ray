#ifndef W_QVOUTBOUNDEDITOR_H
#define W_QVOUTBOUNDEDITOR_H

#include <QDialog>

namespace Ui
{
    class RouteEditor;
}

class RouteEditor : public QDialog
{
        Q_OBJECT

    public:
        explicit RouteEditor(QWidget *parent = nullptr);
        ~RouteEditor();

    private:
        Ui::RouteEditor *ui;
};

#endif // W_QVOUTBOUNDEDITOR_H
