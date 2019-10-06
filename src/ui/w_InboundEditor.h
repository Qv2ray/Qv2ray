#ifndef W_INBOUNDEDITOR_H
#define W_INBOUNDEDITOR_H

#include <QDialog>

namespace Ui {
class InboundEditor;
}

class InboundEditor : public QDialog
{
    Q_OBJECT

public:
    explicit InboundEditor(QWidget *parent = nullptr);
    ~InboundEditor();

private:
    Ui::InboundEditor *ui;
};

#endif // W_INBOUNDEDITOR_H
