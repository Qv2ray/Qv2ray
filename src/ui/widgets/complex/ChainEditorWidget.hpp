#pragma once

#include "ui_ChainEditorWidget.h"

class ChainEditorWidget : public QWidget, private Ui::ChainEditorWidget
{
    Q_OBJECT

public:
    explicit ChainEditorWidget(QWidget *parent = nullptr);

protected:
    void changeEvent(QEvent *e);
};

