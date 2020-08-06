#include "w_PreferencesWindow.hpp"

#include "common/HTTPRequestHelper.hpp"
#include "common/QvHelpers.hpp"
#include "common/QvTranslator.hpp"
#include "components/autolaunch/QvAutoLaunch.hpp"
#include "components/ntp/QvNTPClient.hpp"
#include "core/connection/ConnectionIO.hpp"
#include "core/handler/ConfigHandler.hpp"
#include "core/kernel/V2RayKernelInteractions.hpp"
#include "core/settings/SettingsBackend.hpp"
#include "src/plugin-interface/QvPluginInterface.hpp"
#include "ui/styles/StyleManager.hpp"
#include "ui/widgets/DnsSettingsWidget.hpp"
#include "ui/widgets/RouteSettingsMatrix.hpp"

#include <QColorDialog>
#include <QCompleter>
#include <QDesktopServices>
#include <QFileDialog>
#include <QHostInfo>

using Qv2ray::common::validation::IsIPv4Address;
using Qv2ray::common::validation::IsIPv6Address;
using Qv2ray::common::validation::IsValidDNSServer;
using Qv2ray::common::validation::IsValidIPAddress;

#define LOADINGCHECK                                                                                                                            \
    if (!finishedLoading)                                                                                                                       \
        return;
#define NEEDRESTART                                                                                                                             \
    LOADINGCHECK                                                                                                                                \
    if (finishedLoading)                                                                                                                        \
        NeedRestart = true;

#define SET_PROXY_UI_ENABLE(_enabled)                                                                                                           \
    qvProxyTypeCombo->setEnabled(_enabled);                                                                                                     \
    qvProxyAddressTxt->setEnabled(_enabled);                                                                                                    \
    qvProxyPortCB->setEnabled(_enabled);

#define SET_AUTOSTART_UI_ENABLED(_enabled)                                                                                                      \
    autoStartConnCombo->setEnabled(_enabled);                                                                                                   \
    autoStartSubsCombo->setEnabled(_enabled);

PreferencesWindow::PreferencesWindow(QWidget *parent) : QvDialog(parent), CurrentConfig()
{
    setupUi(this);
    //
    tProxyCheckBox->setVisible(false);
    label_7->setVisible(false);
    //
    QvMessageBusConnect(PreferencesWindow);
    textBrowser->setHtml(StringFromFile(":/assets/credit.html"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

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
    qvPluginInterfaceVersionLabel->setText(tr("Version: %1").arg(QSTRN(QV2RAY_PLUGIN_INTERFACE_VERSION)));
    //
    // Deep copy
    CurrentConfig = GlobalConfig;
    //
    themeCombo->setCurrentText(CurrentConfig.uiConfig.theme);
    darkThemeCB->setChecked(CurrentConfig.uiConfig.useDarkTheme);
    darkTrayCB->setChecked(CurrentConfig.uiConfig.useDarkTrayIcon);
    languageComboBox->setCurrentText(CurrentConfig.uiConfig.language);
    logLevelComboBox->setCurrentIndex(CurrentConfig.logLevel);
    tProxyCheckBox->setChecked(CurrentConfig.tProxySupport);
    quietModeCB->setChecked(CurrentConfig.uiConfig.quietMode);
    useOldShareLinkFormatCB->setChecked(CurrentConfig.uiConfig.useOldShareLinkFormat);
    //
    //
    listenIPTxt->setText(CurrentConfig.inboundConfig.listenip);
    //
    bool have_http = CurrentConfig.inboundConfig.useHTTP;
    httpGroupBox->setChecked(have_http);
    httpPortLE->setValue(CurrentConfig.inboundConfig.httpSettings.port);
    httpAuthCB->setChecked(CurrentConfig.inboundConfig.httpSettings.useAuth);
    //
    httpAuthUsernameTxt->setEnabled(CurrentConfig.inboundConfig.httpSettings.useAuth);
    httpAuthPasswordTxt->setEnabled(CurrentConfig.inboundConfig.httpSettings.useAuth);
    httpAuthUsernameTxt->setText(CurrentConfig.inboundConfig.httpSettings.account.user);
    httpAuthPasswordTxt->setText(CurrentConfig.inboundConfig.httpSettings.account.pass);
    httpSniffingCB->setChecked(CurrentConfig.inboundConfig.httpSettings.sniffing);
    //
    //
    bool have_socks = CurrentConfig.inboundConfig.useSocks;
    socksGroupBox->setChecked(have_socks);
    socksPortLE->setValue(CurrentConfig.inboundConfig.socksSettings.port);
    //
    socksAuthCB->setChecked(CurrentConfig.inboundConfig.socksSettings.useAuth);
    socksAuthUsernameTxt->setEnabled(CurrentConfig.inboundConfig.socksSettings.useAuth);
    socksAuthPasswordTxt->setEnabled(CurrentConfig.inboundConfig.socksSettings.useAuth);
    socksAuthUsernameTxt->setText(CurrentConfig.inboundConfig.socksSettings.account.user);
    socksAuthPasswordTxt->setText(CurrentConfig.inboundConfig.socksSettings.account.pass);
    // Socks UDP Options
    socksUDPCB->setChecked(CurrentConfig.inboundConfig.socksSettings.enableUDP);
    socksUDPIP->setEnabled(CurrentConfig.inboundConfig.socksSettings.enableUDP);
    socksUDPIP->setText(CurrentConfig.inboundConfig.socksSettings.localIP);
    socksSniffingCB->setChecked(CurrentConfig.inboundConfig.socksSettings.sniffing);
    //
    //
    bool have_tproxy = CurrentConfig.inboundConfig.useTPROXY;
    tproxGroupBox->setChecked(have_tproxy);
    tproxyListenAddr->setText(CurrentConfig.inboundConfig.tProxySettings.tProxyIP);
    tproxyListenV6Addr->setText(CurrentConfig.inboundConfig.tProxySettings.tProxyV6IP);
    tProxyPort->setValue(CurrentConfig.inboundConfig.tProxySettings.port);
    tproxyEnableTCP->setChecked(CurrentConfig.inboundConfig.tProxySettings.hasTCP);
    tproxyEnableUDP->setChecked(CurrentConfig.inboundConfig.tProxySettings.hasUDP);
    tproxyMode->setCurrentText(CurrentConfig.inboundConfig.tProxySettings.mode);
    outboundMark->setValue(CurrentConfig.outboundConfig.mark);
#ifndef Q_OS_LINUX
    tproxGroupBox->setChecked(false);
    tproxGroupBox->setEnabled(false);
    tproxGroupBox->setToolTip(tr("tProxy is not supported on macOS and Windows"));
#endif
    dnsIntercept->setChecked(CurrentConfig.defaultRouteConfig.connectionConfig.dnsIntercept);
    DnsFreedomCb->setChecked(CurrentConfig.defaultRouteConfig.connectionConfig.v2rayFreedomDNS);
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
        //
        localDNSCb->setChecked(CurrentConfig.defaultRouteConfig.connectionConfig.withLocalDNS);
    }
    //
    //
    latencyTCPingRB->setChecked(CurrentConfig.networkConfig.latencyTestingMethod == TCPING);
    latencyICMPingRB->setChecked(CurrentConfig.networkConfig.latencyTestingMethod == ICMPING);
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
        setAllowInsecureCB->setChecked(CurrentConfig.advancedConfig.setAllowInsecure);
        setSessionResumptionCB->setChecked(CurrentConfig.advancedConfig.setSessionResumption);
        setTestLatenctCB->setChecked(CurrentConfig.advancedConfig.testLatencyPeriodcally);
    }
    //
    {
        dnsSettingsWidget = new DnsSettingsWidget(this);
        dnsSettingsWidget->SetDNSObject(CurrentConfig.defaultRouteConfig.dnsConfig);
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
        auto autoStartConnId = CurrentConfig.autoStartId.connectionId;
        auto autoStartGroupId = CurrentConfig.autoStartId.groupId;
        //
        for (const auto &group : ConnectionManager->AllGroups()) //
            autoStartSubsCombo->addItem(GetDisplayName(group), group.toString());

        autoStartSubsCombo->setCurrentText(GetDisplayName(autoStartGroupId));

        for (const auto &conn : ConnectionManager->Connections(autoStartGroupId))
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

PreferencesWindow::~PreferencesWindow()
{
    DEBUG(MODULE_UI, "Preference window destructor.")
}

void PreferencesWindow::on_buttonBox_accepted()
{
    // Note:
    // A signal-slot connection from buttonbox_accpted to QDialog::accepted()
    // has been removed. To prevent closing this Dialog.
    QSet<int> ports;
    auto size = 0;

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

    if (!StartupOption.noAPI)
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
    else if (!IsIPv4Address(CurrentConfig.inboundConfig.tProxySettings.tProxyIP))
    {
        QvMessageBoxWarn(this, tr("Preferences"), tr("Invalid tproxy listening ivp4 address."));
    }
    else if (CurrentConfig.inboundConfig.tProxySettings.tProxyV6IP != "" &&
             !IsIPv6Address(CurrentConfig.inboundConfig.tProxySettings.tProxyV6IP))
    {
        QvMessageBoxWarn(this, tr("Preferences"), tr("Invalid tproxy listening ipv6 address."));
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
            else
            {
                LOG(MODULE_UI, "Failed to translate UI to: " + CurrentConfig.uiConfig.language)
            }
        }
        CurrentConfig.defaultRouteConfig.routeConfig = routeSettingsWidget->GetRouteConfig();
        if (!(CurrentConfig.defaultRouteConfig.routeConfig == GlobalConfig.defaultRouteConfig.routeConfig))
        {
            NEEDRESTART
        }
        CurrentConfig.defaultRouteConfig.dnsConfig = dnsSettingsWidget->GetDNSObject();
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
        SaveGlobalSettings(CurrentConfig);
        UIMessageBus.EmitGlobalSignal(QvMBMessage::UPDATE_COLORSCHEME);
        if (NeedRestart && !KernelInstance->CurrentConnection().isEmpty())
        {
            this->setEnabled(false);
            qApp->processEvents();
            ConnectionManager->RestartConnection();
        }
        emit accept();
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
        BLACK(listenIPTxt)
    }
    else
    {
        RED(listenIPTxt)
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

void PreferencesWindow::on_proxyDefaultCb_stateChanged(int arg1)
{
    NEEDRESTART
    CurrentConfig.defaultRouteConfig.connectionConfig.enableProxy = !(arg1 == Qt::Checked);
}

void PreferencesWindow::on_localDNSCb_stateChanged(int arg1)
{
    NEEDRESTART
    CurrentConfig.defaultRouteConfig.connectionConfig.withLocalDNS = arg1 == Qt::Checked;
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

void PreferencesWindow::on_tProxyCheckBox_stateChanged(int arg1)
{
    LOADINGCHECK
#ifdef Q_OS_LINUX
    // Setting up tProxy for linux
    // Steps:
    // --> 1. Copy V2Ray core files to the QV2RAY_TPROXY_VCORE_PATH and QV2RAY_TPROXY_VCTL_PATH dir.
    // --> 2. Change GlobalConfig.v2CorePath.
    // --> 3. Call `pkexec setcap
    // CAP_NET_ADMIN,CAP_NET_RAW,CAP_NET_BIND_SERVICE=eip` on the V2Ray core.
    auto const kernelPath = CurrentConfig.kernelConfig.KernelPath();
    if (arg1 == Qt::Checked)
    {
        // We enable it!
        if (QvMessageBoxAsk(this, tr("Enable tProxy Support"),
                            tr("This will append capabilities to the V2Ray executable.") + NEWLINE + NEWLINE +
                                tr("Qv2ray will copy your V2Ray core to this path: ") + NEWLINE + QV2RAY_TPROXY_VCORE_PATH + NEWLINE + NEWLINE +
                                tr("If anything goes wrong after enabling this, please check issue #57 or the link below:") + NEWLINE +
                                " https://github.com/Qv2ray/Qv2ray/wiki/FAQ ") != QMessageBox::Yes)
        {
            tProxyCheckBox->setChecked(false);
            LOG(MODULE_UI, "Canceled enabling tProxy feature.")
        }
        else
        {

            LOG(MODULE_VCORE, "ENABLING tProxy Support")
            LOG(MODULE_FILEIO, " --> Origin V2Ray core file is at: " + kernelPath)
            auto v2ctlPath = QFileInfo(kernelPath).absolutePath() + "/v2ctl";
            auto newPath = QFileInfo(QV2RAY_TPROXY_VCORE_PATH).absolutePath();
            QString mkPathResult = QDir().mkpath(newPath) ? "OK" : "FAILED";
            LOG(MODULE_FILEIO, " --> mkPath result: " + mkPathResult)
            //
            LOG(MODULE_FILEIO, " --> Origin v2ctl file is at: " + v2ctlPath)
            LOG(MODULE_FILEIO, " --> New V2Ray files will be placed in: " + newPath)
            //
            LOG(MODULE_FILEIO, " --> Copying files....")

            if (QFileInfo(kernelPath).absoluteFilePath() != QFileInfo(QV2RAY_TPROXY_VCORE_PATH).absoluteFilePath())
            {
                // Only trying to remove file when they are not in the default
                // dir. (In other words...) Keep using the current files.
                // <Because we don't know where else we can copy the file
                // from...>
                //
                if (QFile(QV2RAY_TPROXY_VCORE_PATH).exists())
                {
                    LOG(MODULE_FILEIO, QString(QV2RAY_TPROXY_VCORE_PATH) + ": File already exists.")
                    LOG(MODULE_FILEIO, QString(QV2RAY_TPROXY_VCORE_PATH) + ": Deleting file.")
                    QFile(QV2RAY_TPROXY_VCORE_PATH).remove();
                }

                if (QFile(QV2RAY_TPROXY_VCTL_PATH).exists())
                {
                    LOG(MODULE_FILEIO, QV2RAY_TPROXY_VCTL_PATH + ": File already exists.")
                    LOG(MODULE_FILEIO, QV2RAY_TPROXY_VCTL_PATH + ": Deleting file.")
                    QFile(QV2RAY_TPROXY_VCTL_PATH).remove();
                }

                QString vCoreresult = QFile(kernelPath).copy(QV2RAY_TPROXY_VCORE_PATH) ? "OK" : "FAILED";
                LOG(MODULE_FILEIO, " --> V2Ray Core: " + vCoreresult)
                //
                QString vCtlresult = QFile(v2ctlPath).copy(QV2RAY_TPROXY_VCTL_PATH) ? "OK" : "FAILED";
                LOG(MODULE_FILEIO, " --> V2Ray Ctl: " + vCtlresult)
                //

                if (vCoreresult == "OK" && vCtlresult == "OK")
                {
                    LOG(MODULE_VCORE, " --> Done copying files.")
                    on_vCorePathTxt_textEdited(QV2RAY_TPROXY_VCORE_PATH);
                }
                else
                {
                    LOG(MODULE_VCORE, "FAILED to copy V2Ray files. Aborting.")
                    QvMessageBoxWarn(this, tr("Enable tProxy Support"),
                                     tr("Qv2ray cannot copy one or both V2Ray files from: ") + NEWLINE + NEWLINE + kernelPath + NEWLINE +
                                         v2ctlPath + NEWLINE + NEWLINE + tr("to this path: ") + NEWLINE + newPath);
                    return;
                }
            }
            else
            {
                LOG(MODULE_VCORE, "Skipped removing files since the current V2Ray core is in the default path.")
                LOG(MODULE_VCORE, " --> Actually because we don't know where else to obtain the files.")
            }

            LOG(MODULE_UI, "Calling pkexec and setcap...")
            int ret = QProcess::execute("pkexec", { "/usr/sbin/setcap CAP_NET_ADMIN,CAP_NET_RAW,CAP_NET_BIND_SERVICE=eip " + kernelPath });
            if (ret != 0)
            {
                LOG(MODULE_UI, "WARN: setcap exits with code: " + QSTRN(ret))
                QvMessageBoxWarn(this, tr("Preferences"), tr("Failed to setcap onto V2Ray executable. You may need to run `setcap` manually."));
            }

            CurrentConfig.tProxySupport = true;
            NEEDRESTART
        }
    }
    else
    {
        int ret = QProcess::execute("pkexec", { "/usr/sbin/setcap -r " + kernelPath });

        if (ret != 0)
        {
            LOG(MODULE_UI, "WARN: setcap exits with code: " + QSTRN(ret))
            QvMessageBoxWarn(this, tr("Preferences"), tr("Failed to setcap onto V2Ray executable. You may need to run `setcap` manually."));
        }

        CurrentConfig.tProxySupport = false;
        NEEDRESTART
    }

#else
    Q_UNUSED(arg1)
    // No such tProxy thing on Windows and macOS
    QvMessageBoxWarn(this, tr("Preferences"), tr("tProxy is not supported on macOS and Windows"));
    CurrentConfig.tProxySupport = false;
    tProxyCheckBox->setChecked(false);
#endif
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
        BLACK(socksUDPIP)
    }
    else
    {
        RED(socksUDPIP)
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

void PreferencesWindow::on_setSysProxyCB_stateChanged(int arg1)
{
    LOADINGCHECK
    NEEDRESTART
    CurrentConfig.inboundConfig.systemProxySettings.setSystemProxy = arg1 == Qt::Checked;
}

void PreferencesWindow::on_autoStartSubsCombo_currentIndexChanged(const QString &arg1)
{
    LOADINGCHECK
    if (arg1.isEmpty())
    {
        CurrentConfig.autoStartId.clear();
        autoStartConnCombo->clear();
    }
    else
    {
        auto list = ConnectionManager->Connections(GroupId(autoStartSubsCombo->currentData().toString()));
        autoStartConnCombo->clear();

        for (const auto &id : list)
        {
            autoStartConnCombo->addItem(GetDisplayName(id), id.toString());
        }
    }
}

void PreferencesWindow::on_autoStartConnCombo_currentIndexChanged(const QString &arg1)
{
    LOADINGCHECK
    if (arg1.isEmpty())
    {
        CurrentConfig.autoStartId.clear();
    }
    else
    {
        // Fully qualify the connection item.
        // Will not work when duplicated names are in the same group.
        CurrentConfig.autoStartId.groupId = GroupId(autoStartSubsCombo->currentData().toString());
        CurrentConfig.autoStartId.connectionId = ConnectionId(autoStartConnCombo->currentData().toString());
        //= autoStartConnCombo->currentData().toString();
        //            ConnectionManager->GetConnectionIdByDisplayName(arg1,
        //            ConnectionManager->GetGroupIdByDisplayName(autoStartSubsCombo->currentText()))
        //              .toString();
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

void PreferencesWindow::on_fpTypeCombo_currentIndexChanged(const QString &arg1)
{
    LOADINGCHECK
    CurrentConfig.defaultRouteConfig.forwardProxyConfig.type = arg1.toLower();
}

void PreferencesWindow::on_fpAddressTx_textEdited(const QString &arg1)
{
    LOADINGCHECK
    CurrentConfig.defaultRouteConfig.forwardProxyConfig.serverAddress = arg1;

    if (IsValidIPAddress(arg1))
    {
        BLACK(fpAddressTx)
    }
    else
    {
        RED(fpAddressTx)
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
    QString result;

    // prevent some bullshit situations.
    if (const auto vCorePathSmallCased = vcorePath.toLower();
        vCorePathSmallCased.endsWith("qv2ray") || vCorePathSmallCased.endsWith("qv2ray.exe"))
    {
        const auto strWarnTitle = tr("Watch Out!");
        const auto strWarnContent = //
            tr("You may be about to set V2Ray core incorrectly to Qv2ray itself, which is absolutely not correct.\r\n"
               "This won't trigger a fork bomb, however, since Qv2ray works in singleton mode.\r\n"
               "If your V2Ray core filename happened to be 'qv2ray'-something, you are totally free to ignore this warning.");
        const auto answer = QMessageBox::warning(this, strWarnTitle, strWarnContent,                                       //
                                                 QMessageBox::StandardButton::Abort | QMessageBox::StandardButton::Ignore, //
                                                 QMessageBox::StandardButton::Abort);
        if (answer == QMessageBox::StandardButton::Abort)
            return;
    }
    else if (vCorePathSmallCased.endsWith("v2ctl") || vCorePathSmallCased.endsWith("v2ctl.exe"))
    {
        const auto strWarnTitle = tr("Watch Out!");
        const auto strWarnContent = //
            tr("You may be about to set V2Ray core incorrectly to V2Ray Control executable, which is absolutely not correct.\r\n"
               "The filename of V2Ray core is usually 'v2ray' or 'v2ray.exe'. Make sure to choose it wisely.\r\n"
               "If you insist to proceed, we're not providing with any support.");
        const auto answer = QMessageBox::warning(this, strWarnTitle, strWarnContent,                                       //
                                                 QMessageBox::StandardButton::Abort | QMessageBox::StandardButton::Ignore, //
                                                 QMessageBox::StandardButton::Abort);
        if (answer == QMessageBox::StandardButton::Abort)
            return;
    }

    if (!V2RayKernelInstance::ValidateKernel(vcorePath, vAssetsPath, &result))
    {
        QvMessageBoxWarn(this, tr("V2Ray Core Settings"), result);
    }
    else if (!result.toLower().contains("v2ray"))
    {
        const auto strWarnContent = //
            tr("This does not seem like an output from V2Ray Core.\r\n"
               "If you've been looking for plugin cores, you should change this in plugin settings rather than here.\r\n"
               "Output: \r\n\r\n") +
            result;
        QvMessageBoxWarn(this, tr("'V2Ray Core' Settings"), strWarnContent);
    }
    else
    {
        QvMessageBoxInfo(this, tr("V2Ray Core Settings"),
                         tr("V2Ray path configuration check passed.") + NEWLINE + NEWLINE + tr("Current version of V2Ray is: ") + NEWLINE +
                             result);
    }
}

void PreferencesWindow::on_httpGroupBox_clicked(bool checked)
{
    LOADINGCHECK
    NEEDRESTART
    CurrentConfig.inboundConfig.useHTTP = checked;
}

void PreferencesWindow::on_socksGroupBox_clicked(bool checked)
{
    LOADINGCHECK
    NEEDRESTART
    CurrentConfig.inboundConfig.useSocks = checked;
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
    if (arg1 == Qt::Unchecked)
    {
        const auto msgAPIDisableTitle = tr("Disabling API Subsystem");
        const auto msgAPIDisableMsg = tr("Disabling API subsystem will also disable the statistics function of Qv2ray.") + NEWLINE + //
                                      tr("Speed chart and traffic statistics will be disabled.");
        QvMessageBoxWarn(this, msgAPIDisableTitle, msgAPIDisableMsg);
    }
}

void PreferencesWindow::on_updateChannelCombo_currentIndexChanged(int index)
{
    LOADINGCHECK
    CurrentConfig.updateConfig.updateChannel = index;
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

void PreferencesWindow::on_setAllowInsecureCB_stateChanged(int arg1)
{
    LOADINGCHECK
    if (arg1 == Qt::Checked)
    {
        QvMessageBoxWarn(this, tr("Dangerous Operation"), tr("You will lose the advantage of TLS and make your connection under MITM attack."));
    }
    CurrentConfig.advancedConfig.setAllowInsecure = arg1 == Qt::Checked;
}

void PreferencesWindow::on_setTestLatenctCB_stateChanged(int arg1)
{
    LOADINGCHECK
    if (arg1 == Qt::Checked)
    {
        QvMessageBoxWarn(this, tr("Dangerous Operation"), tr("This will (probably) make it easy to fingerprint your connection."));
    }
    CurrentConfig.advancedConfig.testLatencyPeriodcally = arg1 == Qt::Checked;
}

void PreferencesWindow::on_setSessionResumptionCB_stateChanged(int arg1)
{
    LOADINGCHECK
    if (arg1 == Qt::Checked)
    {
        QvMessageBoxWarn(this, tr("Dangerous Operation"), tr("This will make your TLS fingerpring different from common Golang programs."));
    }
    CurrentConfig.advancedConfig.setSessionResumption = arg1 == Qt::Checked;
}

void PreferencesWindow::on_quietModeCB_stateChanged(int arg1)
{
    LOADINGCHECK
    CurrentConfig.uiConfig.quietMode = arg1 == Qt::Checked;
}

void PreferencesWindow::on_tproxGroupBox_toggled(bool arg1)
{
    NEEDRESTART
    CurrentConfig.inboundConfig.useTPROXY = arg1;
}

void PreferencesWindow::on_tProxyPort_valueChanged(int arg1)
{
    NEEDRESTART
    CurrentConfig.inboundConfig.tProxySettings.port = arg1;
}

void PreferencesWindow::on_tproxyEnableTCP_toggled(bool checked)
{
    NEEDRESTART
    CurrentConfig.inboundConfig.tProxySettings.hasTCP = checked;
}

void PreferencesWindow::on_tproxyEnableUDP_toggled(bool checked)
{
    NEEDRESTART
    CurrentConfig.inboundConfig.tProxySettings.hasUDP = checked;
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
    {
        BLACK(tproxyListenAddr)
    }
    else
    {
        RED(tproxyListenAddr)
    }
}

void PreferencesWindow::on_tproxyListenV6Addr_textEdited(const QString &arg1)
{
    NEEDRESTART
    CurrentConfig.inboundConfig.tProxySettings.tProxyV6IP = arg1;

    if (arg1 == "" || IsIPv6Address(arg1))
    {
        BLACK(tproxyListenV6Addr)
    }
    else
    {
        RED(tproxyListenV6Addr)
    }
}

void PreferencesWindow::on_jumpListCountSB_valueChanged(int arg1)
{
    CurrentConfig.uiConfig.maxJumpListCount = arg1;
}

void PreferencesWindow::on_outboundMark_valueChanged(int arg1)
{
    NEEDRESTART
    CurrentConfig.outboundConfig.mark = arg1;
}

void PreferencesWindow::on_dnsIntercept_toggled(bool checked)
{
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

void PreferencesWindow::on_DnsFreedomCb_stateChanged(int arg1)
{
    NEEDRESTART
    CurrentConfig.defaultRouteConfig.connectionConfig.v2rayFreedomDNS = arg1 == Qt::Checked;
}

void PreferencesWindow::on_httpSniffingCB_stateChanged(int arg1)
{
    NEEDRESTART
    CurrentConfig.inboundConfig.httpSettings.sniffing = arg1 == Qt::Checked;
}

void PreferencesWindow::on_socksSniffingCB_stateChanged(int arg1)
{
    NEEDRESTART
    CurrentConfig.inboundConfig.socksSettings.sniffing = arg1 == Qt::Checked;
}

void PreferencesWindow::on_pushButton_clicked()
{
    const auto ntpTitle = tr("NTP Checker");
    const auto ntpHint = tr("Check date and time from server:");
    const static QStringList ntpServerList = { "cn.pool.ntp.org",         //
                                               "cn.ntp.org.cn",           //
                                               "edu.ntp.org.cn",          //
                                               "time.pool.aliyun.com",    //
                                               "time1.cloud.tencent.com", //
                                               "ntp.neu.edu.cn" };
    bool ok = false;
    QString ntpServer = QInputDialog::getItem(this, ntpTitle, ntpHint, ntpServerList, 0, true, &ok).trimmed();
    if (!ok)
        return;

    auto client = new ntp::NtpClient(this);
    connect(client, &ntp::NtpClient::replyReceived, [&](const QHostAddress &, quint16, const ntp::NtpReply &reply) {
        const int offsetSecTotal = reply.localClockOffset() / 1000;
        if (offsetSecTotal >= 90 || offsetSecTotal <= -90)
        {
            const auto inaccurateWarning = tr("Your time offset is %1 seconds, which is too high.") + NEWLINE + //
                                           tr("Please synchronize your system to use V2Ray.");
            QvMessageBoxWarn(this, tr("Time Inaccurate"), inaccurateWarning.arg(offsetSecTotal));
        }
        else if (offsetSecTotal > 15 || offsetSecTotal < -15)
        {
            const auto smallErrorWarning = tr("Your time offset is %1 seconds, which is a little high.") + NEWLINE + //
                                           tr("V2Ray may still work, but we suggest you synchronize your clock.");
            QvMessageBoxInfo(this, tr("Time Somewhat Inaccurate"), smallErrorWarning.arg(offsetSecTotal));
        }
        else
        {
            const auto accurateInfo = tr("Your time offset is %1 seconds, which looks good.") + NEWLINE + //
                                      tr("V2Ray may not suffer from time inaccuracy.");
            QvMessageBoxInfo(this, tr("Time Accurate"), accurateInfo.arg(offsetSecTotal));
        }
    });

    const auto hostInfo = QHostInfo::fromName(ntpServer);
    if (hostInfo.error() == QHostInfo::NoError)
    {
        client->sendRequest(hostInfo.addresses().first(), 123);
    }
    else
    {
        QvMessageBoxWarn(this, ntpTitle, tr("Failed to lookup server: %1").arg(hostInfo.errorString()));
    }
}

void PreferencesWindow::on_noAutoConnectRB_clicked()
{
    LOADINGCHECK
    CurrentConfig.autoStartBehavior = AUTO_CONNECTION_NONE;
    SET_AUTOSTART_UI_ENABLED(false);
}

void PreferencesWindow::on_lastConnectedRB_clicked()
{
    LOADINGCHECK
    CurrentConfig.autoStartBehavior = AUTO_CONNECTION_LAST_CONNECTED;
    SET_AUTOSTART_UI_ENABLED(false);
}

void PreferencesWindow::on_fixedAutoConnectRB_clicked()
{
    LOADINGCHECK
    CurrentConfig.autoStartBehavior = AUTO_CONNECTION_FIXED;
    SET_AUTOSTART_UI_ENABLED(true);
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
