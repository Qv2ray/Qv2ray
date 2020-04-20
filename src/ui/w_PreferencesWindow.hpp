#pragma once

#include "base/Qv2rayBase.hpp"
#include "ui/messaging/QvMessageBus.hpp"

#include <QDialog>
#include <ui_w_PreferencesWindow.h>

class RouteSettingsMatrixWidget;

class PreferencesWindow
    : public QDialog
    , private Ui::PreferencesWindow
{
    Q_OBJECT

  public:
    explicit PreferencesWindow(QWidget *parent = nullptr);
    ~PreferencesWindow();

  private:
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

    void on_DNSListTxt_textChanged();

    void on_aboutQt_clicked();

    void on_cancelIgnoreVersionBtn_clicked();

    void on_tProxyCheckBox_stateChanged(int arg1);

    void on_bypassCNCb_stateChanged(int arg1);

    void on_statsPortBox_valueChanged(int arg1);

    void on_socksUDPCB_stateChanged(int arg1);

    void on_socksUDPIP_textEdited(const QString &arg1);

    void on_nsBarPageAddBTN_clicked();

    void on_nsBarPageDelBTN_clicked();

    void on_nsBarPageYOffset_valueChanged(int arg1);

    void on_nsBarLineAddBTN_clicked();

    void on_nsBarLineDelBTN_clicked();

    void on_nsBarPagesList_currentRowChanged(int currentRow);

    void on_nsBarLinesList_currentRowChanged(int currentRow);

    void on_fontComboBox_currentFontChanged(const QFont &f);

    void on_nsBarFontBoldCB_stateChanged(int arg1);

    void on_nsBarFontItalicCB_stateChanged(int arg1);

    void on_nsBarFontASB_valueChanged(int arg1);

    void on_nsBarFontRSB_valueChanged(int arg1);

    void on_nsBarFontGSB_valueChanged(int arg1);

    void on_nsBarFontBSB_valueChanged(int arg1);

    void on_nsBarFontSizeSB_valueChanged(double arg1);

    void on_chooseColorBtn_clicked();

    void on_nsBarTagTxt_textEdited(const QString &arg1);

    void on_nsBarContentCombo_currentIndexChanged(const QString &arg1);

    void on_applyNSBarSettingsBtn_clicked();

    void on_selectVCoreBtn_clicked();

    void on_vCorePathTxt_textEdited(const QString &arg1);

    void on_themeCombo_currentTextChanged(const QString &arg1);

    void on_darkThemeCB_stateChanged(int arg1);

    void on_darkTrayCB_stateChanged(int arg1);

    void on_setSysProxyCB_stateChanged(int arg1);

    void on_pushButton_clicked();

    void on_autoStartSubsCombo_currentIndexChanged(const QString &arg1);

    void on_autoStartConnCombo_currentIndexChanged(const QString &arg1);

    void on_fpTypeCombo_currentIndexChanged(const QString &arg1);

    void on_fpAddressTx_textEdited(const QString &arg1);

    void on_spPortSB_valueChanged(int arg1);

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

    void on_pluginKernelV2rayIntegrationCB_stateChanged(int arg1);

    void on_pluginKernelPortAllocateCB_valueChanged(int arg1);

    void on_qvProxyAddressTxt_textEdited(const QString &arg1);

    void on_qvProxyTypeCombo_currentTextChanged(const QString &arg1);

    void on_qvProxyPortCB_valueChanged(int arg1);

    void on_qvNetworkUATxt_textEdited(const QString &arg1);

    void on_setAllowInsecureCB_stateChanged(int arg1);

    void on_setTestLatenctCB_stateChanged(int arg1);

    void on_setAllowInsecureCiphersCB_stateChanged(int arg1);

    void on_quietModeCB_stateChanged(int arg1);

    void on_tproxGroupBox_toggled(bool arg1);

    void on_tProxyPort_valueChanged(int arg1);

    void on_tproxyEnableTCP_toggled(bool checked);

    void on_tproxyEnableUDP_toggled(bool checked);

    void on_tproxyFollowRedirect_toggled(bool checked);

    void on_tproxyMode_currentTextChanged(const QString &arg1);

    void on_tproxyListenAddr_textEdited(const QString &arg1);

    void on_customProxySettingsGroupBox_clicked(bool checked);

  private:
    //
    RouteSettingsMatrixWidget *routeSettingsWidget;
    void SetAutoStartButtonsState(bool isAutoStart);
    // Set ui parameters for a line;
    void ShowLineParameters(QvBarLine &line);
    QString GetBarLineDescription(const QvBarLine &barLine);
    //
    int CurrentBarLineId;
    int CurrentBarPageId;
    //
    bool NeedRestart = false;
    bool finishedLoading = false;
    Qv2rayConfig CurrentConfig;
};
