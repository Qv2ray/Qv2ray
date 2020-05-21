#pragma once

#include "ui/messaging/QvMessageBus.hpp"
#include "ui_DnsSettingsWidget.h"

class DnsSettingsWidget
    : public QWidget
    , private Ui::DnsSettingsWidget
{
    Q_OBJECT

  public:
    explicit DnsSettingsWidget(QWidget *parent = nullptr);
    QvMessageBusSlotDecl;
};
