#pragma once

#include "RouteSchemeIO/RouteSchemeIO.hpp"
#include "ui/widgets/AutoCompleteTextEdit.hpp"
#include "ui_RouteSettingsMatrix.h"

#include <QMenu>
#include <QWidget>
#include <optional>

class RouteSettingsMatrixWidget
    : public QWidget
    , private Ui::RouteSettingsMatrix
{
    Q_OBJECT

  public:
    RouteSettingsMatrixWidget(QWidget *parent = nullptr);
    void SetRoute(const Qv2ray::Models::RouteMatrixConfig &conf);
    Qv2ray::Models::RouteMatrixConfig GetRouteConfig() const;
    ~RouteSettingsMatrixWidget();

  private:
    std::optional<QString> openFileDialog();
    std::optional<QString> saveFileDialog();

  private slots:
    void on_importSchemeBtn_clicked();
    void on_exportSchemeBtn_clicked();

  private:
    Qv2ray::ui::widgets::AutoCompleteTextEdit *directDomainTxt;
    Qv2ray::ui::widgets::AutoCompleteTextEdit *proxyDomainTxt;
    Qv2ray::ui::widgets::AutoCompleteTextEdit *blockDomainTxt;

    Qv2ray::ui::widgets::AutoCompleteTextEdit *directIPTxt;
    Qv2ray::ui::widgets::AutoCompleteTextEdit *blockIPTxt;
    Qv2ray::ui::widgets::AutoCompleteTextEdit *proxyIPTxt;

  private:
    QMenu *builtInSchemesMenu;
};
