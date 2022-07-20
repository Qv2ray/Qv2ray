#pragma once

#include "MessageBus/MessageBus.hpp"
#include "Qv2rayBase/Common/Settings.hpp"
#include "ui/WidgetUIBase.hpp"
#include "ui_w_PreferencesWindow.h"

class RouteSettingsMatrixWidget;
class DnsSettingsWidget;

class PreferencesWindow
    : public QvDialog
    , private Ui::PreferencesWindow
{
    Q_OBJECT

  public:
    explicit PreferencesWindow(QWidget *parent = nullptr);
    ~PreferencesWindow();
    void processCommands(QString command, QStringList commands, QMap<QString, QString>) override
    {
        const static QMap<QString, int> indexMap{
            { "general", 0 },    //
            { "inbound", 1 },    //
            { "connection", 2 }, //
            { "dns", 3 },        //
        };

        if (commands.isEmpty())
            return;
        if (command == "open")
        {
            const auto c = commands.takeFirst();
            tabWidget->setCurrentIndex(indexMap[c]);
        }
    }

  private:
    void updateColorScheme() override{};
    QvMessageBusSlotDecl override;

  private slots:
    void on_autoStartConnCombo_currentIndexChanged(int arg1);
    void on_autoStartSubsCombo_currentIndexChanged(int arg1);
    void on_buttonBox_accepted();

    void on_fixedAutoConnectRB_clicked();

    void on_lastConnectedRB_clicked();

    void on_noAutoConnectRB_clicked();
    void on_pluginKernelPortAllocateCB_valueChanged(int arg1);

    void on_qvNetworkUATxt_editTextChanged(const QString &arg1);
    void on_qvProxyAddressTxt_textEdited(const QString &arg1);
    void on_qvProxyPortCB_valueChanged(int arg1);

    void on_startWithLoginCB_stateChanged(int arg1);

    // Inbound options
    void on_tproxyOverrideFakeDNSCB_stateChanged(int arg1);
    void on_tproxyOverrideHTTPCB_stateChanged(int arg1);
    void on_tproxyOverrideTLSCB_stateChanged(int arg1);

    void on_httpOverrideFakeDNSCB_stateChanged(int arg1);
    void on_httpOverrideHTTPCB_stateChanged(int arg1);
    void on_httpOverrideTLSCB_stateChanged(int arg1);

    void on_socksOverrideFakeDNSCB_stateChanged(int arg1);
    void on_socksOverrideHTTPCB_stateChanged(int arg1);
    void on_socksOverrideTLSCB_stateChanged(int arg1);

    void on_qvProxyTypeCombo_currentIndexChanged(int index);

    void on_defaultLatencyTesterCB_currentIndexChanged(int index);

    void on_defaultKernelCB_currentIndexChanged(int index);

  private:
    DnsSettingsWidget *dnsSettingsWidget;
    RouteSettingsMatrixWidget *routeSettingsWidget;
    void SetAutoStartButtonsState(bool isAutoStart);
    bool NeedRestart = false;
    Qv2ray::Models::Qv2rayApplicationConfigObject AppConfig;
    Qv2rayBase::Models::Qv2rayBaseConfigObject BaselibConfig;
};
