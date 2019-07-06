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
    ui->languageComboBox->setCurrentText(QString::fromStdString(CurrentConfig.language));
    ui->runAsRootCheckBox->setChecked(CurrentConfig.runAsRoot);
    ui->logLevelCheckBox->setCurrentText(QString::fromStdString(CurrentConfig.logLevel));
    //
    ui->httpCB->setChecked(CurrentConfig.httpSetting.enabled);
    ui->httpPortLE->setText(QString::fromStdString(to_string(CurrentConfig.httpSetting.port)));
    ui->httpAuthCB->setChecked(CurrentConfig.httpSetting.useAuthentication);
    ui->httpAuthUsernameTxt->setText(QString::fromStdString(CurrentConfig.httpSetting.authUsername));
    ui->httpAuthPasswordTxt->setText(QString::fromStdString(CurrentConfig.httpSetting.authPassword));
    ui->httpPortLE->setValidator(new QIntValidator());
    //
    ui->socksCB->setChecked(CurrentConfig.socksSetting.enabled);
    ui->socksPortLE->setText(QString::fromStdString(to_string(CurrentConfig.socksSetting.port)));
    ui->socksAuthCB->setChecked(CurrentConfig.socksSetting.useAuthentication);
    ui->socksAuthUsernameTxt->setText(QString::fromStdString(CurrentConfig.socksSetting.authUsername));
    ui->socksAuthPasswordTxt->setText(QString::fromStdString(CurrentConfig.socksSetting.authPassword));
    //
    ui->vCoreExePathTxt->setText(QString::fromStdString(CurrentConfig.v2Path));
    //
    ui->httpPortLE->setValidator(new QIntValidator());
    ui->socksPortLE->setValidator(new QIntValidator());
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
    Q_UNUSED(checked)
    ui->httpAuthUsernameTxt->setEnabled(ui->httpCB->checkState() == Qt::Checked);
    ui->httpAuthPasswordTxt->setEnabled(ui->httpCB->checkState() == Qt::Checked);
}

void PrefrencesWindow::on_runAsRootCheckBox_stateChanged(int arg1)
{
    Q_UNUSED(arg1)
#ifdef _WIN32
    showWarnMessageBox(this, tr("Prefrences"), tr("RunAsRootNotOnWindows"));
#endif
}

void PrefrencesWindow::on_socksAuthCB_stateChanged(int checked)
{
    Q_UNUSED(checked)
    ui->socksAuthUsernameTxt->setEnabled(ui->socksCB->checkState() == Qt::Checked);
    ui->socksAuthPasswordTxt->setEnabled(ui->socksCB->checkState() == Qt::Checked);
}
