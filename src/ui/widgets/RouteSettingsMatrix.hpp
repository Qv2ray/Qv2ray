#pragma once
#include "QvAutoCompleteTextEdit.hpp"
#include "base/models/QvSettingsObject.hpp"
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
    RouteSettingsMatrixWidget(const QString &assetsDirPath, QWidget *parent = nullptr);
    void SetRouteConfig(const Qv2ray::base::config::Qv2rayConfig_Routing &conf);
    Qv2ray::base::config::Qv2rayConfig_Routing GetRouteConfig() const;
    ~RouteSettingsMatrixWidget();

  private:
    std::optional<QString> openFileDialog();
    std::optional<QString> saveFileDialog();
    QList<QAction *> getBuiltInSchemes();
    QAction *schemeToAction(const QString &name, const Qv2ray::base::config::Qv2rayConfig_Routing &scheme);

  private:
    QMenu *builtInSchemesMenu;

  private slots:
    void on_importSchemeBtn_clicked();
    void on_exportSchemeBtn_clicked();

  private:
    const QString &assetsDirPath;

  private:
    Qv2ray::ui::widgets::AutoCompleteTextEdit *directDomainTxt;
    Qv2ray::ui::widgets::AutoCompleteTextEdit *proxyDomainTxt;
    Qv2ray::ui::widgets::AutoCompleteTextEdit *blockDomainTxt;
    //
    Qv2ray::ui::widgets::AutoCompleteTextEdit *directIPTxt;
    Qv2ray::ui::widgets::AutoCompleteTextEdit *blockIPTxt;
    Qv2ray::ui::widgets::AutoCompleteTextEdit *proxyIPTxt;
};
