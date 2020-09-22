#pragma once

#include "QvGUIPluginInterface.hpp"
#include "ui_httpin.h"

#include <QJsonArray>

class HTTPInboundEditor
    : public Qv2rayPlugin::QvPluginEditor
    , private Ui::httpInEditor
{
    Q_OBJECT

  public:
    explicit HTTPInboundEditor(QWidget *parent = nullptr);

    void SetHostAddress(const QString &, int) override{};
    QPair<QString, int> GetHostAddress() const override
    {
        return {};
    };

    void SetContent(const QJsonObject &content) override;
    const QJsonObject GetContent() const override
    {
        auto newObject = content;
        // Remove useless, misleading 'accounts' array.
        if (newObject["accounts"].toArray().count() == 0)
        {
            newObject.remove("accounts");
        }
        return newObject;
    }

  protected:
    void changeEvent(QEvent *e) override;

  private slots:

    void on_httpTimeoutSpinBox_valueChanged(int arg1);

    void on_httpTransparentCB_stateChanged(int arg1);

    void on_httpRemoveUserBtn_clicked();

    void on_httpAddUserBtn_clicked();
};
