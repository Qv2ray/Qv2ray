#include "w_PreferencesWindow.hpp"

#include "components/ntp/QvNTPClient.hpp"
#include "components/translations/QvTranslator.hpp"
#include "core/connection/ConnectionIO.hpp"
#include "core/handler/ConfigHandler.hpp"
#include "core/kernel/V2RayKernelInteractions.hpp"
#include "core/settings/SettingsBackend.hpp"
#include "src/plugin-interface/QvPluginInterface.hpp"
#include "ui/common/autolaunch/QvAutoLaunch.hpp"
#include "ui/widgets/styles/StyleManager.hpp"
#include "ui/widgets/widgets/DnsSettingsWidget.hpp"
#include "ui/widgets/widgets/RouteSettingsMatrix.hpp"
#include "utils/HTTPRequestHelper.hpp"
#include "utils/QvHelpers.hpp"

#include <QColorDialog>
#include <QCompleter>
#include <QDesktopServices>
#include <QFileDialog>
#include <QHostInfo>
#include <QInputDialog>
#include <QMessageBox>

using Qv2ray::common::validation::IsIPv4Address;
using Qv2ray::common::validation::IsIPv6Address;
using Qv2ray::common::validation::IsValidDNSServer;
using Qv2ray::common::validation::IsValidIPAddress;

#define LOADINGCHECK                                                                                                                                 \
    if (!finishedLoading)                                                                                                                            \
        return;
#define NEEDRESTART                                                                                                                                  \
    LOADINGCHECK                                                                                                                                     \
    if (finishedLoading)                                                                                                                             \
        NeedRestart = true;

#define SET_PROXY_UI_ENABLE(_enabled)                                                                                                                \
    qvProxyTypeCombo->setEnabled(_enabled);                                                                                                          \
    qvProxyAddressTxt->setEnabled(_enabled);                                                                                                         \
    qvProxyPortCB->setEnabled(_enabled);

#define SET_AUTOSTART_UI_ENABLED(_enabled)                                                                                                           \
    autoStartConnCombo->setEnabled(_enabled);                                                                                                        \
    autoStartSubsCombo->setEnabled(_enabled);

#define SET_AUTOSTART_START_MINIMIZED_ENABLED(_enabled) startMinimizedCB->setEnabled(_enabled);

PreferencesWindow::PreferencesWindow(QWidget *parent) : QvDialog("PreferenceWindow", parent), CurrentConfig()
{
    addStateOptions("width", { [&] { return width(); }, [&](QJsonValue val) { resize(val.toInt(), size().height()); } });
    addStateOptions("height", { [&] { return height(); }, [&](QJsonValue val) { resize(size().width(), val.toInt()); } });
    addStateOptions("x", { [&] { return x(); }, [&](QJsonValue val) { move(val.toInt(), y()); } });
    addStateOptions("y", { [&] { return y(); }, [&](QJsonValue val) { move(x(), val.toInt()); } });

    setupUi(this);
    //
    QvMessageBusConnect(PreferencesWindow);
    textBrowser->setHtml(StringFromFile(":/assets/credit.html"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    configdirLabel->setText(QV2RAY_CONFIG_DIR);

    // We add locales
    auto langs = Qv2rayTranslator->GetAvailableLanguages();
    if (!langs.empty())
    {
        languageComboBox->clear();
        languageComboBox->addItems(langs);
    }
    else
    {
        languageComboBox->setDisabled(true);
        // Since we can't have languages detected. It worths nothing to translate these.
        languageComboBox->setToolTip("Cannot find any language providers.");
    }

    // Set auto start button state
    SetAutoStartButtonsState(GetLaunchAtLoginStatus());
    themeCombo->addItems(StyleManager->AllStyles());
    //
    qvVersion->setText(QV2RAY_VERSION_STRING ":" + QSTRN(QV2RAY_VERSION_BUILD));
    qvBuildInfo->setText(QV2RAY_BUILD_INFO);
    qvBuildExInfo->setText(QV2RAY_BUILD_EXTRA_INFO);
    qvBuildTime->setText(__DATE__ " " __TIME__);
    qvPluginInterfaceVersionLabel->setText(tr("Version: %1").arg(QV2RAY_PLUGIN_INTERFACE_VERSION));
    //
    // Deep copy
    CurrentConfig.loadJson(GlobalConfig.toJson());
    //
    themeCombo->setCurrentText(CurrentConfig.uiConfig.theme);
    darkThemeCB->setChecked(CurrentConfig.uiConfig.useDarkTheme);
    darkTrayCB->setChecked(CurrentConfig.uiConfig.useDarkTrayIcon);
    glyphTrayCB->setChecked(CurrentConfig.uiConfig.useGlyphTrayIcon);
    languageComboBox->setCurrentText(CurrentConfig.uiConfig.language);
    logLevelComboBox->setCurrentIndex(CurrentConfig.logLevel);
    quietModeCB->setChecked(CurrentConfig.uiConfig.quietMode);
    useOldShareLinkFormatCB->setChecked(CurrentConfig.uiConfig.useOldShareLinkFormat);
    startMinimizedCB->setChecked(CurrentConfig.uiConfig.startMinimized);
    startMinimizedCB->setEnabled(CurrentConfig.autoStartBehavior != AUTO_CONNECTION_NONE);
    exitByCloseEventCB->setChecked(CurrentConfig.uiConfig.exitByCloseEvent);
    //
    //
    listenIPTxt->setText(CurrentConfig.inboundConfig.listenip);
    //
    {
        const auto &httpSettings = CurrentConfig.inboundConfig.httpSettings;
        const auto has_http = CurrentConfig.inboundConfig.useHTTP;
        httpGroupBox->setChecked(has_http);
        httpPortLE->setValue(httpSettings.port);
        httpAuthCB->setChecked(httpSettings.useAuth);
        //
        httpAuthUsernameTxt->setEnabled(has_http && httpSettings.useAuth);
        httpAuthPasswordTxt->setEnabled(has_http && httpSettings.useAuth);
        httpAuthUsernameTxt->setText(httpSettings.account.user);
        httpAuthPasswordTxt->setText(httpSettings.account.pass);
        //
        httpSniffingCB->setChecked(httpSettings.sniffing);
        httpSniffingMetadataOnly->setEnabled(has_http && httpSettings.sniffing);
        httpOverrideHTTPCB->setEnabled(has_http && httpSettings.sniffing);
        httpOverrideTLSCB->setEnabled(has_http && httpSettings.sniffing);
        httpOverrideFakeDNSCB->setEnabled(has_http && httpSettings.sniffing);
        httpOverrideFakeDNSOthersCB->setEnabled(has_http && httpSettings.sniffing);
        httpOverrideHTTPCB->setChecked(httpSettings.destOverride.contains("http"));
        httpOverrideTLSCB->setChecked(httpSettings.destOverride.contains("tls"));
        httpOverrideFakeDNSCB->setChecked(httpSettings.destOverride.contains("fakedns"));
        httpOverrideFakeDNSOthersCB->setChecked(httpSettings.destOverride.contains("fakedns+others"));
        httpSniffingMetadataOnly->setChecked(httpSettings.metadataOnly);
    }
    {
        const auto &socksSettings = CurrentConfig.inboundConfig.socksSettings;
        const auto has_socks = CurrentConfig.inboundConfig.useSocks;
        socksGroupBox->setChecked(has_socks);
        socksPortLE->setValue(socksSettings.port);
        //
        socksAuthCB->setChecked(socksSettings.useAuth);
        socksAuthUsernameTxt->setEnabled(has_socks && socksSettings.useAuth);
        socksAuthPasswordTxt->setEnabled(has_socks && socksSettings.useAuth);
        socksAuthUsernameTxt->setText(socksSettings.account.user);
        socksAuthPasswordTxt->setText(socksSettings.account.pass);
        // Socks UDP Options
        socksUDPCB->setChecked(socksSettings.enableUDP);
        socksUDPIP->setEnabled(has_socks && socksSettings.enableUDP);
        socksUDPIP->setText(socksSettings.localIP);
        //
        socksSniffingCB->setChecked(socksSettings.sniffing);
        socksSniffingMetadataOnly->setEnabled(has_socks && socksSettings.sniffing);
        socksOverrideHTTPCB->setEnabled(has_socks && socksSettings.sniffing);
        socksOverrideTLSCB->setEnabled(has_socks && socksSettings.sniffing);
        socksOverrideFakeDNSCB->setEnabled(has_socks && socksSettings.sniffing);
        socksOverrideFakeDNSOthersCB->setEnabled(has_socks && socksSettings.sniffing);
        socksOverrideHTTPCB->setChecked(socksSettings.destOverride.contains("http"));
        socksOverrideTLSCB->setChecked(socksSettings.destOverride.contains("tls"));
        socksOverrideFakeDNSCB->setChecked(socksSettings.destOverride.contains("fakedns"));
        socksOverrideFakeDNSOthersCB->setChecked(socksSettings.destOverride.contains("fakedns+others"));
        socksSniffingMetadataOnly->setChecked(socksSettings.metadataOnly);
    }
    {
        const auto &tProxySettings = CurrentConfig.inboundConfig.tProxySettings;
        const auto has_tproxy = CurrentConfig.inboundConfig.useTPROXY;
        tproxyGroupBox->setChecked(has_tproxy);
        tproxyListenAddr->setText(tProxySettings.tProxyIP);
        tproxyListenV6Addr->setText(tProxySettings.tProxyV6IP);
        tProxyPort->setValue(tProxySettings.port);
        tproxyEnableTCP->setChecked(tProxySettings.hasTCP);
        tproxyEnableUDP->setChecked(tProxySettings.hasUDP);
        //
        tproxySniffingCB->setChecked(tProxySettings.sniffing);
        tproxySniffingMetadataOnlyCB->setEnabled(has_tproxy && tProxySettings.sniffing);
        tproxyOverrideHTTPCB->setEnabled(has_tproxy && tProxySettings.sniffing);
        tproxyOverrideTLSCB->setEnabled(has_tproxy && tProxySettings.sniffing);
        tproxyOverrideFakeDNSCB->setEnabled(has_tproxy && tProxySettings.sniffing);
        tproxyOverrideFakeDNSOthersCB->setEnabled(has_tproxy && tProxySettings.sniffing);
        tproxyOverrideHTTPCB->setChecked(tProxySettings.destOverride.contains("http"));
        tproxyOverrideTLSCB->setChecked(tProxySettings.destOverride.contains("tls"));
        tproxyOverrideFakeDNSCB->setChecked(tProxySettings.destOverride.contains("fakedns"));
        tproxyOverrideFakeDNSOthersCB->setChecked(tProxySettings.destOverride.contains("fakedns+others"));
        tproxySniffingMetadataOnlyCB->setChecked(tProxySettings.metadataOnly);

        tproxyMode->setCurrentText(tProxySettings.mode);
    }
    {
        const auto &browserForwarderSettings = CurrentConfig.inboundConfig.browserForwarderSettings;
        browserForwarderAddressTxt->setText(browserForwarderSettings.address);
        browserForwarderPortSB->setValue(browserForwarderSettings.port);
    }
    outboundMark->setValue(CurrentConfig.outboundConfig.mark);
    //
    dnsIntercept->setChecked(CurrentConfig.defaultRouteConfig.connectionConfig.dnsIntercept);
    dnsFreedomCb->setChecked(CurrentConfig.defaultRouteConfig.connectionConfig.v2rayFreedomDNS);
    //
    // Kernel Settings
    {
        vCorePathTxt->setText(CurrentConfig.kernelConfig.KernelPath());
        vCoreAssetsPathTxt->setText(CurrentConfig.kernelConfig.AssetsPath());
        enableAPI->setChecked(CurrentConfig.kernelConfig.enableAPI);
        statsPortBox->setValue(CurrentConfig.kernelConfig.statsPort);
        //
        V2RayOutboundStatsCB->setChecked(CurrentConfig.uiConfig.graphConfig.useOutboundStats);
        hasDirectStatisticsCB->setEnabled(CurrentConfig.uiConfig.graphConfig.useOutboundStats);
        hasDirectStatisticsCB->setChecked(CurrentConfig.uiConfig.graphConfig.hasDirectStats);
        //
        pluginKernelV2RayIntegrationCB->setChecked(CurrentConfig.pluginConfig.v2rayIntegration);
        pluginKernelPortAllocateCB->setValue(CurrentConfig.pluginConfig.portAllocationStart);
        pluginKernelPortAllocateCB->setEnabled(CurrentConfig.pluginConfig.v2rayIntegration);
    }
    // Connection Settings
    {
        bypassCNCb->setChecked(CurrentConfig.defaultRouteConfig.connectionConfig.bypassCN);
        bypassBTCb->setChecked(CurrentConfig.defaultRouteConfig.connectionConfig.bypassBT);
        proxyDefaultCb->setChecked(!CurrentConfig.defaultRouteConfig.connectionConfig.enableProxy);
        bypassPrivateCb->setChecked(CurrentConfig.defaultRouteConfig.connectionConfig.bypassLAN);
    }
    //
    //
    latencyTCPingRB->setChecked(CurrentConfig.networkConfig.latencyTestingMethod == TCPING);
    latencyICMPingRB->setChecked(CurrentConfig.networkConfig.latencyTestingMethod == ICMPING);
    latencyRealPingTestURLTxt->setText(CurrentConfig.networkConfig.latencyRealPingTestURL);
    //
    {
        qvProxyPortCB->setValue(CurrentConfig.networkConfig.port);
        qvProxyAddressTxt->setText(CurrentConfig.networkConfig.address);
        qvProxyTypeCombo->setCurrentText(CurrentConfig.networkConfig.type);
        qvNetworkUATxt->setEditText(CurrentConfig.networkConfig.userAgent);
        //
        qvProxyNoProxy->setChecked(CurrentConfig.networkConfig.proxyType == Qv2rayConfig_Network::QVPROXY_NONE);
        qvProxySystemProxy->setChecked(CurrentConfig.networkConfig.proxyType == Qv2rayConfig_Network::QVPROXY_SYSTEM);
        qvProxyCustomProxy->setChecked(CurrentConfig.networkConfig.proxyType == Qv2rayConfig_Network::QVPROXY_CUSTOM);
        SET_PROXY_UI_ENABLE(CurrentConfig.networkConfig.proxyType == Qv2rayConfig_Network::QVPROXY_CUSTOM)
    }
    //
    //
    //
    // Advanced config.
    {
        setTestLatencyCB->setChecked(CurrentConfig.advancedConfig.testLatencyPeriodically);
        setTestLatencyOnConnectedCB->setChecked(CurrentConfig.advancedConfig.testLatencyOnConnected);
        disableSystemRootCB->setChecked(CurrentConfig.advancedConfig.disableSystemRoot);
    }
    //
    {
        dnsSettingsWidget = new DnsSettingsWidget(this);
        dnsSettingsWidget->SetDNSObject(CurrentConfig.defaultRouteConfig.dnsConfig, CurrentConfig.defaultRouteConfig.fakeDNSConfig);
        dnsSettingsLayout->addWidget(dnsSettingsWidget);
        //
        routeSettingsWidget = new RouteSettingsMatrixWidget(CurrentConfig.kernelConfig.AssetsPath(), this);
        routeSettingsWidget->SetRouteConfig(CurrentConfig.defaultRouteConfig.routeConfig);
        advRouteSettingsLayout->addWidget(routeSettingsWidget);
    }
    //
#ifdef DISABLE_AUTO_UPDATE
    updateSettingsGroupBox->setEnabled(false);
    updateSettingsGroupBox->setToolTip(tr("Update is disabled by your vendor."));
#endif
    //
    updateChannelCombo->setCurrentIndex(CurrentConfig.updateConfig.updateChannel);
    cancelIgnoreVersionBtn->setEnabled(!CurrentConfig.updateConfig.ignoredVersion.isEmpty());
    ignoredNextVersion->setText(CurrentConfig.updateConfig.ignoredVersion);
    //
    //
    {
        noAutoConnectRB->setChecked(CurrentConfig.autoStartBehavior == AUTO_CONNECTION_NONE);
        lastConnectedRB->setChecked(CurrentConfig.autoStartBehavior == AUTO_CONNECTION_LAST_CONNECTED);
        fixedAutoConnectRB->setChecked(CurrentConfig.autoStartBehavior == AUTO_CONNECTION_FIXED);
        //
        SET_AUTOSTART_UI_ENABLED(CurrentConfig.autoStartBehavior == AUTO_CONNECTION_FIXED);
        //
        if (CurrentConfig.autoStartId.isEmpty())
        {
            CurrentConfig.autoStartId.groupId = DefaultGroupId;
        }
        //
        const auto &autoStartConnId = CurrentConfig.autoStartId.connectionId;
        const auto &autoStartGroupId = CurrentConfig.autoStartId.groupId;
        //
        for (const auto &group : ConnectionManager->AllGroups()) //
            autoStartSubsCombo->addItem(GetDisplayName(group), group.toString());

        autoStartSubsCombo->setCurrentText(GetDisplayName(autoStartGroupId));

        for (const auto &conn : ConnectionManager->GetConnections(autoStartGroupId))
            autoStartConnCombo->addItem(GetDisplayName(conn), conn.toString());

        autoStartConnCombo->setCurrentText(GetDisplayName(autoStartConnId));
    }
    // FP Settings
    if (CurrentConfig.defaultRouteConfig.forwardProxyConfig.type.trimmed().isEmpty())
    {
        CurrentConfig.defaultRouteConfig.forwardProxyConfig.type = "http";
    }

    fpGroupBox->setChecked(CurrentConfig.defaultRouteConfig.forwardProxyConfig.enableForwardProxy);
    fpUsernameTx->setText(CurrentConfig.defaultRouteConfig.forwardProxyConfig.username);
    fpPasswordTx->setText(CurrentConfig.defaultRouteConfig.forwardProxyConfig.password);
    fpAddressTx->setText(CurrentConfig.defaultRouteConfig.forwardProxyConfig.serverAddress);
    fpTypeCombo->setCurrentText(CurrentConfig.defaultRouteConfig.forwardProxyConfig.type);
    fpPortSB->setValue(CurrentConfig.defaultRouteConfig.forwardProxyConfig.port);
    fpUseAuthCB->setChecked(CurrentConfig.defaultRouteConfig.forwardProxyConfig.useAuth);
    fpUsernameTx->setEnabled(fpUseAuthCB->isChecked());
    fpPasswordTx->setEnabled(fpUseAuthCB->isChecked());
    //
    maxLogLinesSB->setValue(CurrentConfig.uiConfig.maximumLogLines);
    jumpListCountSB->setValue(CurrentConfig.uiConfig.maxJumpListCount);
    //
    setSysProxyCB->setChecked(CurrentConfig.inboundConfig.systemProxySettings.setSystemProxy);
    //
    finishedLoading = true;
}

QvMessageBusSlotImpl(PreferencesWindow)
{
    switch (msg)
    {
        MBShowDefaultImpl;
        MBHideDefaultImpl;
        MBRetranslateDefaultImpl;
        case UPDATE_COLORSCHEME: break;
    }
}

PreferencesWindow::~PreferencesWindow(){};

std::optional<QString> PreferencesWindow::checkTProxySettings() const
{
    if (CurrentConfig.inboundConfig.useTPROXY)
    {
        if (!IsIPv4Address(CurrentConfig.inboundConfig.tProxySettings.tProxyIP))
        {
            return tr("Invalid tproxy listening ipv4 address.");
        }
        else if (CurrentConfig.inboundConfig.tProxySettings.tProxyV6IP != "" && !IsIPv6Address(CurrentConfig.inboundConfig.tProxySettings.tProxyV6IP))
        {
            return tr("Invalid tproxy listening ipv6 address.");
        }
    }
    return std::nullopt;
}

void PreferencesWindow::on_buttonBox_accepted()
{
    // Note:
    // A signal-slot connection from buttonbox_accpted to QDialog::accepted()
    // has been removed. To prevent closing this Dialog.
    QSet<int> ports;
    auto size = 0;

    { // kernel check scope
        auto vcorePath = vCorePathTxt->text();
        auto vAssetsPath = vCoreAssetsPathTxt->text();

#if QV2RAY_FEATURE(kernel_check_filename)
        // prevent some bullshit situations.
        if (const auto vCorePathSmallCased = vcorePath.toLower(); vCorePathSmallCased.endsWith("qv2ray") || vCorePathSmallCased.endsWith("qv2ray.exe"))
        {
            const auto content = tr("You may be about to set V2Ray core incorrectly to Qv2ray itself, which is absolutely not correct.\r\n"
                                    "This won't trigger a fork bomb, however, since Qv2ray works in singleton mode.\r\n"
                                    "If your V2Ray core filename happened to be 'qv2ray'-something, you are totally free to ignore this warning.");
            QvMessageBoxWarn(this, tr("Watch Out!"), content);
        }
        else if (vCorePathSmallCased.endsWith("v2ctl") || vCorePathSmallCased.endsWith("v2ctl.exe"))
        {
            const auto content = tr("You may be about to set V2Ray core incorrectly to V2Ray Control executable, which is absolutely not correct.\r\n"
                                    "The filename of V2Ray core is usually 'v2ray' or 'v2ray.exe'. Make sure to choose it wisely.\r\n"
                                    "If you insist to proceed, we're not providing with any support.");
            QvMessageBoxWarn(this, tr("Watch Out!"), content);
        }
#endif

        if (const auto &&[result, msg] = V2RayKernelInstance::ValidateVersionedKernel(vcorePath, vAssetsPath); !result)
        {
            QvMessageBoxWarn(this, tr("V2Ray Core Settings"), *msg);
        }
#if QV2RAY_FEATURE(kernel_check_output)
        else if (!msg->toLower().contains("v2ray") && !msg->toLower().contains("xray"))
        {
            const auto content = tr("This does not seem like an output from V2Ray Core.") + NEWLINE +                         //
                                 tr("If you are looking for plugins settings, you should go to plugin settings.") + NEWLINE + //
                                 tr("Output:") + NEWLINE +                                                                    //
                                 NEWLINE + *msg;
            QvMessageBoxWarn(this, tr("'V2Ray Core' Settings"), content);
        }
#endif
        // no else branch
    }

    if (CurrentConfig.inboundConfig.useHTTP)
    {
        size++;
        ports << CurrentConfig.inboundConfig.httpSettings.port;
    }

    if (CurrentConfig.inboundConfig.useSocks)
    {
        size++;
        ports << CurrentConfig.inboundConfig.socksSettings.port;
    }

    if (CurrentConfig.inboundConfig.useTPROXY)
    {
        size++;
        ports << CurrentConfig.inboundConfig.tProxySettings.port;
    }

    if (!QvCoreApplication->StartupArguments.noAPI)
    {
        size++;
        ports << CurrentConfig.kernelConfig.statsPort;
    }

    if (ports.size() != size)
    {
        // Duplicates detected.
        QvMessageBoxWarn(this, tr("Preferences"), tr("Duplicated port numbers detected, please check the port number settings."));
    }
    else if (!IsValidIPAddress(CurrentConfig.inboundConfig.listenip))
    {
        QvMessageBoxWarn(this, tr("Preferences"), tr("Invalid inbound listening address."));
    }
    else if (const auto err = checkTProxySettings(); err.has_value())
    {
        QvMessageBoxWarn(this, tr("Preferences"), *err);
    }
    else if (!dnsSettingsWidget->CheckIsValidDNS())
    {
        QvMessageBoxWarn(this, tr("Preferences"), tr("Invalid DNS settings."));
    }
    else
    {
        if (CurrentConfig.uiConfig.language != GlobalConfig.uiConfig.language)
        {
            // Install translator
            if (Qv2rayTranslator->InstallTranslation(CurrentConfig.uiConfig.language))
            {
                UIMessageBus.EmitGlobalSignal(QvMBMessage::RETRANSLATE);
                QApplication::processEvents();
            }
        }
        CurrentConfig.defaultRouteConfig.routeConfig = routeSettingsWidget->GetRouteConfig();
        if (!(CurrentConfig.defaultRouteConfig.routeConfig == GlobalConfig.defaultRouteConfig.routeConfig))
        {
            NEEDRESTART
        }
        const auto &[dns, fakedns] = dnsSettingsWidget->GetDNSObject();
        CurrentConfig.defaultRouteConfig.dnsConfig = dns;
        CurrentConfig.defaultRouteConfig.fakeDNSConfig = fakedns;
        if (!(CurrentConfig.defaultRouteConfig.dnsConfig == GlobalConfig.defaultRouteConfig.dnsConfig))
        {
            NEEDRESTART
        }
        //
        //
        if (CurrentConfig.uiConfig.theme != GlobalConfig.uiConfig.theme)
        {
            StyleManager->ApplyStyle(CurrentConfig.uiConfig.theme);
        }
        GlobalConfig.loadJson(CurrentConfig.toJson());
        SaveGlobalSettings();
        UIMessageBus.EmitGlobalSignal(QvMBMessage::UPDATE_COLORSCHEME);
        if (NeedRestart && !KernelInstance->CurrentConnection().isEmpty())
        {
            const auto message = tr("You may need to reconnect to apply the settings now.") + NEWLINE +              //
                                 tr("Otherwise they will be applied next time you connect to a server.") + NEWLINE + //
                                 NEWLINE +                                                                           //
                                 tr("Do you want to reconnect now?");
            const auto askResult = QvMessageBoxAsk(this, tr("Reconnect Required"), message);
            if (askResult == Yes)
            {
                ConnectionManager->RestartConnection();
                this->setEnabled(false);
            }
        }
        accept();
    }
}

void PreferencesWindow::on_httpAuthCB_stateChanged(int checked)
{
    NEEDRESTART
    bool enabled = checked == Qt::Checked;
    httpAuthUsernameTxt->setEnabled(enabled);
    httpAuthPasswordTxt->setEnabled(enabled);
    CurrentConfig.inboundConfig.httpSettings.useAuth = enabled;
}

void PreferencesWindow::on_socksAuthCB_stateChanged(int checked)
{
    NEEDRESTART
    bool enabled = checked == Qt::Checked;
    socksAuthUsernameTxt->setEnabled(enabled);
    socksAuthPasswordTxt->setEnabled(enabled);
    CurrentConfig.inboundConfig.socksSettings.useAuth = enabled;
}

void PreferencesWindow::on_languageComboBox_currentTextChanged(const QString &arg1)
{
    LOADINGCHECK
    CurrentConfig.uiConfig.language = arg1;
}

void PreferencesWindow::on_logLevelComboBox_currentIndexChanged(int index)
{
    NEEDRESTART
    CurrentConfig.logLevel = index;
}

void PreferencesWindow::on_vCoreAssetsPathTxt_textEdited(const QString &arg1)
{
    NEEDRESTART
    CurrentConfig.kernelConfig.AssetsPath(arg1);
}

void PreferencesWindow::on_listenIPTxt_textEdited(const QString &arg1)
{
    NEEDRESTART
    CurrentConfig.inboundConfig.listenip = arg1;

    if (arg1 == "" || IsValidIPAddress(arg1))
    {
        BLACK(listenIPTxt);
    }
    else
    {
        RED(listenIPTxt);
    }

    // pacAccessPathTxt->setText("http://" + arg1 + ":" +
    // QSTRN(pacPortSB->value()) + "/pac");
}

void PreferencesWindow::on_httpAuthUsernameTxt_textEdited(const QString &arg1)
{
    NEEDRESTART
    CurrentConfig.inboundConfig.httpSettings.account.user = arg1;
}

void PreferencesWindow::on_httpAuthPasswordTxt_textEdited(const QString &arg1)
{
    NEEDRESTART
    CurrentConfig.inboundConfig.httpSettings.account.pass = arg1;
}

void PreferencesWindow::on_socksAuthUsernameTxt_textEdited(const QString &arg1)
{
    NEEDRESTART
    CurrentConfig.inboundConfig.socksSettings.account.user = arg1;
}

void PreferencesWindow::on_socksAuthPasswordTxt_textEdited(const QString &arg1)
{
    NEEDRESTART
    CurrentConfig.inboundConfig.socksSettings.account.pass = arg1;
}

void PreferencesWindow::on_latencyRealPingTestURLTxt_textEdited(const QString &arg1)
{
    CurrentConfig.networkConfig.latencyRealPingTestURL = arg1;
}

void PreferencesWindow::on_proxyDefaultCb_stateChanged(int arg1)
{
    NEEDRESTART
    CurrentConfig.defaultRouteConfig.connectionConfig.enableProxy = !(arg1 == Qt::Checked);
}

void PreferencesWindow::on_selectVAssetBtn_clicked()
{
    NEEDRESTART
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open V2Ray assets folder"), QDir::currentPath());

    if (!dir.isEmpty())
    {
        vCoreAssetsPathTxt->setText(dir);
        on_vCoreAssetsPathTxt_textEdited(dir);
    }
}

void PreferencesWindow::on_selectVCoreBtn_clicked()
{
    QString core = QFileDialog::getOpenFileName(this, tr("Open V2Ray core file"), QDir::currentPath());

    if (!core.isEmpty())
    {
        vCorePathTxt->setText(core);
        on_vCorePathTxt_textEdited(core);
    }
}

void PreferencesWindow::on_vCorePathTxt_textEdited(const QString &arg1)
{
    NEEDRESTART
    CurrentConfig.kernelConfig.KernelPath(arg1);
}

void PreferencesWindow::on_aboutQt_clicked()
{
    QApplication::aboutQt();
}

void PreferencesWindow::on_cancelIgnoreVersionBtn_clicked()
{
    CurrentConfig.updateConfig.ignoredVersion.clear();
    cancelIgnoreVersionBtn->setEnabled(false);
}

void PreferencesWindow::on_bypassCNCb_stateChanged(int arg1)
{
    NEEDRESTART
    CurrentConfig.defaultRouteConfig.connectionConfig.bypassCN = arg1 == Qt::Checked;
}

void PreferencesWindow::on_bypassBTCb_stateChanged(int arg1)
{
    NEEDRESTART
    if (arg1 == Qt::Checked)
    {
        QvMessageBoxInfo(this, tr("Note"),
                         tr("To recognize the protocol of a connection, one must enable sniffing option in inbound proxy.") + NEWLINE +
                             tr("tproxy inbound's sniffing is enabled by default."));
    }
    CurrentConfig.defaultRouteConfig.connectionConfig.bypassBT = arg1 == Qt::Checked;
}

void PreferencesWindow::on_statsPortBox_valueChanged(int arg1)
{
    NEEDRESTART
    CurrentConfig.kernelConfig.statsPort = arg1;
}

void PreferencesWindow::on_socksPortLE_valueChanged(int arg1)
{
    NEEDRESTART
    CurrentConfig.inboundConfig.socksSettings.port = arg1;
}

void PreferencesWindow::on_httpPortLE_valueChanged(int arg1)
{
    NEEDRESTART
    CurrentConfig.inboundConfig.httpSettings.port = arg1;
}

void PreferencesWindow::on_socksUDPCB_stateChanged(int arg1)
{
    NEEDRESTART
    CurrentConfig.inboundConfig.socksSettings.enableUDP = arg1 == Qt::Checked;
    socksUDPIP->setEnabled(arg1 == Qt::Checked);
}

void PreferencesWindow::on_socksUDPIP_textEdited(const QString &arg1)
{
    NEEDRESTART
    CurrentConfig.inboundConfig.socksSettings.localIP = arg1;

    if (IsValidIPAddress(arg1))
    {
        BLACK(socksUDPIP);
    }
    else
    {
        RED(socksUDPIP);
    }
}

void PreferencesWindow::on_themeCombo_currentTextChanged(const QString &arg1)
{
    LOADINGCHECK
    CurrentConfig.uiConfig.theme = arg1;
}

void PreferencesWindow::on_darkThemeCB_stateChanged(int arg1)
{
    LOADINGCHECK
    CurrentConfig.uiConfig.useDarkTheme = arg1 == Qt::Checked;
}

void PreferencesWindow::on_darkTrayCB_stateChanged(int arg1)
{
    LOADINGCHECK
    CurrentConfig.uiConfig.useDarkTrayIcon = arg1 == Qt::Checked;
}

void PreferencesWindow::on_glyphTrayCB_stateChanged(int arg1)
{
    LOADINGCHECK
    CurrentConfig.uiConfig.useGlyphTrayIcon = arg1 == Qt::Checked;
}

void PreferencesWindow::on_setSysProxyCB_stateChanged(int arg1)
{
    LOADINGCHECK
    NEEDRESTART
    CurrentConfig.inboundConfig.systemProxySettings.setSystemProxy = arg1 == Qt::Checked;
}

void PreferencesWindow::on_autoStartSubsCombo_currentIndexChanged(int arg1)
{
    LOADINGCHECK
    if (arg1 == -1)
    {
        CurrentConfig.autoStartId.clear();
        autoStartConnCombo->clear();
    }
    else
    {
        auto list = ConnectionManager->GetConnections(GroupId(autoStartSubsCombo->currentData().toString()));
        autoStartConnCombo->clear();

        for (const auto &id : list)
        {
            autoStartConnCombo->addItem(GetDisplayName(id), id.toString());
        }
    }
}

void PreferencesWindow::on_autoStartConnCombo_currentIndexChanged(int arg1)
{
    LOADINGCHECK
    if (arg1 == -1)
    {
        CurrentConfig.autoStartId.clear();
    }
    else
    {
        CurrentConfig.autoStartId.groupId = GroupId(autoStartSubsCombo->currentData().toString());
        CurrentConfig.autoStartId.connectionId = ConnectionId(autoStartConnCombo->currentData().toString());
    }
}

void PreferencesWindow::on_startWithLoginCB_stateChanged(int arg1)
{
    bool isEnabled = arg1 == Qt::Checked;
    SetLaunchAtLoginStatus(isEnabled);

    if (GetLaunchAtLoginStatus() != isEnabled)
    {
        QvMessageBoxWarn(this, tr("Start with boot"), tr("Failed to set auto start option."));
    }

    SetAutoStartButtonsState(GetLaunchAtLoginStatus());
}

void PreferencesWindow::SetAutoStartButtonsState(bool isAutoStart)
{
    startWithLoginCB->setChecked(isAutoStart);
}

void PreferencesWindow::on_fpTypeCombo_currentIndexChanged(int arg1)
{
    LOADINGCHECK
    CurrentConfig.defaultRouteConfig.forwardProxyConfig.type = fpTypeCombo->itemText(arg1).toLower();
}

void PreferencesWindow::on_fpAddressTx_textEdited(const QString &arg1)
{
    LOADINGCHECK
    CurrentConfig.defaultRouteConfig.forwardProxyConfig.serverAddress = arg1;

    if (IsValidIPAddress(arg1))
    {
        BLACK(fpAddressTx);
    }
    else
    {
        RED(fpAddressTx);
    }
}

void PreferencesWindow::on_fpUseAuthCB_stateChanged(int arg1)
{
    bool authEnabled = arg1 == Qt::Checked;
    CurrentConfig.defaultRouteConfig.forwardProxyConfig.useAuth = authEnabled;
    fpUsernameTx->setEnabled(authEnabled);
    fpPasswordTx->setEnabled(authEnabled);
}

void PreferencesWindow::on_fpUsernameTx_textEdited(const QString &arg1)
{
    LOADINGCHECK
    CurrentConfig.defaultRouteConfig.forwardProxyConfig.username = arg1;
}

void PreferencesWindow::on_fpPasswordTx_textEdited(const QString &arg1)
{
    LOADINGCHECK
    CurrentConfig.defaultRouteConfig.forwardProxyConfig.password = arg1;
}

void PreferencesWindow::on_fpPortSB_valueChanged(int arg1)
{
    LOADINGCHECK
    CurrentConfig.defaultRouteConfig.forwardProxyConfig.port = arg1;
}

void PreferencesWindow::on_checkVCoreSettings_clicked()
{
    auto vcorePath = vCorePathTxt->text();
    auto vAssetsPath = vCoreAssetsPathTxt->text();

#if QV2RAY_FEATURE(kernel_check_filename)
    // prevent some bullshit situations.
    if (const auto vCorePathSmallCased = vcorePath.toLower(); vCorePathSmallCased.endsWith("qv2ray") || vCorePathSmallCased.endsWith("qv2ray.exe"))
    {
        const auto content = tr("You may be about to set V2Ray core incorrectly to Qv2ray itself, which is absolutely not correct.\r\n"
                                "This won't trigger a fork bomb, however, since Qv2ray works in singleton mode.\r\n"
                                "If your V2Ray core filename happened to be 'qv2ray'-something, you are totally free to ignore this warning.");
        QvMessageBoxWarn(this, tr("Watch Out!"), content);
    }
#if !defined(QV2RAY_USE_V5_CORE)
    else if (vCorePathSmallCased.endsWith("v2ctl") || vCorePathSmallCased.endsWith("v2ctl.exe"))
    {
        const auto content = tr("You may be about to set V2Ray core incorrectly to V2Ray Control executable, which is absolutely not correct.\r\n"
                                "The filename of V2Ray core is usually 'v2ray' or 'v2ray.exe'. Make sure to choose it wisely.\r\n"
                                "If you insist to proceed, we're not providing with any support.");
        QvMessageBoxWarn(this, tr("Watch Out!"), content);
    }
#endif
#endif

    if (const auto &&[result, msg] = V2RayKernelInstance::ValidateVersionedKernel(vcorePath, vAssetsPath); !result)
    {
        QvMessageBoxWarn(this, tr("V2Ray Core Settings"), *msg);
    }
#if QV2RAY_FEATURE(kernel_check_output)
    else if (!msg->toLower().contains("v2ray") && !msg->toLower().contains("xray"))
    {
        const auto content = tr("This does not seem like an output from V2Ray Core.") + NEWLINE +                         //
                             tr("If you are looking for plugins settings, you should go to plugin settings.") + NEWLINE + //
                             tr("Output:") + NEWLINE +                                                                    //
                             NEWLINE + *msg;
        QvMessageBoxWarn(this, tr("'V2Ray Core' Settings"), content);
    }
#endif
    else
    {
        const auto content = tr("V2Ray path configuration check passed.") + NEWLINE + NEWLINE + tr("Current version of V2Ray is: ") + NEWLINE + *msg;
        QvMessageBoxInfo(this, tr("V2Ray Core Settings"), content);
    }
}

void PreferencesWindow::on_httpGroupBox_clicked(bool checked)
{
    LOADINGCHECK
    NEEDRESTART
    CurrentConfig.inboundConfig.useHTTP = checked;
    httpAuthUsernameTxt->setEnabled(checked && CurrentConfig.inboundConfig.httpSettings.useAuth);
    httpAuthPasswordTxt->setEnabled(checked && CurrentConfig.inboundConfig.httpSettings.useAuth);
    httpOverrideHTTPCB->setEnabled(checked && CurrentConfig.inboundConfig.httpSettings.sniffing);
    httpOverrideTLSCB->setEnabled(checked && CurrentConfig.inboundConfig.httpSettings.sniffing);
    httpOverrideFakeDNSCB->setEnabled(checked && CurrentConfig.inboundConfig.httpSettings.sniffing);
    httpOverrideFakeDNSOthersCB->setEnabled(checked && CurrentConfig.inboundConfig.httpSettings.sniffing);
}

void PreferencesWindow::on_socksGroupBox_clicked(bool checked)
{
    LOADINGCHECK
    NEEDRESTART
    CurrentConfig.inboundConfig.useSocks = checked;
    socksUDPIP->setEnabled(checked && CurrentConfig.inboundConfig.socksSettings.enableUDP);
    socksAuthUsernameTxt->setEnabled(checked && CurrentConfig.inboundConfig.socksSettings.useAuth);
    socksAuthPasswordTxt->setEnabled(checked && CurrentConfig.inboundConfig.socksSettings.useAuth);
    socksOverrideHTTPCB->setEnabled(checked && CurrentConfig.inboundConfig.socksSettings.sniffing);
    socksOverrideTLSCB->setEnabled(checked && CurrentConfig.inboundConfig.socksSettings.sniffing);
    socksOverrideFakeDNSCB->setEnabled(checked && CurrentConfig.inboundConfig.socksSettings.sniffing);
    socksOverrideFakeDNSOthersCB->setEnabled(checked && CurrentConfig.inboundConfig.socksSettings.sniffing);
}

void PreferencesWindow::on_fpGroupBox_clicked(bool checked)
{
    LOADINGCHECK
    NEEDRESTART
    CurrentConfig.defaultRouteConfig.forwardProxyConfig.enableForwardProxy = checked;
}

void PreferencesWindow::on_maxLogLinesSB_valueChanged(int arg1)
{
    LOADINGCHECK
    NEEDRESTART
    CurrentConfig.uiConfig.maximumLogLines = arg1;
}

void PreferencesWindow::on_enableAPI_stateChanged(int arg1)
{
    LOADINGCHECK
    NEEDRESTART
    CurrentConfig.kernelConfig.enableAPI = arg1 == Qt::Checked;
}

void PreferencesWindow::on_updateChannelCombo_currentIndexChanged(int index)
{
    LOADINGCHECK
    CurrentConfig.updateConfig.updateChannel = (Qv2rayConfig_Update::UpdateChannel) index;
    CurrentConfig.updateConfig.ignoredVersion.clear();
}

void PreferencesWindow::on_pluginKernelV2RayIntegrationCB_stateChanged(int arg1)
{
    LOADINGCHECK
    if (KernelInstance->ActivePluginKernelsCount() > 0)
        NEEDRESTART;
    CurrentConfig.pluginConfig.v2rayIntegration = arg1 == Qt::Checked;
    pluginKernelPortAllocateCB->setEnabled(arg1 == Qt::Checked);
}

void PreferencesWindow::on_pluginKernelPortAllocateCB_valueChanged(int arg1)
{
    LOADINGCHECK
    if (KernelInstance->ActivePluginKernelsCount() > 0)
        NEEDRESTART;
    CurrentConfig.pluginConfig.portAllocationStart = arg1;
}

void PreferencesWindow::on_qvProxyAddressTxt_textEdited(const QString &arg1)
{
    LOADINGCHECK
    CurrentConfig.networkConfig.address = arg1;
}

void PreferencesWindow::on_qvProxyTypeCombo_currentTextChanged(const QString &arg1)
{
    LOADINGCHECK
    CurrentConfig.networkConfig.type = arg1.toLower();
}

void PreferencesWindow::on_qvProxyPortCB_valueChanged(int arg1)
{
    LOADINGCHECK
    CurrentConfig.networkConfig.port = arg1;
}

void PreferencesWindow::on_setTestlatencyCB_stateChanged(int arg1)
{
    LOADINGCHECK
    if (arg1 == Qt::Checked)
    {
        QvMessageBoxWarn(this, tr("Dangerous Operation"), tr("This will (probably) make it easy to fingerprint your connection."));
    }
    CurrentConfig.advancedConfig.testLatencyPeriodically = arg1 == Qt::Checked;
}

void PreferencesWindow::on_setTestlatencyOnConnectedCB_stateChanged(int arg1)
{
    LOADINGCHECK
    if (arg1 == Qt::Checked)
    {
        QvMessageBoxWarn(this, tr("Dangerous Operation"), tr("This will (probably) make it easy to fingerprint your connection."));
    }
    CurrentConfig.advancedConfig.testLatencyOnConnected = arg1 == Qt::Checked;
}

void PreferencesWindow::on_quietModeCB_stateChanged(int arg1)
{
    LOADINGCHECK
    CurrentConfig.uiConfig.quietMode = arg1 == Qt::Checked;
}

void PreferencesWindow::on_tproxyGroupBox_toggled(bool arg1)
{
    LOADINGCHECK
    NEEDRESTART
    CurrentConfig.inboundConfig.useTPROXY = arg1;
    tproxyOverrideHTTPCB->setEnabled(arg1 && CurrentConfig.inboundConfig.tProxySettings.sniffing);
    tproxyOverrideTLSCB->setEnabled(arg1 && CurrentConfig.inboundConfig.tProxySettings.sniffing);
    tproxyOverrideFakeDNSCB->setEnabled(arg1 && CurrentConfig.inboundConfig.tProxySettings.sniffing);
    tproxyOverrideFakeDNSOthersCB->setEnabled(arg1 && CurrentConfig.inboundConfig.tProxySettings.sniffing);
}

void PreferencesWindow::on_tProxyPort_valueChanged(int arg1)
{
    LOADINGCHECK
    NEEDRESTART
    CurrentConfig.inboundConfig.tProxySettings.port = arg1;
}

void PreferencesWindow::on_tproxyEnableTCP_toggled(bool checked)
{
    LOADINGCHECK
    NEEDRESTART
    CurrentConfig.inboundConfig.tProxySettings.hasTCP = checked;
}

void PreferencesWindow::on_tproxyEnableUDP_toggled(bool checked)
{
    LOADINGCHECK
    NEEDRESTART
    CurrentConfig.inboundConfig.tProxySettings.hasUDP = checked;
}

void PreferencesWindow::on_tproxySniffingCB_stateChanged(int arg1)
{
    LOADINGCHECK
    NEEDRESTART
    CurrentConfig.inboundConfig.tProxySettings.sniffing = arg1 == Qt::Checked;
    tproxySniffingMetadataOnlyCB->setEnabled(arg1 == Qt::Checked);
    tproxyOverrideHTTPCB->setEnabled(arg1 == Qt::Checked);
    tproxyOverrideTLSCB->setEnabled(arg1 == Qt::Checked);
    tproxyOverrideFakeDNSCB->setEnabled(arg1 == Qt::Checked);
    tproxyOverrideFakeDNSOthersCB->setEnabled(arg1 == Qt::Checked);
}

void PreferencesWindow::on_tproxyOverrideHTTPCB_stateChanged(int arg1)
{
    NEEDRESTART
    if (arg1 != Qt::Checked)
        CurrentConfig.inboundConfig.tProxySettings.destOverride.removeAll("http");
    else if (!CurrentConfig.inboundConfig.tProxySettings.destOverride.contains("http"))
        CurrentConfig.inboundConfig.tProxySettings.destOverride.append("http");
}

void PreferencesWindow::on_tproxyOverrideTLSCB_stateChanged(int arg1)
{
    NEEDRESTART
    if (arg1 != Qt::Checked)
        CurrentConfig.inboundConfig.tProxySettings.destOverride.removeAll("tls");
    else if (!CurrentConfig.inboundConfig.tProxySettings.destOverride.contains("tls"))
        CurrentConfig.inboundConfig.tProxySettings.destOverride.append("tls");
}

void PreferencesWindow::on_tproxyMode_currentTextChanged(const QString &arg1)
{
    NEEDRESTART
    CurrentConfig.inboundConfig.tProxySettings.mode = arg1;
}

void PreferencesWindow::on_tproxyListenAddr_textEdited(const QString &arg1)
{
    NEEDRESTART
    CurrentConfig.inboundConfig.tProxySettings.tProxyIP = arg1;
    if (arg1 == "" || IsIPv4Address(arg1))
        BLACK(tproxyListenAddr);
    else
        RED(tproxyListenAddr);
}

void PreferencesWindow::on_tproxyListenV6Addr_textEdited(const QString &arg1)
{
    NEEDRESTART
    CurrentConfig.inboundConfig.tProxySettings.tProxyV6IP = arg1;
    if (arg1 == "" || IsIPv6Address(arg1))
        BLACK(tproxyListenV6Addr);
    else
        RED(tproxyListenV6Addr);
}

void PreferencesWindow::on_jumpListCountSB_valueChanged(int arg1)
{
    LOADINGCHECK
    CurrentConfig.uiConfig.maxJumpListCount = arg1;
}

void PreferencesWindow::on_outboundMark_valueChanged(int arg1)
{
    LOADINGCHECK
    NEEDRESTART
    CurrentConfig.outboundConfig.mark = arg1;
}

void PreferencesWindow::on_dnsIntercept_toggled(bool checked)
{
    LOADINGCHECK
    NEEDRESTART
    CurrentConfig.defaultRouteConfig.connectionConfig.dnsIntercept = checked;
}

void PreferencesWindow::on_qvProxyCustomProxy_clicked()
{
    CurrentConfig.networkConfig.proxyType = Qv2rayConfig_Network::QVPROXY_CUSTOM;
    SET_PROXY_UI_ENABLE(true);
    qvProxyNoProxy->setChecked(false);
    qvProxySystemProxy->setChecked(false);
    qvProxyCustomProxy->setChecked(true);
}

void PreferencesWindow::on_qvProxySystemProxy_clicked()
{
    CurrentConfig.networkConfig.proxyType = Qv2rayConfig_Network::QVPROXY_SYSTEM;
    SET_PROXY_UI_ENABLE(false);
    qvProxyNoProxy->setChecked(false);
    qvProxyCustomProxy->setChecked(false);
    qvProxySystemProxy->setChecked(true);
}

void PreferencesWindow::on_qvProxyNoProxy_clicked()
{
    CurrentConfig.networkConfig.proxyType = Qv2rayConfig_Network::QVPROXY_NONE;
    SET_PROXY_UI_ENABLE(false);
    qvProxySystemProxy->setChecked(false);
    qvProxyCustomProxy->setChecked(false);
    qvProxyNoProxy->setChecked(true);
}

void PreferencesWindow::on_dnsFreedomCb_stateChanged(int arg1)
{
    LOADINGCHECK
    NEEDRESTART
    CurrentConfig.defaultRouteConfig.connectionConfig.v2rayFreedomDNS = arg1 == Qt::Checked;
}

void PreferencesWindow::on_httpSniffingCB_stateChanged(int arg1)
{
    LOADINGCHECK
    NEEDRESTART
    CurrentConfig.inboundConfig.httpSettings.sniffing = arg1 == Qt::Checked;
    httpSniffingMetadataOnly->setEnabled(arg1 == Qt::Checked);
    httpOverrideHTTPCB->setEnabled(arg1 == Qt::Checked);
    httpOverrideTLSCB->setEnabled(arg1 == Qt::Checked);
    httpOverrideFakeDNSCB->setEnabled(arg1 == Qt::Checked);
    httpOverrideFakeDNSOthersCB->setEnabled(arg1 == Qt::Checked);
}

void PreferencesWindow::on_httpOverrideHTTPCB_stateChanged(int arg1)
{
    LOADINGCHECK
    NEEDRESTART
    if (arg1 != Qt::Checked)
        CurrentConfig.inboundConfig.httpSettings.destOverride.removeAll("http");
    else if (!CurrentConfig.inboundConfig.httpSettings.destOverride.contains("http"))
        CurrentConfig.inboundConfig.httpSettings.destOverride.append("http");
}

void PreferencesWindow::on_httpOverrideTLSCB_stateChanged(int arg1)
{
    LOADINGCHECK
    NEEDRESTART
    if (arg1 != Qt::Checked)
        CurrentConfig.inboundConfig.httpSettings.destOverride.removeAll("tls");
    else if (!CurrentConfig.inboundConfig.httpSettings.destOverride.contains("tls"))
        CurrentConfig.inboundConfig.httpSettings.destOverride.append("tls");
}

void PreferencesWindow::on_socksSniffingCB_stateChanged(int arg1)
{
    LOADINGCHECK
    NEEDRESTART
    CurrentConfig.inboundConfig.socksSettings.sniffing = arg1 == Qt::Checked;
    socksSniffingMetadataOnly->setEnabled(arg1 == Qt::Checked);
    socksOverrideHTTPCB->setEnabled(arg1 == Qt::Checked);
    socksOverrideTLSCB->setEnabled(arg1 == Qt::Checked);
    socksOverrideFakeDNSCB->setEnabled(arg1 == Qt::Checked);
    socksOverrideFakeDNSOthersCB->setEnabled(arg1 == Qt::Checked);
}

void PreferencesWindow::on_socksOverrideHTTPCB_stateChanged(int arg1)
{
    LOADINGCHECK
    NEEDRESTART
    if (arg1 != Qt::Checked)
        CurrentConfig.inboundConfig.socksSettings.destOverride.removeAll("http");
    else if (!CurrentConfig.inboundConfig.socksSettings.destOverride.contains("http"))
        CurrentConfig.inboundConfig.socksSettings.destOverride.append("http");
}

void PreferencesWindow::on_socksOverrideTLSCB_stateChanged(int arg1)
{
    LOADINGCHECK
    NEEDRESTART
    if (arg1 != Qt::Checked)
        CurrentConfig.inboundConfig.socksSettings.destOverride.removeAll("tls");
    else if (!CurrentConfig.inboundConfig.socksSettings.destOverride.contains("tls"))
        CurrentConfig.inboundConfig.socksSettings.destOverride.append("tls");
}

void PreferencesWindow::on_pushButton_clicked()
{
#if QV2RAY_FEATURE(util_has_ntp)
    const auto ntpTitle = tr("NTP Checker");
    const auto ntpHint = tr("Check date and time from server:");
    const static QStringList ntpServerList = { "cn.pool.ntp.org",      "cn.ntp.org.cn",           "edu.ntp.org.cn",
                                               "time.pool.aliyun.com", "time1.cloud.tencent.com", "ntp.neu.edu.cn" };
    bool ok = false;
    const auto ntpServer = QInputDialog::getItem(this, ntpTitle, ntpHint, ntpServerList, 0, true, &ok).trimmed();
    if (!ok)
        return;

    auto client = new ntp::NtpClient(this);
    connect(client, &ntp::NtpClient::replyReceived,
            [&](const QHostAddress &, quint16, const ntp::NtpReply &reply)
            {
                const int offsetSecTotal = reply.localClockOffset() / 1000;
                if (offsetSecTotal >= 90 || offsetSecTotal <= -90)
                {
                    const auto inaccurateWarning = tr("Your time offset is %1 seconds, which is too high.") + NEWLINE + //
                                                   tr("Please synchronize your system to use the VMess protocol.");
                    QvMessageBoxWarn(this, tr("Time Inaccurate"), inaccurateWarning.arg(offsetSecTotal));
                }
                else if (offsetSecTotal > 15 || offsetSecTotal < -15)
                {
                    const auto smallErrorWarning = tr("Your time offset is %1 seconds, which is a little high.") + NEWLINE + //
                                                   tr("VMess protocol may still work, but we suggest you synchronize your clock.");
                    QvMessageBoxInfo(this, tr("Time Somewhat Inaccurate"), smallErrorWarning.arg(offsetSecTotal));
                }
                else
                {
                    const auto accurateInfo = tr("Your time offset is %1 seconds, which looks good.") + NEWLINE + //
                                              tr("VMess protocol may not suffer from time inaccuracy.");
                    QvMessageBoxInfo(this, tr("Time Accurate"), accurateInfo.arg(offsetSecTotal));
                }
            });

    const auto hostInfo = QHostInfo::fromName(ntpServer);
    if (hostInfo.error() == QHostInfo::NoError)
        client->sendRequest(hostInfo.addresses().first(), 123);
    else
        QvMessageBoxWarn(this, ntpTitle, tr("Failed to lookup server: %1").arg(hostInfo.errorString()));
#else
    QvMessageBoxWarn(this, tr("No NTP Backend"), tr("Qv2ray was not built with NTP support."));
#endif
}

void PreferencesWindow::on_noAutoConnectRB_clicked()
{
    LOADINGCHECK
    CurrentConfig.autoStartBehavior = AUTO_CONNECTION_NONE;
    SET_AUTOSTART_UI_ENABLED(false);
    SET_AUTOSTART_START_MINIMIZED_ENABLED(false);
}

void PreferencesWindow::on_lastConnectedRB_clicked()
{
    LOADINGCHECK
    CurrentConfig.autoStartBehavior = AUTO_CONNECTION_LAST_CONNECTED;
    SET_AUTOSTART_UI_ENABLED(false);
    SET_AUTOSTART_START_MINIMIZED_ENABLED(true);
}

void PreferencesWindow::on_fixedAutoConnectRB_clicked()
{
    LOADINGCHECK
    CurrentConfig.autoStartBehavior = AUTO_CONNECTION_FIXED;
    SET_AUTOSTART_UI_ENABLED(true);
    SET_AUTOSTART_START_MINIMIZED_ENABLED(true);
}

void PreferencesWindow::on_latencyTCPingRB_clicked()
{
    LOADINGCHECK
    CurrentConfig.networkConfig.latencyTestingMethod = TCPING;
    latencyICMPingRB->setChecked(false);
    latencyTCPingRB->setChecked(true);
}

void PreferencesWindow::on_latencyICMPingRB_clicked()
{
    LOADINGCHECK
    CurrentConfig.networkConfig.latencyTestingMethod = ICMPING;
    latencyICMPingRB->setChecked(true);
    latencyTCPingRB->setChecked(false);
}

void PreferencesWindow::on_qvNetworkUATxt_editTextChanged(const QString &arg1)
{
    LOADINGCHECK
    CurrentConfig.networkConfig.userAgent = arg1;
}

void PreferencesWindow::on_V2RayOutboundStatsCB_stateChanged(int arg1)
{
    hasDirectStatisticsCB->setEnabled(arg1 == Qt::Checked);
    LOADINGCHECK
    NEEDRESTART
    CurrentConfig.uiConfig.graphConfig.useOutboundStats = arg1 == Qt::Checked;
}

void PreferencesWindow::on_hasDirectStatisticsCB_stateChanged(int arg1)
{
    NEEDRESTART
    LOADINGCHECK
    CurrentConfig.uiConfig.graphConfig.hasDirectStats = arg1 == Qt::Checked;
}

void PreferencesWindow::on_useOldShareLinkFormatCB_stateChanged(int arg1)
{
    LOADINGCHECK
    CurrentConfig.uiConfig.useOldShareLinkFormat = arg1 == Qt::Checked;
}

void PreferencesWindow::on_bypassPrivateCb_clicked(bool checked)
{
    LOADINGCHECK
    NEEDRESTART
    CurrentConfig.defaultRouteConfig.connectionConfig.bypassLAN = checked;
}

void PreferencesWindow::on_disableSystemRootCB_stateChanged(int arg1)
{
    LOADINGCHECK
    CurrentConfig.advancedConfig.disableSystemRoot = arg1 == Qt::Checked;
}

void PreferencesWindow::on_openConfigDirCB_clicked()
{
    QvCoreApplication->OpenURL(QV2RAY_CONFIG_DIR);
}

void PreferencesWindow::on_startMinimizedCB_stateChanged(int arg1)
{
    LOADINGCHECK
    CurrentConfig.uiConfig.startMinimized = arg1 == Qt::Checked;
}

void PreferencesWindow::on_exitByCloseEventCB_stateChanged(int arg1)
{
    LOADINGCHECK
    CurrentConfig.uiConfig.exitByCloseEvent = arg1 == Qt::Checked;
}

void PreferencesWindow::on_httpSniffingMetadataOnly_stateChanged(int arg1)
{
    LOADINGCHECK
    NEEDRESTART
    CurrentConfig.inboundConfig.httpSettings.metadataOnly = arg1 == Qt::Checked;
}

void PreferencesWindow::on_socksSniffingMetadataOnly_stateChanged(int arg1)
{
    LOADINGCHECK
    NEEDRESTART
    CurrentConfig.inboundConfig.socksSettings.metadataOnly = arg1 == Qt::Checked;
}

void PreferencesWindow::on_tproxySniffingMetadataOnlyCB_stateChanged(int arg1)
{
    LOADINGCHECK
    NEEDRESTART
    CurrentConfig.inboundConfig.tProxySettings.metadataOnly = arg1 == Qt::Checked;
}

void PreferencesWindow::on_socksOverrideFakeDNSCB_stateChanged(int arg1)
{
    NEEDRESTART
    if (arg1 != Qt::Checked)
        CurrentConfig.inboundConfig.socksSettings.destOverride.removeAll("fakedns");
    else if (!CurrentConfig.inboundConfig.socksSettings.destOverride.contains("fakedns"))
        CurrentConfig.inboundConfig.socksSettings.destOverride.append("fakedns");
}

void PreferencesWindow::on_socksOverrideFakeDNSOthersCB_stateChanged(int arg1)
{
    NEEDRESTART
    if (arg1 != Qt::Checked)
        CurrentConfig.inboundConfig.socksSettings.destOverride.removeAll("fakedns+others");
    else if (!CurrentConfig.inboundConfig.socksSettings.destOverride.contains("fakedns+others"))
        CurrentConfig.inboundConfig.socksSettings.destOverride.append("fakedns+others");
}

void PreferencesWindow::on_httpOverrideFakeDNSCB_stateChanged(int arg1)
{
    NEEDRESTART
    if (arg1 != Qt::Checked)
        CurrentConfig.inboundConfig.httpSettings.destOverride.removeAll("fakedns");
    else if (!CurrentConfig.inboundConfig.httpSettings.destOverride.contains("fakedns"))
        CurrentConfig.inboundConfig.httpSettings.destOverride.append("fakedns");
}

void PreferencesWindow::on_httpOverrideFakeDNSOthersCB_stateChanged(int arg1)
{
    NEEDRESTART
    if (arg1 != Qt::Checked)
        CurrentConfig.inboundConfig.httpSettings.destOverride.removeAll("fakedns+others");
    else if (!CurrentConfig.inboundConfig.httpSettings.destOverride.contains("fakedns+others"))
        CurrentConfig.inboundConfig.httpSettings.destOverride.append("fakedns+others");
}

void PreferencesWindow::on_tproxyOverrideFakeDNSCB_stateChanged(int arg1)
{
    NEEDRESTART
    if (arg1 != Qt::Checked)
        CurrentConfig.inboundConfig.tProxySettings.destOverride.removeAll("fakedns");
    else if (!CurrentConfig.inboundConfig.tProxySettings.destOverride.contains("fakedns"))
        CurrentConfig.inboundConfig.tProxySettings.destOverride.append("fakedns");
}

void PreferencesWindow::on_tproxyOverrideFakeDNSOthersCB_stateChanged(int arg1)
{
    NEEDRESTART
    if (arg1 != Qt::Checked)
        CurrentConfig.inboundConfig.tProxySettings.destOverride.removeAll("fakedns+others");
    else if (!CurrentConfig.inboundConfig.tProxySettings.destOverride.contains("fakedns+others"))
        CurrentConfig.inboundConfig.tProxySettings.destOverride.append("fakedns+others");
}

void PreferencesWindow::on_browserForwarderAddressTxt_textEdited(const QString &arg1)
{
    NEEDRESTART
    CurrentConfig.inboundConfig.browserForwarderSettings.address = arg1;
}

void PreferencesWindow::on_browserForwarderPortSB_valueChanged(int arg1)
{
    NEEDRESTART
    CurrentConfig.inboundConfig.browserForwarderSettings.port = arg1;
}
