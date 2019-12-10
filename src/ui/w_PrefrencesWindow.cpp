#include <QFileDialog>
#include <QColorDialog>
#include <QStyleFactory>
#include <QStyle>
#include <QDesktopServices>

#include "QvUtils.hpp"
#include "QvCoreInteractions.hpp"
#include "QvNetSpeedPlugin.hpp"
#include "QvCoreConfigOperations.hpp"

#include "w_PrefrencesWindow.hpp"
#include "QvHTTPRequestHelper.hpp"

#define LOADINGCHECK if(!finishedLoading) return;
#define NEEDRESTART if(finishedLoading) IsConnectionPropertyChanged = true;

PrefrencesWindow::PrefrencesWindow(QWidget *parent) : QDialog(parent),
    CurrentConfig(),
    ui(new Ui::PrefrencesWindow)
{
    setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    // We add locales
    languageComboBox->clear();
    QDirIterator it(":/translations");

    while (it.hasNext()) {
        languageComboBox->addItem(it.next().split("/").last().split(".").first());
    }

    //
    nsBarContentCombo->addItems(NetSpeedPluginMessages.values());
    themeCombo->addItems(QStyleFactory::keys());
    //
    qvVersion->setText(QV2RAY_VERSION_STRING);
    qvBuildTime->setText(__DATE__ " " __TIME__);
    CurrentConfig = GetGlobalConfig();
    //
    themeCombo->setCurrentText(QSTRING(CurrentConfig.uiConfig.theme));
    darkThemeCB->setChecked(CurrentConfig.uiConfig.useDarkTheme);
    darkTrayCB->setChecked(CurrentConfig.uiConfig.useDarkTrayIcon);
#if QV2RAY_USE_BUILTIN_DARKTHEME
    // If we use built in theme, it should always be fusion.
    themeCombo->setEnabled(!CurrentConfig.uiConfig.useDarkTheme);
    darkThemeLabel->setText(tr("Use Darkmode Theme"));
#endif
    languageComboBox->setCurrentText(QSTRING(CurrentConfig.uiConfig.language));
    logLevelComboBox->setCurrentIndex(CurrentConfig.logLevel);
    tProxyCheckBox->setChecked(CurrentConfig.tProxySupport);
    //
    //
    listenIPTxt->setText(QSTRING(CurrentConfig.inboundConfig.listenip));
    bool pacEnabled = CurrentConfig.inboundConfig.pacConfig.enablePAC;
    enablePACCB->setChecked(pacEnabled);
    setSysProxyCB->setChecked(CurrentConfig.inboundConfig.setSystemProxy);
    //
    // PAC
    pacGroupBox->setEnabled(pacEnabled);
    pacPortSB->setValue(CurrentConfig.inboundConfig.pacConfig.port);
    pacProxyTxt->setText(QSTRING(CurrentConfig.inboundConfig.pacConfig.localIP));
    pacProxyCB->setCurrentIndex(CurrentConfig.inboundConfig.pacConfig.useSocksProxy ? 1 : 0);
    //
    bool have_http = CurrentConfig.inboundConfig.useHTTP;
    httpCB->setChecked(have_http);
    httpPortLE->setValue(CurrentConfig.inboundConfig.http_port);
    httpAuthCB->setChecked(CurrentConfig.inboundConfig.http_useAuth);
    //
    httpAuthCB->setChecked(CurrentConfig.inboundConfig.http_useAuth);
    httpAuthUsernameTxt->setEnabled(CurrentConfig.inboundConfig.http_useAuth);
    httpAuthPasswordTxt->setEnabled(CurrentConfig.inboundConfig.http_useAuth);
    httpAuthUsernameTxt->setText(QSTRING(CurrentConfig.inboundConfig.httpAccount.user));
    httpAuthPasswordTxt->setText(QSTRING(CurrentConfig.inboundConfig.httpAccount.pass));
    httpGroupBox->setEnabled(have_http);
    //
    //
    bool have_socks = CurrentConfig.inboundConfig.useSocks;
    socksCB->setChecked(have_socks);
    socksPortLE->setValue(CurrentConfig.inboundConfig.socks_port);
    //
    socksAuthCB->setChecked(CurrentConfig.inboundConfig.socks_useAuth);
    socksAuthUsernameTxt->setEnabled(CurrentConfig.inboundConfig.socks_useAuth);
    socksAuthPasswordTxt->setEnabled(CurrentConfig.inboundConfig.socks_useAuth);
    socksAuthUsernameTxt->setText(QSTRING(CurrentConfig.inboundConfig.socksAccount.user));
    socksAuthPasswordTxt->setText(QSTRING(CurrentConfig.inboundConfig.socksAccount.pass));
    // Socks UDP Options
    socksUDPCB->setChecked(CurrentConfig.inboundConfig.socksUDP);
    socksUDPIP->setEnabled(CurrentConfig.inboundConfig.socksUDP);
    socksUDPIP->setText(QSTRING(CurrentConfig.inboundConfig.socksLocalIP));
    socksGroupBox->setEnabled(have_socks);
    //
    //
    vCorePathTxt->setText(QSTRING(CurrentConfig.v2CorePath));
    vCoreAssetsPathTxt->setText(QSTRING(CurrentConfig.v2AssetsPath));
    statsCheckbox->setChecked(CurrentConfig.connectionConfig.enableStats);
    statsPortBox->setValue(CurrentConfig.connectionConfig.statsPort);
    //
    //
    bypassCNCb->setChecked(CurrentConfig.connectionConfig.bypassCN);
    proxyDefaultCb->setChecked(CurrentConfig.connectionConfig.enableProxy);
    //
    localDNSCb->setChecked(CurrentConfig.connectionConfig.withLocalDNS);
    //
    DNSListTxt->clear();

    foreach (auto dnsStr, CurrentConfig.connectionConfig.dnsList) {
        auto str = QString::fromStdString(dnsStr).trimmed();

        if (!str.isEmpty()) {
            DNSListTxt->appendPlainText(str);
        }
    }

    //
    cancelIgnoreVersionBtn->setEnabled(CurrentConfig.ignoredVersion != "");
    ignoredNextVersion->setText(QSTRING(CurrentConfig.ignoredVersion));

    for (size_t i = 0; i < CurrentConfig.toolBarConfig.Pages.size(); i++) {
        nsBarPagesList->addItem(tr("Page") + QString::number(i + 1) + ": " + QString::number(CurrentConfig.toolBarConfig.Pages[i].Lines.size()) + " " + tr("Item(s)"));
    }

    if (CurrentConfig.toolBarConfig.Pages.size() > 0) {
        nsBarPagesList->setCurrentRow(0);
        on_nsBarPagesList_currentRowChanged(0);
    } else {
        nsBarVerticalLayout->setEnabled(false);
        nsBarLinesList->setEnabled(false);
        nsBarLineDelBTN->setEnabled(false);
        nsBarLineAddBTN->setEnabled(false);
        nsBarPageYOffset->setEnabled(false);
    }

    CurrentBarPageId = 0;
    //
    // Empty for global config.
    autoStartConnCombo->addItem("");

    for (auto item : CurrentConfig.subscribes) {
        autoStartSubsCombo->addItem(QSTRING(item.first));
    }

    autoStartSubsCombo->setCurrentText(QSTRING(CurrentConfig.autoStartConfig.subscriptionName));

    if (CurrentConfig.autoStartConfig.subscriptionName.empty()) {
        autoStartConnCombo->addItems(ConvertQStringList(QList<string>::fromStdList(CurrentConfig.configs)));
    } else {
        auto list = GetSubscriptionConnection(CurrentConfig.autoStartConfig.subscriptionName);
        autoStartConnCombo->addItems(list.keys());
    }

    autoStartConnCombo->setCurrentText(QSTRING(CurrentConfig.autoStartConfig.connectionName));
    finishedLoading = true;
}

PrefrencesWindow::~PrefrencesWindow()
{
    delete ui;
}

void PrefrencesWindow::on_buttonBox_accepted()
{
    int sp = socksPortLE->text().toInt();
    int hp = httpPortLE->text().toInt() ;

    if (!(sp == 0 || hp == 0) && sp == hp) {
        QvMessageBox(this, tr("Prefrences"), tr("Port numbers cannot be the same"));
        return;
    }

    SetGlobalConfig(CurrentConfig);
    emit s_reload_config(IsConnectionPropertyChanged);
}

void PrefrencesWindow::on_httpCB_stateChanged(int checked)
{
    NEEDRESTART
    bool enabled = checked == Qt::Checked;
    httpGroupBox->setEnabled(enabled);
    CurrentConfig.inboundConfig.useHTTP = enabled;
}

void PrefrencesWindow::on_socksCB_stateChanged(int checked)
{
    NEEDRESTART
    bool enabled = checked == Qt::Checked;
    socksGroupBox->setEnabled(enabled);
    CurrentConfig.inboundConfig.useSocks = enabled;
}

void PrefrencesWindow::on_httpAuthCB_stateChanged(int checked)
{
    NEEDRESTART
    bool enabled = checked == Qt::Checked;
    httpAuthUsernameTxt->setEnabled(enabled);
    httpAuthPasswordTxt->setEnabled(enabled);
    CurrentConfig.inboundConfig.http_useAuth = enabled;
}

void PrefrencesWindow::on_socksAuthCB_stateChanged(int checked)
{
    NEEDRESTART
    bool enabled = checked == Qt::Checked;
    socksAuthUsernameTxt->setEnabled(enabled);
    socksAuthPasswordTxt->setEnabled(enabled);
    CurrentConfig.inboundConfig.socks_useAuth = enabled;
}

void PrefrencesWindow::on_languageComboBox_currentTextChanged(const QString &arg1)
{
    LOADINGCHECK
    //
    // A strange bug prevents us to change the UI language online
    //    https://github.com/lhy0403/Qv2ray/issues/34
    //
    CurrentConfig.uiConfig.language = arg1.toStdString();
    //
    //
    //if (QApplication::installTranslator(getTranslator(arg1))) {
    //    LOG(MODULE_UI, "Loaded translations " + arg1.toStdString())
    //    retranslateUi(this);
    //} else {
    //    QvMessageBox(this, tr("#Prefrences"), tr("#SwitchTranslationError"));
    //}
    //
    //emit retranslateUi(this);
}

void PrefrencesWindow::on_logLevelComboBox_currentIndexChanged(int index)
{
    NEEDRESTART
    CurrentConfig.logLevel = index;
}

void PrefrencesWindow::on_vCoreAssetsPathTxt_textEdited(const QString &arg1)
{
    NEEDRESTART
    CurrentConfig.v2AssetsPath = arg1.toStdString();
}

void PrefrencesWindow::on_listenIPTxt_textEdited(const QString &arg1)
{
    NEEDRESTART
    CurrentConfig.inboundConfig.listenip = arg1.toStdString();
    //pacAccessPathTxt->setText("http://" + arg1 + ":" + QString::number(pacPortSB->value()) + "/pac");
}

void PrefrencesWindow::on_httpAuthUsernameTxt_textEdited(const QString &arg1)
{
    NEEDRESTART
    CurrentConfig.inboundConfig.httpAccount.user = arg1.toStdString();
}

void PrefrencesWindow::on_httpAuthPasswordTxt_textEdited(const QString &arg1)
{
    NEEDRESTART
    CurrentConfig.inboundConfig.httpAccount.pass = arg1.toStdString();
}

void PrefrencesWindow::on_socksAuthUsernameTxt_textEdited(const QString &arg1)
{
    NEEDRESTART
    CurrentConfig.inboundConfig.socksAccount.user = arg1.toStdString();
}

void PrefrencesWindow::on_socksAuthPasswordTxt_textEdited(const QString &arg1)
{
    NEEDRESTART
    CurrentConfig.inboundConfig.socksAccount.pass = arg1.toStdString();
}

void PrefrencesWindow::on_proxyDefaultCb_stateChanged(int arg1)
{
    NEEDRESTART
    CurrentConfig.connectionConfig.enableProxy = arg1 == Qt::Checked;
}

void PrefrencesWindow::on_localDNSCb_stateChanged(int arg1)
{
    NEEDRESTART
    CurrentConfig.connectionConfig.withLocalDNS = arg1 == Qt::Checked;
}

void PrefrencesWindow::on_selectVAssetBtn_clicked()
{
    NEEDRESTART
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open v2ray assets folder"), QDir::currentPath());

    if (!dir.isEmpty()) {
        vCoreAssetsPathTxt->setText(dir);
        on_vCoreAssetsPathTxt_textEdited(dir);
    }
}

void PrefrencesWindow::on_selectVCoreBtn_clicked()
{
    QString core = QFileDialog::getOpenFileName(this, tr("Open v2ray core file"), QDir::currentPath());

    if (!core.isEmpty()) {
        vCorePathTxt->setText(core);
        on_vCorePathTxt_textEdited(core);
    }
}

void PrefrencesWindow::on_vCorePathTxt_textEdited(const QString &arg1)
{
    NEEDRESTART
    CurrentConfig.v2CorePath = arg1.toStdString();
}

void PrefrencesWindow::on_DNSListTxt_textChanged()
{
    if (finishedLoading) {
        try {
            QStringList hosts = DNSListTxt->toPlainText().replace("\r", "").split("\n");
            CurrentConfig.connectionConfig.dnsList.clear();

            foreach (auto host, hosts) {
                if (host != "" && host != "\r") {
                    // Not empty, so we save.
                    CurrentConfig.connectionConfig.dnsList.push_back(host.toStdString());
                    NEEDRESTART
                }
            }

            BLACK(DNSListTxt)
        } catch (...) {
            RED(DNSListTxt)
        }
    }
}

void PrefrencesWindow::on_aboutQt_clicked()
{
    QApplication::aboutQt();
}

void PrefrencesWindow::on_cancelIgnoreVersionBtn_clicked()
{
    CurrentConfig.ignoredVersion.clear();
    cancelIgnoreVersionBtn->setEnabled(false);
}

void PrefrencesWindow::on_tProxyCheckBox_stateChanged(int arg1)
{
#ifdef __linux

    if (finishedLoading) {
        // Set UID and GID for linux
        // Steps:
        // --> 1. Copy v2ray core files to the #CONFIG_DIR#/vcore/ dir.
        // --> 2. Change GlobalConfig.v2CorePath.
        // --> 3. Call `pkexec setcap CAP_NET_ADMIN,CAP_NET_RAW,CAP_NET_BIND_SERVICE=eip` on the v2ray core.
        if (arg1 == Qt::Checked) {
            // We enable it!
            if (QvMessageBoxAsk(this, tr("Enable tProxy Support"),
                                tr("This will append capabilities to the v2ray executable.")  + NEWLINE + NEWLINE +
                                tr("Qv2ray will copy your v2ray core to this path: ") + NEWLINE + QV2RAY_DEFAULT_VCORE_PATH + NEWLINE + NEWLINE +
                                tr("If anything goes wrong after enabling this, please refer to issue #57 or the link below:") + NEWLINE +
                                " https://lhy0403.github.io/Qv2ray/zh-CN/FAQ.html ") != QMessageBox::Yes) {
                tProxyCheckBox->setChecked(false);
                LOG(MODULE_UI, "Canceled enabling tProxy feature.")
            } else {
                LOG(MODULE_VCORE, "ENABLING tProxy Support")
                LOG(MODULE_FILE, " --> Origin v2ray core file is at: " + CurrentConfig.v2CorePath)
                auto v2ctlPath = QFileInfo(QSTRING(CurrentConfig.v2CorePath)).path() + "/v2ctl";
                auto newPath = QFileInfo(QV2RAY_DEFAULT_VCORE_PATH).path();
                //
                LOG(MODULE_FILE, " --> Origin v2ctl file is at: " + v2ctlPath.toStdString())
                LOG(MODULE_FILE, " --> New v2ray files will be placed in: " + newPath.toStdString())
                //
                LOG(MODULE_FILE, " --> Copying files....")

                if (QFileInfo(QSTRING(CurrentConfig.v2CorePath)).absoluteFilePath() !=  QFileInfo(QV2RAY_DEFAULT_VCORE_PATH).absoluteFilePath()) {
                    // Only trying to remove file when they are not in the default dir.
                    // (In other words...) Keep using the current files. <Because we don't know where else we can copy the file from...>
                    if (QFile(QV2RAY_DEFAULT_VCORE_PATH).exists()) {
                        LOG(MODULE_FILE, QV2RAY_DEFAULT_VCORE_PATH.toStdString() + ": File already exists.")
                        LOG(MODULE_FILE, QV2RAY_DEFAULT_VCORE_PATH.toStdString() + ": Deleting file.")
                        QFile(QV2RAY_DEFAULT_VCORE_PATH).remove();
                    }

                    if (QFile(newPath + "/v2ctl").exists()) {
                        LOG(MODULE_FILE, newPath.toStdString() + "/v2ctl : File already exists.")
                        LOG(MODULE_FILE, newPath.toStdString() + "/v2ctl : Deleting file.")
                        QFile(newPath + "/v2ctl").remove();
                    }

                    string vCoreresult = QFile(QSTRING(CurrentConfig.v2CorePath)).copy(QV2RAY_DEFAULT_VCORE_PATH) ? "OK" : "FAILED";
                    LOG(MODULE_FILE, " --> v2ray Core: " + vCoreresult)
                    //
                    string vCtlresult = QFile(v2ctlPath).copy(newPath + "/v2ctl") ? "OK" : "FAILED";
                    LOG(MODULE_FILE, " --> v2ray Ctl: " + vCtlresult)
                    //

                    if (vCoreresult == "OK" && vCtlresult == "OK") {
                        LOG(MODULE_VCORE, " --> Done copying files.")
                        on_vCorePathTxt_textEdited(QV2RAY_DEFAULT_VCORE_PATH);
                    } else {
                        LOG(MODULE_VCORE, "FAILED to copy v2ray files. Aborting.")
                        QvMessageBox(this, tr("Enable tProxy Support"),
                                     tr("Qv2ray cannot copy one or both v2ray files from: ") + NEWLINE + NEWLINE +
                                     QSTRING(CurrentConfig.v2CorePath) + NEWLINE + v2ctlPath + NEWLINE + NEWLINE +
                                     tr("to this path: ") + NEWLINE + newPath);
                        return;
                    }
                } else {
                    LOG(MODULE_VCORE, "Skipped removing files since the current v2ray core is in the default path.")
                    LOG(MODULE_VCORE, " --> Actually because we don't know where else to obtain the files.")
                }

                LOG(MODULE_UI, "Calling pkexec and setcap...")
                int ret = QProcess::execute("pkexec setcap CAP_NET_ADMIN,CAP_NET_RAW,CAP_NET_BIND_SERVICE=eip " + QSTRING(CurrentConfig.v2CorePath));

                if (ret != 0) {
                    LOG(MODULE_UI, "WARN: setcap exits with code: " + to_string(ret))
                    QvMessageBox(this, tr("Prefrences"), tr("Failed to setcap onto v2ray executable. You may need to run `setcap` manually."));
                }

                CurrentConfig.tProxySupport = true;
                NEEDRESTART
            }
        } else {
            int ret = QProcess::execute("pkexec setcap -r " + QSTRING(CurrentConfig.v2CorePath));

            if (ret != 0) {
                LOG(MODULE_UI, "WARN: setcap exits with code: " + to_string(ret))
                QvMessageBox(this, tr("Prefrences"), tr("Failed to setcap onto v2ray executable. You may need to run `setcap` manually."));
            }

            CurrentConfig.tProxySupport = false;
            NEEDRESTART
        }
    }

#else
    Q_UNUSED(arg1)
    // No such tProxy thing on Windows and macOS
    QvMessageBox(this, tr("Prefrences"), tr("tProxy is not supported on macOS and Windows"));
    CurrentConfig.tProxySupport = false;
    tProxyCheckBox->setChecked(false);
#endif
}
void PrefrencesWindow::on_bypassCNCb_stateChanged(int arg1)
{
    NEEDRESTART
    CurrentConfig.connectionConfig.bypassCN = arg1 == Qt::Checked;
}

void PrefrencesWindow::on_statsCheckbox_stateChanged(int arg1)
{
    NEEDRESTART
    CurrentConfig.connectionConfig.enableStats = arg1 == Qt::Checked;
}

void PrefrencesWindow::on_statsPortBox_valueChanged(int arg1)
{
    NEEDRESTART
    CurrentConfig.connectionConfig.statsPort = arg1;
}

void PrefrencesWindow::on_socksPortLE_valueChanged(int arg1)
{
    NEEDRESTART
    CurrentConfig.inboundConfig.socks_port = arg1;
}

void PrefrencesWindow::on_httpPortLE_valueChanged(int arg1)
{
    NEEDRESTART
    CurrentConfig.inboundConfig.http_port = arg1;
}

void PrefrencesWindow::on_socksUDPCB_stateChanged(int arg1)
{
    NEEDRESTART
    CurrentConfig.inboundConfig.socksUDP = arg1 == Qt::Checked;
    socksUDPIP->setEnabled(arg1 == Qt::Checked);
}

void PrefrencesWindow::on_socksUDPIP_textEdited(const QString &arg1)
{
    NEEDRESTART
    CurrentConfig.inboundConfig.socksLocalIP = arg1.toStdString();
}

// ------------------- NET SPEED PLUGIN OPERATIONS -----------------------------------------------------------------

#define CurrentBarPage CurrentConfig.toolBarConfig.Pages[this->CurrentBarPageId]
#define CurrentBarLine CurrentBarPage.Lines[this->CurrentBarLineId]
#define SET_LINE_LIST_TEXT nsBarLinesList->currentItem()->setText(GetBarLineDescription(CurrentBarLine));

void PrefrencesWindow::on_nsBarPageAddBTN_clicked()
{
    QvBarPage page;
    CurrentConfig.toolBarConfig.Pages.push_back(page);
    CurrentBarPageId = CurrentConfig.toolBarConfig.Pages.size() - 1 ;
    // Add default line.
    QvBarLine line;
    CurrentBarPage.Lines.push_back(line);
    CurrentBarLineId = 0;
    nsBarPagesList->addItem(QString::number(CurrentBarPageId));
    ShowLineParameters(CurrentBarLine);
    LOG(MODULE_UI, "Adding new page Id: " + to_string(CurrentBarPageId))
    nsBarPageDelBTN->setEnabled(true);
    nsBarLineAddBTN->setEnabled(true);
    nsBarLineDelBTN->setEnabled(true);
    nsBarLinesList->setEnabled(true);
    nsBarPageYOffset->setEnabled(true);
    on_nsBarPagesList_currentRowChanged(static_cast<int>(CurrentBarPageId));
    nsBarPagesList->setCurrentRow(static_cast<int>(CurrentBarPageId));
}

void PrefrencesWindow::on_nsBarPageDelBTN_clicked()
{
    if (nsBarPagesList->currentRow() >= 0) {
        RemoveItem(CurrentConfig.toolBarConfig.Pages, static_cast<size_t>(nsBarPagesList->currentRow()));
        nsBarPagesList->takeItem(nsBarPagesList->currentRow());

        if (nsBarPagesList->count() <= 0) {
            nsBarPageDelBTN->setEnabled(false);
            nsBarLineAddBTN->setEnabled(false);
            nsBarLineDelBTN->setEnabled(false);
            nsBarLinesList->setEnabled(false);
            nsBarVerticalLayout->setEnabled(false);
            nsBarPageYOffset->setEnabled(false);
            nsBarLinesList->clear();
        }
    }
}

void PrefrencesWindow::on_nsBarPageYOffset_valueChanged(int arg1)
{
    LOADINGCHECK
    CurrentBarPage.OffsetYpx = arg1;
}

void PrefrencesWindow::on_nsBarLineAddBTN_clicked()
{
    // WARNING Is it really just this simple?
    QvBarLine line;
    CurrentBarPage.Lines.push_back(line);
    CurrentBarLineId = CurrentBarPage.Lines.size() - 1;
    nsBarLinesList->addItem(QString::number(CurrentBarLineId));
    ShowLineParameters(CurrentBarLine);
    nsBarLineDelBTN->setEnabled(true);
    LOG(MODULE_UI, "Adding new line Id: " + to_string(CurrentBarLineId))
    nsBarLinesList->setCurrentRow(static_cast<int>(CurrentBarPage.Lines.size() - 1));
}

void PrefrencesWindow::on_nsBarLineDelBTN_clicked()
{
    if (nsBarLinesList->currentRow() >= 0) {
        RemoveItem(CurrentBarPage.Lines, static_cast<size_t>(nsBarLinesList->currentRow()));
        nsBarLinesList->takeItem(nsBarLinesList->currentRow());
        CurrentBarLineId = 0;

        if (nsBarLinesList->count() <= 0) {
            nsBarVerticalLayout->setEnabled(false);
            nsBarLineDelBTN->setEnabled(false);
        }

        // TODO Disabling some UI;
    }
}

void PrefrencesWindow::on_nsBarPagesList_currentRowChanged(int currentRow)
{
    if (currentRow < 0) return;

    // Change page.
    // We reload the lines
    // Set all parameters item to the property of the first line.
    CurrentBarPageId = static_cast<size_t>(currentRow);
    CurrentBarLineId = 0;
    nsBarPageYOffset->setValue(CurrentBarPage.OffsetYpx);
    nsBarLinesList->clear();

    if (!CurrentBarPage.Lines.empty()) {
        for (auto line : CurrentBarPage.Lines) {
            auto description = GetBarLineDescription(line);
            nsBarLinesList->addItem(description);
        }

        nsBarLinesList->setCurrentRow(0);
        ShowLineParameters(CurrentBarLine);
    } else {
        nsBarVerticalLayout->setEnabled(false);
    }
}

void PrefrencesWindow::on_nsBarLinesList_currentRowChanged(int currentRow)
{
    if (currentRow < 0) return;

    CurrentBarLineId = static_cast<size_t>(currentRow);
    ShowLineParameters(CurrentBarLine);
}

void PrefrencesWindow::on_fontComboBox_currentFontChanged(const QFont &f)
{
    LOADINGCHECK
    CurrentBarLine.Family = f.family().toStdString();
    SET_LINE_LIST_TEXT
}

void PrefrencesWindow::on_nsBarFontBoldCB_stateChanged(int arg1)
{
    LOADINGCHECK
    CurrentBarLine.Bold = arg1 == Qt::Checked;
    SET_LINE_LIST_TEXT
}

void PrefrencesWindow::on_nsBarFontItalicCB_stateChanged(int arg1)
{
    LOADINGCHECK
    CurrentBarLine.Italic = arg1 == Qt::Checked;
    SET_LINE_LIST_TEXT
}

void PrefrencesWindow::on_nsBarFontASB_valueChanged(int arg1)
{
    LOADINGCHECK
    CurrentBarLine.ColorA = arg1;
    ShowLineParameters(CurrentBarLine);
    SET_LINE_LIST_TEXT
}

void PrefrencesWindow::on_nsBarFontRSB_valueChanged(int arg1)
{
    LOADINGCHECK
    CurrentBarLine.ColorR = arg1;
    ShowLineParameters(CurrentBarLine);
    SET_LINE_LIST_TEXT
}

void PrefrencesWindow::on_nsBarFontGSB_valueChanged(int arg1)
{
    LOADINGCHECK
    CurrentBarLine.ColorG = arg1;
    ShowLineParameters(CurrentBarLine);
    SET_LINE_LIST_TEXT
}

void PrefrencesWindow::on_nsBarFontBSB_valueChanged(int arg1)
{
    LOADINGCHECK
    CurrentBarLine.ColorB = arg1;
    ShowLineParameters(CurrentBarLine);
    SET_LINE_LIST_TEXT
}

void PrefrencesWindow::on_nsBarFontSizeSB_valueChanged(double arg1)
{
    LOADINGCHECK
    CurrentBarLine.Size = arg1;
    SET_LINE_LIST_TEXT
}

QString PrefrencesWindow::GetBarLineDescription(QvBarLine barLine)
{
    QString result = "Empty";
    result = NetSpeedPluginMessages[barLine.ContentType];

    if (barLine.ContentType == 0) {
        result +=  " (" + QSTRING(barLine.Message) + ")";
    }

    result = result.append(barLine.Bold ?  ", " + tr("Bold") : "");
    result = result.append(barLine.Italic ? ", " + tr("Italic") : "");
    return result;
}

void PrefrencesWindow::ShowLineParameters(QvBarLine &barLine)
{
    finishedLoading = false;

    if (!barLine.Family.empty()) {
        fontComboBox->setCurrentFont(QFont(QSTRING(barLine.Family)));
    }

    // Colors
    nsBarFontASB->setValue(barLine.ColorA);
    nsBarFontBSB->setValue(barLine.ColorB);
    nsBarFontGSB->setValue(barLine.ColorG);
    nsBarFontRSB->setValue(barLine.ColorR);
    //
    QColor color = QColor::fromRgb(barLine.ColorR, barLine.ColorG, barLine.ColorB, barLine.ColorA);
    QString s("background: #"
              + QString(color.red() < 16 ? "0" : "") + QString::number(color.red(), 16)
              + QString(color.green() < 16 ? "0" : "") + QString::number(color.green(), 16)
              + QString(color.blue() < 16 ? "0" : "") + QString::number(color.blue(), 16) + ";");
    chooseColorBtn->setStyleSheet(s);
    nsBarFontSizeSB->setValue(barLine.Size);
    nsBarFontBoldCB->setChecked(barLine.Bold);
    nsBarFontItalicCB->setChecked(barLine.Italic);
    nsBarContentCombo->setCurrentText(NetSpeedPluginMessages[barLine.ContentType]);
    nsBarTagTxt->setText(QSTRING(barLine.Message));
    finishedLoading = true;
    nsBarVerticalLayout->setEnabled(true);
}

void PrefrencesWindow::on_chooseColorBtn_clicked()
{
    LOADINGCHECK
    QColorDialog d(QColor::fromRgb(CurrentBarLine.ColorR, CurrentBarLine.ColorG, CurrentBarLine.ColorB, CurrentBarLine.ColorA), this);
    d.exec();

    if (d.result() == QDialog::DialogCode::Accepted) {
        d.selectedColor().getRgb(&CurrentBarLine.ColorR, &CurrentBarLine.ColorG, &CurrentBarLine.ColorB, &CurrentBarLine.ColorA);
        ShowLineParameters(CurrentBarLine);
        SET_LINE_LIST_TEXT
    }
}

void PrefrencesWindow::on_nsBarTagTxt_textEdited(const QString &arg1)
{
    LOADINGCHECK
    CurrentBarLine.Message = arg1.toStdString();
    SET_LINE_LIST_TEXT
}

void PrefrencesWindow::on_nsBarContentCombo_currentIndexChanged(const QString &arg1)
{
    LOADINGCHECK
    CurrentBarLine.ContentType = NetSpeedPluginMessages.key(arg1);
    SET_LINE_LIST_TEXT
}

void PrefrencesWindow::on_applyNSBarSettingsBtn_clicked()
{
    auto conf = GetGlobalConfig();
    conf.toolBarConfig = CurrentConfig.toolBarConfig;
    SetGlobalConfig(conf);
}

void PrefrencesWindow::on_themeCombo_currentTextChanged(const QString &arg1)
{
    LOADINGCHECK
    CurrentConfig.uiConfig.theme = arg1.toStdString();
}

void PrefrencesWindow::on_darkThemeCB_stateChanged(int arg1)
{
    LOADINGCHECK
    CurrentConfig.uiConfig.useDarkTheme = arg1 == Qt::Checked;
    QvMessageBox(this, tr("Dark Mode"), tr("Please restart Qv2ray to fully apply this feature."));
#if QV2RAY_USE_BUILTIN_DARKTHEME
    themeCombo->setEnabled(arg1 != Qt::Checked);

    if (arg1 == Qt::Checked) {
        themeCombo->setCurrentIndex(QStyleFactory::keys().indexOf("Fusion"));
        CurrentConfig.uiConfig.theme = "Fusion";
    }

#endif
}

void PrefrencesWindow::on_darkTrayCB_stateChanged(int arg1)
{
    LOADINGCHECK
    CurrentConfig.uiConfig.useDarkTrayIcon = arg1 == Qt::Checked;
}

void PrefrencesWindow::on_enablePACCB_stateChanged(int arg1)
{
    LOADINGCHECK
    NEEDRESTART
    bool enabled = arg1 == Qt::Checked;
    CurrentConfig.inboundConfig.pacConfig.enablePAC = enabled;
    pacGroupBox->setEnabled(enabled);
}

void PrefrencesWindow::on_pacGoBtn_clicked()
{
    LOADINGCHECK
    QString gfwLocation;
    QString fileContent;
    auto request = new QvHttpRequestHelper();

    switch (gfwListCB->currentIndex()) {
        case 0:
            gfwLocation = "https://raw.githubusercontent.com/gfwlist/gfwlist/master/gfwlist.txt";
            fileContent = QString::fromUtf8(request->syncget(gfwLocation));
            break;

        case 1:
            gfwLocation = "https://pagure.io/gfwlist/raw/master/f/gfwlist.txt";
            fileContent = QString::fromUtf8(request->syncget(gfwLocation));
            break;

        case 2:
            gfwLocation = "http://repo.or.cz/gfwlist.git/blob_plain/HEAD:/gfwlist.txt";
            fileContent = QString::fromUtf8(request->syncget(gfwLocation));
            break;

        case 3:
            gfwLocation = "https://bitbucket.org/gfwlist/gfwlist/raw/HEAD/gfwlist.txt";
            fileContent = QString::fromUtf8(request->syncget(gfwLocation));
            break;

        case 4:
            gfwLocation = "https://gitlab.com/gfwlist/gfwlist/raw/master/gfwlist.txt";
            fileContent = QString::fromUtf8(request->syncget(gfwLocation));
            break;

        case 5:
            gfwLocation = "https://git.tuxfamily.org/gfwlist/gfwlist.git/plain/gfwlist.txt";
            fileContent = QString::fromUtf8(request->syncget(gfwLocation));
            break;

        case 6:
            QFileDialog d;
            d.exec();
            auto file = d.getOpenFileUrl(this, tr("Select GFWList in base64")).toString();
            //
            fileContent = StringFromFile(new QFile(file));
            break;
    }

    //
    if (!QDir(QV2RAY_RULES_DIR).exists()) {
        QDir(QV2RAY_RULES_DIR).mkpath(QV2RAY_RULES_DIR);
    }

    QFile privateGFWListFile(QV2RAY_RULES_GFWLIST_PATH);
    StringToFile(&fileContent, &privateGFWListFile);
}

void PrefrencesWindow::on_pacPortSB_valueChanged(int arg1)
{
    LOADINGCHECK
    NEEDRESTART
    CurrentConfig.inboundConfig.pacConfig.port = arg1;
    //pacAccessPathTxt->setText("http://" + listenIPTxt->text() + ":" + QString::number(arg1) + "/pac");
}

void PrefrencesWindow::on_setSysProxyCB_stateChanged(int arg1)
{
    LOADINGCHECK
    NEEDRESTART
    CurrentConfig.inboundConfig.setSystemProxy = arg1 == Qt::Checked;
}

void PrefrencesWindow::on_pacProxyCB_currentIndexChanged(int index)
{
    LOADINGCHECK
    NEEDRESTART
    // 0 -> http
    // 1 -> socks
    CurrentConfig.inboundConfig.pacConfig.useSocksProxy = index == 1;
}

void PrefrencesWindow::on_pushButton_clicked()
{
    LOADINGCHECK
    QDesktopServices::openUrl(QUrl::fromUserInput(QV2RAY_RULES_DIR));
}

void PrefrencesWindow::on_pacProxyTxt_textEdited(const QString &arg1)
{
    LOADINGCHECK
    NEEDRESTART
    CurrentConfig.inboundConfig.pacConfig.localIP = arg1.toStdString();
}

void PrefrencesWindow::on_autoStartSubsCombo_currentIndexChanged(const QString &arg1)
{
    LOADINGCHECK
    CurrentConfig.autoStartConfig.subscriptionName = arg1.toStdString();
    autoStartConnCombo->clear();

    if (arg1.isEmpty()) {
        autoStartConnCombo->addItem("");
        autoStartConnCombo->addItems(ConvertQStringList(QList<string>::fromStdList(CurrentConfig.configs)));
    } else {
        auto list = GetSubscriptionConnection(arg1.toStdString());
        autoStartConnCombo->addItems(list.keys());
    }
}

void PrefrencesWindow::on_autoStartConnCombo_currentIndexChanged(const QString &arg1)
{
    LOADINGCHECK
    CurrentConfig.autoStartConfig.connectionName = arg1.toStdString();
}
