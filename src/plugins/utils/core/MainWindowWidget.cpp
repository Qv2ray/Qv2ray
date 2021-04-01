#include "MainWindowWidget.hpp"

#include <QFocusEvent>

MainWindowWidget::MainWindowWidget(QWidget *parent) : Qv2rayPlugin::QvPluginMainWindowWidget(parent)
{
    setupUi(this);
}

void MainWindowWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);

    switch (e->type())
    {
        case QEvent::LanguageChange: retranslateUi(this); break;
        default: break;
    }

    if (!isActiveWindow())
        close();
}
