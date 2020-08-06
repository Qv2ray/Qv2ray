#pragma once

#include "base/Qv2rayBase.hpp"
#include "ui/common/QvDialog.hpp"
#include "ui/messaging/QvMessageBus.hpp"
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
            { "kernel", 1 },     //
            { "inbound", 2 },    //
            { "connection", 3 }, //
            { "dns", 4 },        //
            { "route", 5 },      //
            { "about", 6 }       //
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
    QvMessageBusSlotDecl;

  private slots:
    void on_buttonBox_accepted();
    void on_httpAuthCB_stateChanged(int arg1);
    void on_socksAuthCB_stateChanged(int arg1);
    void on_languageComboBox_currentTextChanged(const QString &arg1);
    void on_logLevelComboBox_currentIndexChanged(int index);
    void on_vCoreAssetsPathTxt_textEdited(const QString &arg1);
    void on_listenIPTxt_textEdited(const QString &arg1);
    void on_socksPortLE_valueChanged(int arg1);
    void on_httpPortLE_valueChanged(int arg1);
    void on_httpAuthUsernameTxt_textEdited(const QString &arg1);
    void on_httpAuthPasswordTxt_textEdited(const QString &arg1);
    void on_socksAuthUsernameTxt_textEdited(const QString &arg1);
    void on_socksAuthPasswordTxt_textEdited(const QString &arg1);
    void on_proxyDefaultCb_stateChanged(int arg1);
    void on_localDNSCb_stateChanged(int arg1);
    void on_selectVAssetBtn_clicked();
    void on_aboutQt_clicked();
    void on_cancelIgnoreVersionBtn_clicked();
    void on_tProxyCheckBox_stateChanged(int arg1);
    void on_bypassCNCb_stateChanged(int arg1);
    void on_bypassBTCb_stateChanged(int arg1);
    void on_statsPortBox_valueChanged(int arg1);
    void on_socksUDPCB_stateChanged(int arg1);
    void on_socksUDPIP_textEdited(const QString &arg1);
    void on_selectVCoreBtn_clicked();
    void on_vCorePathTxt_textEdited(const QString &arg1);
    void on_themeCombo_currentTextChanged(const QString &arg1);
    void on_darkThemeCB_stateChanged(int arg1);
    void on_darkTrayCB_stateChanged(int arg1);
    void on_setSysProxyCB_stateChanged(int arg1);
    void on_autoStartSubsCombo_currentIndexChanged(const QString &arg1);
    void on_autoStartConnCombo_currentIndexChanged(const QString &arg1);
    void on_fpTypeCombo_currentIndexChanged(const QString &arg1);
    void on_fpAddressTx_textEdited(const QString &arg1);
    void on_fpUseAuthCB_stateChanged(int arg1);
    void on_fpUsernameTx_textEdited(const QString &arg1);
    void on_fpPasswordTx_textEdited(const QString &arg1);
    void on_fpPortSB_valueChanged(int arg1);
    void on_checkVCoreSettings_clicked();
    void on_httpGroupBox_clicked(bool checked);
    void on_socksGroupBox_clicked(bool checked);
    void on_fpGroupBox_clicked(bool checked);
    void on_maxLogLinesSB_valueChanged(int arg1);
    void on_enableAPI_stateChanged(int arg1);
    void on_startWithLoginCB_stateChanged(int arg1);
    void on_updateChannelCombo_currentIndexChanged(int index);
    void on_pluginKernelV2RayIntegrationCB_stateChanged(int arg1);
    void on_pluginKernelPortAllocateCB_valueChanged(int arg1);
    void on_qvProxyAddressTxt_textEdited(const QString &arg1);
    void on_qvProxyTypeCombo_currentTextChanged(const QString &arg1);
    void on_qvProxyPortCB_valueChanged(int arg1);
    void on_setAllowInsecureCB_stateChanged(int arg1);
    void on_setTestLatenctCB_stateChanged(int arg1);
    void on_setSessionResumptionCB_stateChanged(int arg1);
    void on_quietModeCB_stateChanged(int arg1);
    void on_tproxGroupBox_toggled(bool arg1);
    void on_tProxyPort_valueChanged(int arg1);
    void on_tproxyEnableTCP_toggled(bool checked);
    void on_tproxyEnableUDP_toggled(bool checked);
    void on_tproxyMode_currentTextChanged(const QString &arg1);
    void on_tproxyListenAddr_textEdited(const QString &arg1);
    void on_tproxyListenV6Addr_textEdited(const QString &arg1);
    void on_jumpListCountSB_valueChanged(int arg1);
    void on_outboundMark_valueChanged(int arg1);
    void on_dnsIntercept_toggled(bool checked);
    void on_qvProxyCustomProxy_clicked();
    void on_qvProxySystemProxy_clicked();
    void on_qvProxyNoProxy_clicked();
    void on_DnsFreedomCb_stateChanged(int arg1);
    void on_httpSniffingCB_stateChanged(int arg1);
    void on_socksSniffingCB_stateChanged(int arg1);
    void on_pushButton_clicked();
    void on_noAutoConnectRB_clicked();
    void on_lastConnectedRB_clicked();
    void on_fixedAutoConnectRB_clicked();
    void on_latencyTCPingRB_clicked();
    void on_latencyICMPingRB_clicked();
    void on_qvNetworkUATxt_editTextChanged(const QString &arg1);
    void on_V2RayOutboundStatsCB_stateChanged(int arg1);
    void on_hasDirectStatisticsCB_stateChanged(int arg1);
    void on_useOldShareLinkFormatCB_stateChanged(int arg1);

  private:
    //
    DnsSettingsWidget *dnsSettingsWidget;
    RouteSettingsMatrixWidget *routeSettingsWidget;
    void SetAutoStartButtonsState(bool isAutoStart);
    //
    bool NeedRestart = false;
    bool finishedLoading = false;
    Qv2rayConfigObject CurrentConfig;
};
