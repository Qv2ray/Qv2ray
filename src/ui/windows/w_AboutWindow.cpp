#include "w_AboutWindow.hpp"

#include "Qv2rayBase/Common/Utils.hpp"
#include "Qv2rayBase/Interfaces/IStorageProvider.hpp"
#include "Qv2rayBase/Qv2rayBaseLibrary.hpp"
#include "QvPlugin/Common/QvPluginBase.hpp"
#include "ui_w_AboutWindow.h"

#include <QFontDatabase>
#include <QSslSocket>

AboutWindow::AboutWindow(QWidget *parent) : QDialog(parent), ui(new Ui::w_AboutWindow)
{
    ui->setupUi(this);
    ui->configdirLabel->setText(QvStorageProvider->StorageLocation());
    ui->qvVersion->setText(QString::fromUtf8(QV2RAY_VERSION_STRING));
    ui->qvBuildInfo->setText(QString::fromUtf8(QV2RAY_BUILD_INFO));
    ui->qvBuildExInfo->setText(QString::fromUtf8(QV2RAY_BUILD_EXTRA_INFO));
    ui->qvPluginInterfaceVersionLabel->setText(QString::number(Qv2rayPlugin::QV2RAY_PLUGIN_INTERFACE_VERSION));
    ui->tlsBackendLabel->setText(QSslSocket::activeBackend() + " (" + QSslSocket::sslLibraryVersionString() + ")");
    // ui->textBrowser->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
}

AboutWindow::~AboutWindow()
{
    delete ui;
}

void AboutWindow::on_openConfigDirCB_clicked()
{
    QvBaselib->OpenURL(QvStorageProvider->StorageLocation());
}

void AboutWindow::on_aboutQt_clicked()
{
    QApplication::aboutQt();
}
