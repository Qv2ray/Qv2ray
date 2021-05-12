#include "w_ImportConfig.hpp"

#include "core/connection/Serialization.hpp"
#include "core/handler/ConfigHandler.hpp"
#include "ui/common/QRCodeHelper.hpp"
#include "ui/widgets/editors/w_JsonEditor.hpp"
#include "ui/widgets/editors/w_OutboundEditor.hpp"
#include "ui/widgets/editors/w_RoutesEditor.hpp"
#include "ui/widgets/windows/w_GroupManager.hpp"
#include "w_ScreenShot_Core.hpp"

#include <QFileDialog>
#define QV_MODULE_NAME "ImportWindow"

namespace
{
    constexpr auto LINK_PAGE = 0;
#if QV2RAY_FEATURE(ui_has_import_qrcode)
    constexpr auto QRCODE_PAGE = 1;
    constexpr auto ADVANCED_PAGE = 2;
#else
    constexpr auto ADVANCED_PAGE = 1;
#endif
} // namespace

ImportConfigWindow::ImportConfigWindow(QWidget *parent) : QvDialog("ImportWindow", parent)
{
    addStateOptions("width", { [&] { return width(); }, [&](QJsonValue val) { resize(val.toInt(), size().height()); } });
    addStateOptions("height", { [&] { return height(); }, [&](QJsonValue val) { resize(size().width(), val.toInt()); } });
    addStateOptions("x", { [&] { return x(); }, [&](QJsonValue val) { move(val.toInt(), y()); } });
    addStateOptions("y", { [&] { return y(); }, [&](QJsonValue val) { move(x(), val.toInt()); } });

    setupUi(this);
    QvMessageBusConnect(ImportConfigWindow);
    RESTORE_RUNTIME_CONFIG(screenShotHideQv2ray, hideQv2rayCB->setChecked)
    //
    auto defaultItemIndex = 0;
    for (const auto &gid : ConnectionManager->AllGroups())
    {
        groupCombo->addItem(GetDisplayName(gid), gid.toString());
        if (gid == DefaultGroupId)
            defaultItemIndex = groupCombo->count() - 1;
    }
    groupCombo->setCurrentIndex(defaultItemIndex);
#if !QV2RAY_FEATURE(ui_has_import_qrcode)
    qrCodeTab->setVisible(false);
    tabWidget->removeTab(1);
#endif
}

void ImportConfigWindow::updateColorScheme()
{
    // Stub
}

QvMessageBusSlotImpl(ImportConfigWindow)
{
    switch (msg)
    {
        MBShowDefaultImpl;
        MBHideDefaultImpl;
        MBRetranslateDefaultImpl;
        MBUpdateColorSchemeDefaultImpl;
    }
}

ImportConfigWindow::~ImportConfigWindow()
{
}

QMultiMap<QString, CONFIGROOT> ImportConfigWindow::SelectConnection(bool outboundsOnly)
{
    // partial import means only import as an outbound, will set outboundsOnly to
    // false and disable the checkbox
    keepImportedInboundCheckBox->setEnabled(!outboundsOnly);
    groupCombo->setEnabled(false);
    this->exec();
    QMultiMap<QString, CONFIGROOT> conn;
    for (const auto &connEntry : connectionsToNewGroup.values())
    {
        conn += connEntry;
    }
    for (const auto &connEntry : connectionsToExistingGroup.values())
    {
        conn += connEntry;
    }
    return result() == Accepted ? conn : QMultiMap<QString, CONFIGROOT>{};
}

int ImportConfigWindow::PerformImportConnection()
{
    this->exec();
    int count = 0;
    for (const auto &groupObject : connectionsToNewGroup)
    {
        const auto groupName = connectionsToNewGroup.key(groupObject);
        GroupId groupId = ConnectionManager->CreateGroup(groupName, false);
        for (const auto &connConf : groupObject)
        {
            auto connName = groupObject.key(connConf);

            auto [protocol, host, port] = GetConnectionInfo(connConf);
            if (connName.isEmpty())
            {
                connName = protocol + "/" + host + ":" + QSTRN(port) + "-" + GenerateRandomString(5);
            }
            ConnectionManager->CreateConnection(connConf, connName, groupId, true);
        }
    }

    for (const auto &groupObject : connectionsToExistingGroup)
    {
        const auto groupId = connectionsToExistingGroup.key(groupObject);
        for (const auto &connConf : groupObject)
        {
            auto connName = groupObject.key(connConf);
            auto [protocol, host, port] = GetConnectionInfo(connConf);
            if (connName.isEmpty())
            {
                connName = protocol + "/" + host + ":" + QSTRN(port) + "-" + GenerateRandomString(5);
            }
            ConnectionManager->CreateConnection(connConf, connName, groupId, true);
        }
    }

    return count;
}

#if QV2RAY_FEATURE(ui_has_import_qrcode)
void ImportConfigWindow::on_qrFromScreenBtn_clicked()
{
    bool hideQv2ray = hideQv2rayCB->isChecked();

    if (hideQv2ray)
    {
        UIMessageBus.EmitGlobalSignal(QvMBMessage::HIDE_WINDOWS);
    }

    QApplication::processEvents();
    QThread::msleep(doubleSpinBox->value() * 1000UL);
    ScreenShotWindow w;
    auto pix = w.DoScreenShot();
    if (hideQv2ray)
    {
        UIMessageBus.EmitGlobalSignal(QvMBMessage::SHOW_WINDOWS);
    }

    if (w.result() == QDialog::Accepted)
    {
        auto str = DecodeQRCode(pix);
        if (str.trimmed().isEmpty())
        {
            LOG("Cannot decode QR Code from an image, size:", pix.width(), pix.height());
            QvMessageBoxWarn(this, tr("Capture QRCode"), tr("Cannot find a valid QRCode from this region."));
        }
        else
        {
            qrCodeLinkTxt->setText(str.trimmed());
        }
    }
}

void ImportConfigWindow::on_selectImageBtn_clicked()
{
    const auto dir = QFileDialog::getOpenFileName(this, tr("Select an image to import"));
    imageFileEdit->setText(dir);
    //
    QFile file(dir);
    if (!file.exists())
        return;
    file.open(QFile::OpenModeFlag::ReadOnly);
    auto buf = file.readAll();
    file.close();
    //
    const auto str = DecodeQRCode(QImage::fromData(buf));

    if (str.isEmpty())
    {
        QvMessageBoxWarn(this, tr("QRCode scanning failed"), tr("Cannot find any QRCode from the image."));
        return;
    }
    qrCodeLinkTxt->setText(str.trimmed());
}

void ImportConfigWindow::on_hideQv2rayCB_stateChanged(int arg1)
{
    Q_UNUSED(arg1)
    SET_RUNTIME_CONFIG(screenShotHideQv2ray, hideQv2rayCB->isChecked)
}
#endif

void ImportConfigWindow::on_beginImportBtn_clicked()
{
    QString aliasPrefix = nameTxt->text();

    switch (tabWidget->currentIndex())
    {
        case LINK_PAGE:
        {
            QStringList linkList = SplitLines(vmessConnectionStringTxt->toPlainText());
            //
            // Clear UI and error lists
            linkErrors.clear();
            vmessConnectionStringTxt->clear();
            errorsList->clear();
            //
            LOG(linkList.count(), "entries found.");

            while (!linkList.isEmpty())
            {
                aliasPrefix = nameTxt->text();
                const auto link = linkList.takeFirst().trimmed();
                if (link.isEmpty() || link.startsWith("#") || link.startsWith("//"))
                    continue;

                // warn if someone tries to import a https:// link
                if (link.startsWith("https://"))
                {
                    errorsList->addItem(tr("WARNING: You may have mistaken 'subscription link' with 'share link'"));
                }

                QString errMessage;
                QString newGroupName;
                const auto config = ConvertConfigFromString(link, &aliasPrefix, &errMessage, &newGroupName);

                // If the config is empty or we have any err messages.
                if (config.isEmpty() || !errMessage.isEmpty())
                {
                    // To prevent duplicated values.
                    linkErrors[link] = QSTRN(linkErrors.count() + 1) + ": " + errMessage;
                    continue;
                }
                else if (newGroupName.isEmpty())
                {
                    for (const auto &conf : config)
                    {
                        connectionsToExistingGroup[GroupId{ groupCombo->currentData().toString() }].insert(conf.first, conf.second);
                    }
                }
                else
                {
                    for (const auto &conf : config)
                    {
                        connectionsToNewGroup[newGroupName].insert(conf.first, conf.second);
                    }
                }
            }

            if (!linkErrors.isEmpty())
            {
                for (const auto &item : qAsConst(linkErrors))
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
#if QV2RAY_FEATURE(ui_has_import_qrcode)
        case QRCODE_PAGE:
        {
            QString errorMsg;
            const auto root = ConvertConfigFromString(qrCodeLinkTxt->text(), &aliasPrefix, &errorMsg);
            if (!errorMsg.isEmpty())
            {
                QvMessageBoxWarn(this, tr("Failed to import connection"), errorMsg);
                break;
            }
            for (const auto &conf : root)
            {
                connectionsToExistingGroup[GroupId{ groupCombo->currentData().toString() }].insert(conf.first, conf.second);
            }
            break;
        }
#endif
        case ADVANCED_PAGE:
        {
            // From File...
            bool ImportAsComplex = keepImportedInboundCheckBox->isChecked();
            const auto path = fileLineTxt->text();

            if (const auto &result = V2RayKernelInstance::ValidateConfig(path); result)
            {
                QvMessageBoxWarn(this, tr("Import config file"), *result);
                return;
            }

            aliasPrefix += "_" + QFileInfo(path).fileName();
            CONFIGROOT config = ConvertConfigFromFile(path, ImportAsComplex);
            connectionsToExistingGroup[GroupId{ groupCombo->currentData().toString() }].insert(aliasPrefix, config);
            break;
        }
    }

    accept();
}

void ImportConfigWindow::on_errorsList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    Q_UNUSED(previous)

    if (!current)
        return;

    const auto currentErrorText = current->text();
    const auto vmessEntry = linkErrors.key(currentErrorText);
    //
    const auto startPos = vmessConnectionStringTxt->toPlainText().indexOf(vmessEntry);
    const auto endPos = startPos + vmessEntry.length();

    if (startPos < 0)
        return;

    // Select vmess string that is invalid.
    auto c = vmessConnectionStringTxt->textCursor();
    c.setPosition(startPos);
    c.setPosition(endPos, QTextCursor::KeepAnchor);
    vmessConnectionStringTxt->setTextCursor(c);
}

void ImportConfigWindow::on_cancelImportBtn_clicked()
{
    reject();
}

void ImportConfigWindow::on_routeEditBtn_clicked()
{
    RouteEditor w(QJsonObject(), this);
    const auto result = w.OpenEditor();
    const auto alias = nameTxt->text();
    bool isChanged = w.result() == QDialog::Accepted;

    if (isChanged)
    {
        connectionsToExistingGroup[GroupId{ groupCombo->currentData().toString() }].insert(alias, result);
        accept();
    }
}

void ImportConfigWindow::on_jsonEditBtn_clicked()
{
    JsonEditor w(QJsonObject(), this);
    const auto result = w.OpenEditor();
    const auto alias = nameTxt->text();
    const auto isChanged = w.result() == QDialog::Accepted;

    if (isChanged)
    {
        connectionsToExistingGroup[GroupId{ groupCombo->currentData().toString() }].insert(alias, CONFIGROOT(result));
        accept();
    }
}

void ImportConfigWindow::on_selectFileBtn_clicked()
{
    const auto dir = QFileDialog::getOpenFileName(this, tr("Select file to import"));
    fileLineTxt->setText(dir);
}
