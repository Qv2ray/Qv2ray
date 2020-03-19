#pragma once
#include "QvAutoCompleteTextEdit.h"
#include "base/models/QvSettingsObject.hpp"
#include "ui_RouteSettingsMatrix.h"

#include <QWidget>

class RouteSettingsMatrixWidget
    : public QWidget
    , private Ui::RouteSettingsMatrix
{
    Q_OBJECT

  public:
    explicit RouteSettingsMatrixWidget(QWidget *parent = nullptr);
    void SetRouteConfig(const Qv2ray::base::config::Qv2rayRouteConfig conf, const QString &assetsDirPath);
    Qv2ray::base::config::Qv2rayRouteConfig GetRouteConfig() const;
    ~RouteSettingsMatrixWidget();

  private:
    Qv2ray::ui::widgets::AutoCompleteTextEdit *directDomainTxt;
    Qv2ray::ui::widgets::AutoCompleteTextEdit *proxyDomainTxt;
    Qv2ray::ui::widgets::AutoCompleteTextEdit *blockDomainTxt;
    //
    Qv2ray::ui::widgets::AutoCompleteTextEdit *directIPTxt;
    Qv2ray::ui::widgets::AutoCompleteTextEdit *blockIPTxt;
    Qv2ray::ui::widgets::AutoCompleteTextEdit *proxyIPTxt;
};
