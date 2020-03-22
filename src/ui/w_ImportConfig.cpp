#include "w_ImportConfig.hpp"

#include "3rdparty/qzxing/src/QZXing.h"
#include "core/CoreUtils.hpp"
#include "core/connection/ConnectionIO.hpp"
#include "core/connection/Serialization.hpp"
#include "core/kernel/KernelInteractions.hpp"
#include "ui/editors/w_JsonEditor.hpp"
#include "ui/editors/w_OutboundEditor.hpp"
#include "ui/editors/w_RoutesEditor.hpp"
#include "ui/w_SubscriptionManager.hpp"
#include "w_ScreenShot_Core.hpp"

#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QThread>

ImportConfigWindow::ImportConfigWindow(QWidget *parent) : QDialog(parent)
{
    setupUi(this);
    // nameTxt->setText(tr("My Connection Imported at: ") + QDateTime::currentDateTime().toString("MM-dd hh:mm"));
    QvMessageBusConnect(ImportConfigWindow);
    RESTORE_RUNTIME_CONFIG(screenShotHideQv2ray, hideQv2rayCB->setChecked)
}

void ImportConfigWindow::UpdateColorScheme()
{
    // Stub
}

QvMessageBusSlotImpl(ImportConfigWindow)
{
    switch (msg)
    {
        MBShowDefaultImpl MBHideDefaultImpl MBRetranslateDefaultImpl MBUpdateColorSchemeDefaultImpl
    }
}

ImportConfigWindow::~ImportConfigWindow()
{
}

QMultiMap<QString, CONFIGROOT> ImportConfigWindow::OpenImport(bool partialImport)
{
    // partial import means only import as an outbound, will set keepImported to
    // false and disable the checkbox
    // keepImportedInboundCheckBox->setChecked(!outboundsOnly);
    keepImportedInboundCheckBox->setEnabled(!partialImport);
    routeEditBtn->setEnabled(!partialImport);
    this->exec();
    return this->result() == QDialog::Accepted ? connections : QMultiMap<QString, CONFIGROOT>();
}

void ImportConfigWindow::on_selectFileBtn_clicked()
{
    QString dir = QFileDialog::getOpenFileName(this, tr("Select file to import"));
    fileLineTxt->setText(dir);
}

void ImportConfigWindow::on_qrFromScreenBtn_clicked()
{
    bool hideQv2ray = hideQv2rayCB->isChecked();

    if (hideQv2ray)
    {
        UIMessageBus.EmitGlobalSignal(QvMBMessage::HIDE_WINDOWS);
    }

    QApplication::processEvents();
    QThread::msleep(static_cast<ulong>(doubleSpinBox->value() * 1000));
    ScreenShotWindow w;
    auto pix = w.DoScreenShot();
    auto _r = w.result();

    if (hideQv2ray)
    {
        UIMessageBus.EmitGlobalSignal(QvMBMessage::SHOW_WINDOWS);
    }

    if (_r == QDialog::Accepted)
    {
        QZXing qzxing;
        qzxing.setDecoder(QZXing::DecoderFormat_QR_CODE | QZXing::DecoderFormat_EAN_13);
        auto str = qzxing.decodeImage(pix);

        if (str.trimmed().isEmpty())
        {
            LOG(MODULE_UI, "Cannot decode QR Code from an image, size: h=" + QSTRN(pix.width()) + ", v=" + QSTRN(pix.height()))
            QvMessageBoxWarn(this, tr("Capture QRCode"), tr("Cannot find a valid QRCode from this region."));
        }
        else
        {
            vmessConnectionStringTxt->appendPlainText(str.trimmed() + NEWLINE);
        }
    }
}

void ImportConfigWindow::on_beginImportBtn_clicked()
{
    QString aliasPrefix = nameTxt->text();

    switch (tabWidget->currentIndex())
    {
        case 0:
        {
            QStringList linkList = SplitLines(vmessConnectionStringTxt->toPlainText());
            //
            // Clear UI and error lists
            linkErrors.clear();
            vmessConnectionStringTxt->clear();
            errorsList->clear();
            //
            LOG(MODULE_IMPORT, QSTRN(linkList.count()) + " string found in vmess box.")

            while (!linkList.isEmpty())
            {
                aliasPrefix = nameTxt->text();
                auto link = linkList.takeFirst();
                if (link.trimmed().isEmpty() || link.startsWith("#") || link.startsWith("//"))
                {
                    continue;
                }
                QString errMessage;
                const CONFIGROOT config = ConvertConfigFromString(link, &aliasPrefix, &errMessage);

                // If the config is empty or we have any err messages.
                if (config.isEmpty() || !errMessage.isEmpty())
                {
                    // To prevent duplicated values.
                    linkErrors[link] = QSTRN(linkErrors.count() + 1) + ": " + errMessage;
                    continue;
                }
                else
                {
                    connections.insert(aliasPrefix, config);
                }
            }

            if (!linkErrors.isEmpty())
            {
                for (auto item : linkErrors)
                {
                    vmessConnectionStringTxt->appendPlainText(linkErrors.key(item));
                    errorsList->addItem(item);
                }

                vmessConnectionStringTxt->setLineWidth(errorsList->lineWidth());
                errorsList->sortItems();
                return;
            }

            break;
        }
        case 2:
        {
            // From File...
            bool ImportAsComplex = keepImportedInboundCheckBox->isChecked();
            QString path = fileLineTxt->text();

            if (!V2rayKernelInstance::ValidateConfig(path))
            {
                QvMessageBoxWarn(this, tr("Import config file"), tr("Failed to check the validity of the config file."));
                return;
            }

            aliasPrefix += "_" + QFileInfo(path).fileName();
            CONFIGROOT config = ConvertConfigFromFile(path, ImportAsComplex);
            connections.insert(aliasPrefix, config);
            break;
        }
    }

    accept();
}
void ImportConfigWindow::on_selectImageBtn_clicked()
{
    QString dir = QFileDialog::getOpenFileName(this, tr("Select an image to import"));
    imageFileEdit->setText(dir);
    //
    QFile file(dir);
    file.open(QFile::OpenModeFlag::ReadOnly);
    auto buf = file.readAll();
    file.close();
    //
    QZXing decoder;
    decoder.setDecoder(QZXing::DecoderFormat_QR_CODE | QZXing::DecoderFormat_EAN_13);
    auto str = decoder.decodeImage(QImage::fromData(buf));

    if (str.isEmpty())
    {
        QvMessageBoxWarn(this, tr("QRCode scanning failed"), tr("Cannot find any QRCode from the image."));
        return;
    }
    else
    {
        vmessConnectionStringTxt->appendPlainText(str.trimmed() + NEWLINE);
    }
}
void ImportConfigWindow::on_errorsList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    Q_UNUSED(previous)

    if (current == nullptr)
    {
        return;
    }

    auto currentErrorText = current->text();
    auto vmessEntry = linkErrors.key(currentErrorText);
    //
    auto startPos = vmessConnectionStringTxt->toPlainText().indexOf(vmessEntry);
    auto endPos = startPos + vmessEntry.length();

    if (startPos < 0)
    {
        return;
    }

    // Select vmess string that is invalid.
    QTextCursor c = vmessConnectionStringTxt->textCursor();
    c.setPosition(startPos);
    c.setPosition(endPos, QTextCursor::KeepAnchor);
    vmessConnectionStringTxt->setTextCursor(c);
}

void ImportConfigWindow::on_connectionEditBtn_clicked()
{
    OutboundEditor w(this);
    auto outboundEntry = w.OpenEditor();
    bool isChanged = w.result() == QDialog::Accepted;
    QString alias = w.GetFriendlyName();

    if (isChanged)
    {
        OUTBOUNDS outboundsList;
        outboundsList.push_back(outboundEntry);
        CONFIGROOT root;
        root.insert("outbounds", outboundsList);
        //
        connections.insert(alias, root);
        accept();
    }
}

void ImportConfigWindow::on_cancelImportBtn_clicked()
{
    reject();
}

void ImportConfigWindow::on_subscriptionButton_clicked()
{
    hide();
    SubscriptionEditor w;
    w.exec();
    auto importToComplex = !keepImportedInboundCheckBox->isEnabled();
    connections.clear();

    if (importToComplex)
    {
        auto [alias, conf] = w.GetSelectedConfig();
        connections.insert(alias, conf);
    }

    accept();
}

void ImportConfigWindow::on_routeEditBtn_clicked()
{
    RouteEditor w(QJsonObject(), this);
    auto result = w.OpenEditor();
    bool isChanged = w.result() == QDialog::Accepted;
    QString alias = nameTxt->text();

    if (isChanged)
    {
        connections.insert(alias, result);
        accept();
    }
}

void ImportConfigWindow::on_hideQv2rayCB_stateChanged(int arg1)
{
    Q_UNUSED(arg1)
    SET_RUNTIME_CONFIG(screenShotHideQv2ray, hideQv2rayCB->isChecked)
}

void ImportConfigWindow::on_jsonEditBtn_clicked()
{
    JsonEditor w(QJsonObject(), this);
    auto result = w.OpenEditor();
    bool isChanged = w.result() == QDialog::Accepted;
    QString alias = nameTxt->text();

    if (isChanged)
    {
        connections.insert(alias, CONFIGROOT(result));
        accept();
    }
}
