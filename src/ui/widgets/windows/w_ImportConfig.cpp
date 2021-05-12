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
    constexpr auto ADVANCED_PAGE = 1;
} // namespace

ImportConfigWindow::ImportConfigWindow(QWidget *parent) : QvDialog("ImportWindow", parent)
{
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
    qrCodeTab->setVisible(false);
    tabWidget->removeTab(1);
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
