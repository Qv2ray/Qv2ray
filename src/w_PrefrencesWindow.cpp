#include "QvUtils.h"
#include "QvCoreInteractions.h"
#include "w_PrefrencesWindow.h"

#ifndef _WIN32
#include <unistd.h>
#endif

PrefrencesWindow::PrefrencesWindow(QWidget *parent) : QDialog(parent),
    CurrentConfig(),
    ui(new Ui::PrefrencesWindow)
{
    ui->setupUi(this);
    CurrentConfig = GetGlobalConfig();
    //
    ui->languageComboBox->setCurrentText(QString::fromStdString(CurrentConfig.language));
    ui->runAsRootCheckBox->setChecked(CurrentConfig.runAsRoot);
    ui->logLevelComboBox->setCurrentIndex(CurrentConfig.logLevel);
    //
    //
    ui->httpCB->setChecked(CurrentConfig.inBoundSettings.http_port != 0);
    ui->httpPortLE->setText(QString::fromStdString(to_string(CurrentConfig.inBoundSettings.http_port)));
    ui->httpAuthCB->setChecked(CurrentConfig.inBoundSettings.http_useAuth);
    //
    ui->httpAuthUsernameTxt->setEnabled(CurrentConfig.inBoundSettings.http_useAuth);
    ui->httpAuthPasswordTxt->setEnabled(CurrentConfig.inBoundSettings.http_useAuth);
    ui->httpAuthUsernameTxt->setText(QString::fromStdString(CurrentConfig.inBoundSettings.httpAccount.user));
    ui->httpAuthPasswordTxt->setText(QString::fromStdString(CurrentConfig.inBoundSettings.httpAccount.pass));
    ui->httpPortLE->setValidator(new QIntValidator());
    //
    //
    ui->socksCB->setChecked(CurrentConfig.inBoundSettings.socks_port != 0);
    ui->socksPortLE->setText(QString::fromStdString(to_string(CurrentConfig.inBoundSettings.socks_port)));
    ui->socksAuthCB->setChecked(CurrentConfig.inBoundSettings.socks_useAuth);
    //
    ui->socksAuthUsernameTxt->setEnabled(CurrentConfig.inBoundSettings.socks_useAuth);
    ui->socksAuthPasswordTxt->setEnabled(CurrentConfig.inBoundSettings.socks_useAuth);
    ui->socksAuthUsernameTxt->setText(QString::fromStdString(CurrentConfig.inBoundSettings.socksAccount.user));
    ui->socksAuthPasswordTxt->setText(QString::fromStdString(CurrentConfig.inBoundSettings.socksAccount.pass));
    ui->socksPortLE->setValidator(new QIntValidator());
    //
    //
    ui->vCoreExePathTxt->setText(QString::fromStdString(CurrentConfig.v2CorePath));
    ui->vCoreAssetsPathTxt->setText(QString::fromStdString(CurrentConfig.v2AssetsPath));
    //
    //
    ui->muxEnabledCB->setChecked(CurrentConfig.mux.enabled);
    ui->muxConcurrencyTxt->setValue(CurrentConfig.mux.concurrency);
    //
    parentMW = parent;
}

PrefrencesWindow::~PrefrencesWindow()
{
    delete ui;
}

void PrefrencesWindow::on_buttonBox_accepted()
{
    if (ui->httpPortLE->text().toInt() != ui->socksPortLE->text().toInt()) {
#ifndef _WIN32
        // Set UID and GID in *nix
        // The file is actually not here
        QFileInfo v2rayCoreExeFile("v2ray");

        if (ui->runAsRootCheckBox->isChecked() && v2rayCoreExeFile.ownerId() != 0) {
            QProcess::execute("pkexec", QStringList() << "bash"
                              << "-c"
                              << "chown root:root " + QCoreApplication::applicationDirPath() + "/v2ray" + ";chmod +s " + QCoreApplication::applicationDirPath() + "/v2ray");
        } else if (!ui->runAsRootCheckBox->isChecked() && v2rayCoreExeFile.ownerId() == 0) {
            uid_t uid = getuid();
            gid_t gid = getgid();
            QProcess::execute("pkexec", QStringList() << "chown" << QString::number(uid) + ":" + QString::number(gid) << QCoreApplication::applicationDirPath() + "/v2ray");
        }

        v2rayCoreExeFile.refresh();
        //rootObj.insert("v2suidEnabled", v2rayCoreExeFile.ownerId() == 0);
#else
        // No such uid gid thing on windows....
#endif
    } else {
        QvMessageBox(this, tr("Prefrences"), tr("PortNumbersCannotBeSame"));
    }
}

void PrefrencesWindow::on_httpCB_stateChanged(int checked)
{
    if (checked != Qt::Checked) {
        ui->httpPortLE->setDisabled(true);
    } else {
        ui->httpPortLE->setEnabled(true);
    }
}

void PrefrencesWindow::on_socksCB_stateChanged(int checked)
{
    if (checked != Qt::Checked) {
        ui->socksPortLE->setEnabled(false);
    } else {
        ui->socksPortLE->setEnabled(true);
    }
}

void PrefrencesWindow::on_httpAuthCB_stateChanged(int checked)
{
    ui->httpAuthUsernameTxt->setEnabled(checked == Qt::Checked);
    ui->httpAuthPasswordTxt->setEnabled(checked == Qt::Checked);
}

void PrefrencesWindow::on_runAsRootCheckBox_stateChanged(int arg1)
{
    Q_UNUSED(arg1)
#ifdef _WIN32
    QvMessageBox(this, tr("Prefrences"), tr("RunAsRootNotOnWindows"));
#endif
}

void PrefrencesWindow::on_socksAuthCB_stateChanged(int checked)
{
    ui->socksAuthUsernameTxt->setEnabled(checked == Qt::Checked);
    ui->socksAuthPasswordTxt->setEnabled(checked == Qt::Checked);
}
