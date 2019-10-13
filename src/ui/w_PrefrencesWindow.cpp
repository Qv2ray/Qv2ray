#include "QvUtils.h"
#include "QvCoreInteractions.h"
#include "w_PrefrencesWindow.h"
#include <QFileDialog>

#include <iostream>

#define NEEDRESTART if(finishedLoading) IsConnectionPropertyChanged = true;

PrefrencesWindow::PrefrencesWindow(QWidget *parent) : QDialog(parent),
    CurrentConfig(),
    ui(new Ui::PrefrencesWindow)
{
    ui->setupUi(this);
    // We add locales
    ui->languageComboBox->clear();
    QDirIterator it(":/translations");

    while (it.hasNext()) {
        ui->languageComboBox->addItem(it.next().split("/").last().split(".").first());
    }

    //
    ui->qvVersion->setText(QV2RAY_VERSION_STRING);
    CurrentConfig = GetGlobalConfig();
    //
    ui->languageComboBox->setCurrentText(QSTRING(CurrentConfig.language));
    ui->logLevelComboBox->setCurrentIndex(CurrentConfig.logLevel);
    ui->tProxyCheckBox->setChecked(CurrentConfig.tProxySupport);
    //
    //
    ui->listenIPTxt->setText(QSTRING(CurrentConfig.inBoundSettings.listenip));
    //
    bool have_http = CurrentConfig.inBoundSettings.http_port != 0;
    ui->httpCB->setChecked(have_http);
    ui->httpPortLE->setValue(CurrentConfig.inBoundSettings.http_port);
    ui->httpAuthCB->setChecked(CurrentConfig.inBoundSettings.http_useAuth);
    //
    ui->httpAuthCB->setEnabled(have_http);
    ui->httpAuthCB->setChecked(CurrentConfig.inBoundSettings.http_useAuth);
    ui->httpAuthUsernameTxt->setEnabled(have_http && CurrentConfig.inBoundSettings.http_useAuth);
    ui->httpAuthPasswordTxt->setEnabled(have_http && CurrentConfig.inBoundSettings.http_useAuth);
    ui->httpAuthUsernameTxt->setText(QSTRING(CurrentConfig.inBoundSettings.httpAccount.user));
    ui->httpAuthPasswordTxt->setText(QSTRING(CurrentConfig.inBoundSettings.httpAccount.pass));
    //
    //
    bool have_socks = CurrentConfig.inBoundSettings.socks_port != 0;
    ui->socksCB->setChecked(have_socks);
    ui->socksPortLE->setValue(CurrentConfig.inBoundSettings.socks_port);
    ui->socksAuthCB->setChecked(CurrentConfig.inBoundSettings.socks_useAuth);
    //
    ui->socksAuthCB->setEnabled(have_socks);
    ui->socksAuthCB->setChecked(CurrentConfig.inBoundSettings.socks_useAuth);
    ui->socksAuthUsernameTxt->setEnabled(have_socks && CurrentConfig.inBoundSettings.socks_useAuth);
    ui->socksAuthPasswordTxt->setEnabled(have_socks && CurrentConfig.inBoundSettings.socks_useAuth);
    ui->socksAuthUsernameTxt->setText(QSTRING(CurrentConfig.inBoundSettings.socksAccount.user));
    ui->socksAuthPasswordTxt->setText(QSTRING(CurrentConfig.inBoundSettings.socksAccount.pass));
    // Socks UDP Options
    ui->socksUDPCB->setChecked(CurrentConfig.inBoundSettings.socksUDP);
    ui->socksUDPIP->setEnabled(CurrentConfig.inBoundSettings.socksUDP);
    ui->socksUDPIP->setText(QSTRING(CurrentConfig.inBoundSettings.socksLocalIP));
    //
    //
    ui->vCoreAssetsPathTxt->setText(QSTRING(CurrentConfig.v2AssetsPath));
    ui->statsCheckbox->setChecked(CurrentConfig.enableStats);
    ui->statsPortBox->setValue(CurrentConfig.statsPort);
    //
    //
#if false
    ui->muxEnabledCB->setChecked(CurrentConfig.mux.enabled);
    ui->muxConcurrencyTxt->setValue(CurrentConfig.mux.concurrency);
#endif
    //
    //
    ui->bypassCNCb->setChecked(CurrentConfig.bypassCN);
    ui->proxyDefaultCb->setChecked(CurrentConfig.enableProxy);
    //
    ui->localDNSCb->setChecked(CurrentConfig.withLocalDNS);
    //
    ui->DNSListTxt->clear();

    foreach (auto dnsStr, CurrentConfig.dnsList) {
        auto str = QString::fromStdString(dnsStr).trimmed();

        if (!str.isEmpty()) {
            ui->DNSListTxt->appendPlainText(str);
        }
    }

    foreach (auto connection, CurrentConfig.configs) {
        ui->autoStartCombo->addItem(QSTRING(connection));
    }

    ui->autoStartCombo->setCurrentText(QSTRING(CurrentConfig.autoStartConfig));
    ui->cancelIgnoreVersionBtn->setEnabled(CurrentConfig.ignoredVersion != "");
    ui->ignoredNextVersion->setText(QSTRING(CurrentConfig.ignoredVersion));
    finishedLoading = true;
}

PrefrencesWindow::~PrefrencesWindow()
{
    delete ui;
}

void PrefrencesWindow::on_buttonBox_accepted()
{
    int sp = ui->socksPortLE->text().toInt();
    int hp = ui->httpPortLE->text().toInt() ;

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
    ui->httpPortLE->setEnabled(checked == Qt::Checked);
    ui->httpAuthCB->setEnabled(checked == Qt::Checked);
    ui->httpAuthUsernameTxt->setEnabled(checked == Qt::Checked && ui->httpAuthCB->isChecked());
    ui->httpAuthPasswordTxt->setEnabled(checked == Qt::Checked && ui->httpAuthCB->isChecked());
    CurrentConfig.inBoundSettings.http_port = checked == Qt::Checked ? CurrentConfig.inBoundSettings.http_port : 0;

    if (checked != Qt::Checked) {
        ui->httpPortLE->setValue(0);
    }
}

void PrefrencesWindow::on_socksCB_stateChanged(int checked)
{
    NEEDRESTART
    ui->socksPortLE->setEnabled(checked == Qt::Checked);
    ui->socksAuthCB->setEnabled(checked == Qt::Checked);
    ui->socksAuthUsernameTxt->setEnabled(checked == Qt::Checked && ui->socksAuthCB->isChecked());
    ui->socksAuthPasswordTxt->setEnabled(checked == Qt::Checked && ui->socksAuthCB->isChecked());
    CurrentConfig.inBoundSettings.socks_port = checked == Qt::Checked ? CurrentConfig.inBoundSettings.socks_port : 0;

    if (checked != Qt::Checked) {
        ui->socksPortLE->setValue(0);
    }
}

void PrefrencesWindow::on_httpAuthCB_stateChanged(int checked)
{
    NEEDRESTART
    ui->httpAuthUsernameTxt->setEnabled(checked == Qt::Checked);
    ui->httpAuthPasswordTxt->setEnabled(checked == Qt::Checked);
    CurrentConfig.inBoundSettings.http_useAuth = checked == Qt::Checked;
}

void PrefrencesWindow::on_socksAuthCB_stateChanged(int checked)
{
    NEEDRESTART
    ui->socksAuthUsernameTxt->setEnabled(checked == Qt::Checked);
    ui->socksAuthPasswordTxt->setEnabled(checked == Qt::Checked);
    CurrentConfig.inBoundSettings.socks_useAuth = checked == Qt::Checked;
}

void PrefrencesWindow::on_languageComboBox_currentTextChanged(const QString &arg1)
{
    CurrentConfig.language = arg1.toStdString();
    //
    // A strange bug prevents us to change the UI language online
    //    https://github.com/lhy0403/Qv2ray/issues/34
    //
    //if (QApplication::installTranslator(getTranslator(&arg1))) {
    //    LOG(MODULE_UI, "Loaded translations " + arg1.toStdString())
    //    ui->retranslateUi(this);
    //} else {
    //    QvMessageBox(this, tr("#Prefrences"), tr("#SwitchTranslationError"));
    //}
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

void PrefrencesWindow::on_muxEnabledCB_stateChanged(int arg1)
{
    NEEDRESTART
    CurrentConfig.mux.enabled = arg1 == Qt::Checked;
}

void PrefrencesWindow::on_muxConcurrencyTxt_valueChanged(int arg1)
{
    NEEDRESTART
    CurrentConfig.mux.concurrency = arg1;
}

void PrefrencesWindow::on_listenIPTxt_textEdited(const QString &arg1)
{
    NEEDRESTART
    CurrentConfig.inBoundSettings.listenip = arg1.toStdString();
}

void PrefrencesWindow::on_httpAuthUsernameTxt_textEdited(const QString &arg1)
{
    NEEDRESTART
    CurrentConfig.inBoundSettings.httpAccount.user = arg1.toStdString();
}

void PrefrencesWindow::on_httpAuthPasswordTxt_textEdited(const QString &arg1)
{
    NEEDRESTART
    CurrentConfig.inBoundSettings.httpAccount.pass = arg1.toStdString();
}

void PrefrencesWindow::on_socksAuthUsernameTxt_textEdited(const QString &arg1)
{
    NEEDRESTART
    CurrentConfig.inBoundSettings.socksAccount.user = arg1.toStdString();
}

void PrefrencesWindow::on_socksAuthPasswordTxt_textEdited(const QString &arg1)
{
    NEEDRESTART
    CurrentConfig.inBoundSettings.socksAccount.pass = arg1.toStdString();
}

void PrefrencesWindow::on_proxyDefaultCb_stateChanged(int arg1)
{
    NEEDRESTART
    CurrentConfig.enableProxy = arg1 == Qt::Checked;
}

void PrefrencesWindow::on_localDNSCb_stateChanged(int arg1)
{
    NEEDRESTART
    CurrentConfig.withLocalDNS = arg1 == Qt::Checked;
}

void PrefrencesWindow::on_selectVAssetBtn_clicked()
{
    NEEDRESTART
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open v2ray assets folder"), QDir::currentPath());
    ui->vCoreAssetsPathTxt->setText(dir);
    on_vCoreAssetsPathTxt_textEdited(dir);
}

void PrefrencesWindow::on_DNSListTxt_textChanged()
{
    if (finishedLoading) {
        try {
            QStringList hosts = ui->DNSListTxt->toPlainText().replace("\r", "").split("\n");
            CurrentConfig.dnsList.clear();

            foreach (auto host, hosts) {
                if (host != "" && host != "\r") {
                    // Not empty, so we save.
                    CurrentConfig.dnsList.push_back(host.toStdString());
                    NEEDRESTART
                }
            }

            BLACK(DNSListTxt)
        } catch (...) {
            RED(DNSListTxt)
        }
    }
}

void PrefrencesWindow::on_autoStartCombo_currentTextChanged(const QString &arg1)
{
    CurrentConfig.autoStartConfig = arg1.toStdString();
}

void PrefrencesWindow::on_aboutQt_clicked()
{
    QApplication::aboutQt();
}

void PrefrencesWindow::on_cancelIgnoreVersionBtn_clicked()
{
    CurrentConfig.ignoredVersion.clear();
    ui->cancelIgnoreVersionBtn->setEnabled(false);
}

void PrefrencesWindow::on_tProxyCheckBox_stateChanged(int arg1)
{
#ifdef __linux

    if (finishedLoading) {
        //LOG(MODULE_UI, "Running getcap....")
        //QProcess::execute("getcap " + QV2RAY_V2RAY_CORE_PATH);

        // Set UID and GID for linux
        // Steps:
        // --> 1. Copy v2ray core files to the #CONFIG_DIR#/vcore/ dir.
        // --> 2. Change GlobalConfig.v2CorePath.
        // --> 3. Call `pkexec setcap CAP_NET_ADMIN,CAP_NET_RAW,CAP_NET_BIND_SERVICE=eip` on the v2ray core.
        if (arg1 == Qt::Checked) {
            // We enable it!
            if (QvMessageBoxAsk(this, tr("Enable tProxy Support"), tr("This will append capabilities to the v2ray executable.")  + "\r\n"
                                + tr("If anything goes wrong after enabling this, please refer to issue #57 or the link below:") + "\r\n" +
                                " https://github.com/lhy0403/Qv2ray/blob/master/docs/FAQ.md ") != QMessageBox::Yes) {
                ui->tProxyCheckBox->setChecked(false);
                LOG(MODULE_UI, "Canceled enabling tProxy feature.")
            }

            int ret = QProcess::execute("pkexec setcap CAP_NET_ADMIN,CAP_NET_RAW,CAP_NET_BIND_SERVICE=eip " + QV2RAY_V2RAY_CORE_PATH);

            if (ret != 0) {
                LOG(MODULE_UI, "WARN: setcap exits with code: " + to_string(ret))
                QvMessageBox(this, tr("Prefrences"), tr("Failed to setcap onto v2ray executable. You may need to run `setcap` manually."));
            }

            CurrentConfig.tProxySupport = true;
            NEEDRESTART
        } else {
            int ret = QProcess::execute("pkexec setcap -r " + QV2RAY_V2RAY_CORE_PATH);

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
    ui->tProxyCheckBox->setChecked(false);
    // No such uid gid thing on Windows and macOS
    QvMessageBox(this, tr("Prefrences"), tr("tProxy is not supported on macOS and Windows"));
#endif
}
void PrefrencesWindow::on_bypassCNCb_stateChanged(int arg1)
{
    NEEDRESTART
    CurrentConfig.bypassCN = arg1 == Qt::Checked;
}

void PrefrencesWindow::on_statsCheckbox_stateChanged(int arg1)
{
    NEEDRESTART
    CurrentConfig.enableStats = arg1 == Qt::Checked;
}

void PrefrencesWindow::on_statsPortBox_valueChanged(int arg1)
{
    NEEDRESTART
    CurrentConfig.statsPort = arg1;
}

void PrefrencesWindow::on_socksPortLE_valueChanged(int arg1)
{
    NEEDRESTART
    CurrentConfig.inBoundSettings.socks_port = arg1;
}

void PrefrencesWindow::on_httpPortLE_valueChanged(int arg1)
{
    NEEDRESTART
    CurrentConfig.inBoundSettings.http_port = arg1;
}

void PrefrencesWindow::on_socksUDPCB_stateChanged(int arg1)
{
    NEEDRESTART
    CurrentConfig.inBoundSettings.socksUDP = arg1 == Qt::Checked;
    ui->socksUDPIP->setEnabled(arg1 == Qt::Checked);
}

void PrefrencesWindow::on_socksUDPIP_textEdited(const QString &arg1)
{
    NEEDRESTART
    CurrentConfig.inBoundSettings.socksLocalIP = arg1.toStdString();
}
