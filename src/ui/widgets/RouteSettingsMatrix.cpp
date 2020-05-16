#include "RouteSettingsMatrix.hpp"

#include "common/QvHelpers.hpp"
#include "components/geosite/QvGeositeReader.hpp"
#include "components/route/RouteSchemeIO.hpp"
#include "components/route/presets/RouteScheme_V2rayN.hpp"
#include "ui/common/UIBase.hpp"

#include <QFileDialog>
#include <QInputDialog>

RouteSettingsMatrixWidget::RouteSettingsMatrixWidget(const QString &assetsDirPath, QWidget *parent)
    : QWidget(parent), assetsDirPath(assetsDirPath)
{
    setupUi(this);
    //
    builtInSchemesMenu = new QMenu(this);
    builtInSchemesMenu->addActions(this->getBuiltInSchemes());
    builtInSchemeBtn->setMenu(builtInSchemesMenu);
    //
    auto sourceStringsDomain = ReadGeoSiteFromFile(assetsDirPath + "/geosite.dat");
    directDomainTxt = new AutoCompleteTextEdit("geosite", sourceStringsDomain, this);
    proxyDomainTxt = new AutoCompleteTextEdit("geosite", sourceStringsDomain, this);
    blockDomainTxt = new AutoCompleteTextEdit("geosite", sourceStringsDomain, this);
    //
    auto sourceStringsIP = ReadGeoSiteFromFile(assetsDirPath + "/geoip.dat");
    directIPTxt = new AutoCompleteTextEdit("geoip", sourceStringsIP, this);
    proxyIPTxt = new AutoCompleteTextEdit("geoip", sourceStringsIP, this);
    blockIPTxt = new AutoCompleteTextEdit("geoip", sourceStringsIP, this);
    //
    directTxtLayout->addWidget(directDomainTxt, 0, 0);
    proxyTxtLayout->addWidget(proxyDomainTxt, 0, 0);
    blockTxtLayout->addWidget(blockDomainTxt, 0, 0);
    //
    directIPLayout->addWidget(directIPTxt, 0, 0);
    proxyIPLayout->addWidget(proxyIPTxt, 0, 0);
    blockIPLayout->addWidget(blockIPTxt, 0, 0);
}

/**
 * @brief RouteSettingsMatrixWidget::getBuiltInSchemes
 * @return
 */
QList<QAction *> RouteSettingsMatrixWidget::getBuiltInSchemes()
{
    QList<QAction *> list;
    list.append(this->schemeToAction(tr("empty scheme"), emptyScheme));
    list.append(this->schemeToAction(tr("v2rayN preset"), presets::v2rayN::v2rayNScheme));
    return list;
}

QAction *RouteSettingsMatrixWidget::schemeToAction(const QString &name, const Qv2ray::base::config::Qv2rayConfig_Routing &scheme)
{
    QAction *action = new QAction(this);
    action->setText(name);
    connect(action, &QAction::triggered, [this, &scheme] { this->SetRouteConfig(scheme); });
    return action;
}

void RouteSettingsMatrixWidget::SetRouteConfig(const Qv2rayConfig_Routing &conf)
{
    domainStrategyCombo->setCurrentText(conf.domainStrategy);
    //
    directDomainTxt->setText(conf.domains.direct.join(NEWLINE));
    proxyDomainTxt->setText(conf.domains.proxy.join(NEWLINE));
    blockDomainTxt->setText(conf.domains.block.join(NEWLINE));
    //
    blockIPTxt->setText(conf.ips.block.join(NEWLINE));
    directIPTxt->setText(conf.ips.direct.join(NEWLINE));
    proxyIPTxt->setText(conf.ips.proxy.join(NEWLINE));
}

Qv2rayConfig_Routing RouteSettingsMatrixWidget::GetRouteConfig() const
{
    config::Qv2rayConfig_Routing conf;
    conf.domainStrategy = this->domainStrategyCombo->currentText();
    conf.domains.block = SplitLines(blockDomainTxt->toPlainText().replace(" ", ""));
    conf.domains.direct = SplitLines(directDomainTxt->toPlainText().replace(" ", ""));
    conf.domains.proxy = SplitLines(proxyDomainTxt->toPlainText().replace(" ", ""));
    //
    conf.ips.block = SplitLines(blockIPTxt->toPlainText().replace(" ", ""));
    conf.ips.direct = SplitLines(directIPTxt->toPlainText().replace(" ", ""));
    conf.ips.proxy = SplitLines(proxyIPTxt->toPlainText().replace(" ", ""));
    return conf;
}

RouteSettingsMatrixWidget::~RouteSettingsMatrixWidget()
{
}

/**
 * @brief RouteSettingsMatrixWidget::on_importSchemeBtn_clicked
 * @author DuckSoft <realducksoft@gmail.com>
 * @todo add some debug output
 */
void RouteSettingsMatrixWidget::on_importSchemeBtn_clicked()
{
    try
    {
        // open up the file dialog and choose a file.
        auto filePath = this->openFileDialog();
        if (!filePath)
            return;

        // read the file and parse back to struct.
        // if error occurred on parsing, an exception will be thrown.
        auto content = StringFromFile(ACCESS_OPTIONAL_VALUE(filePath));
        auto scheme = Qv2rayRouteScheme::fromJson(JsonFromString(content));

        // show the information of this scheme to user,
        // and ask user if he/she wants to import and apply this.
        auto strPrompt = tr("Import scheme '%1' made by '%2'? \r\n Description: %3").arg(scheme.name, scheme.author, scheme.description);
        auto decision = QvMessageBoxAsk(this, tr("Importing Scheme"), strPrompt);

        // if user don't want to import, just leave.
        if (decision != QMessageBox::Yes)
            return;

        // write the scheme onto the window
        this->SetRouteConfig(static_cast<Qv2rayConfig_Routing>(scheme));

        // done
        LOG(MODULE_SETTINGS, "Imported route config: " + scheme.name + " by: " + scheme.author)
    }
    catch (std::exception &e)
    {
        LOG(MODULE_UI, "Exception: " + QString(e.what()))
        // TODO: Give some error as Notification
    }
}

/**
 * @brief RouteSettingsMatrixWidget::on_exportSchemeBtn_clicked
 * @author DuckSoft <realducksoft@gmail.com>
 */
void RouteSettingsMatrixWidget::on_exportSchemeBtn_clicked()
{
    try
    {
        // parse the config back from the window components
        auto config = this->GetRouteConfig();

        // init some constants
        const auto dialogTitle = tr("Exporting Scheme");

        // scheme name?
        bool ok = false;
        auto schemeName = QInputDialog::getText(this, dialogTitle, tr("Scheme name:"), QLineEdit::Normal, tr("Unnamed Scheme"), &ok);
        if (!ok)
            return;

        // scheme author?
        auto schemeAuthor = QInputDialog::getText(this, dialogTitle, tr("Author:"), QLineEdit::Normal, "Anonymous <mystery@example.com>", &ok);
        if (!ok)
            return;

        // scheme description?
        auto schemeDescription =
            QInputDialog::getText(this, dialogTitle, tr("Description:"), QLineEdit::Normal, tr("The author is too lazy to leave a comment"));
        if (!ok)
            return;

        // where to save?
        auto savePath = this->saveFileDialog();
        if (!savePath)
            return;

        // construct the data structure
        Qv2rayRouteScheme scheme;
        scheme.name = schemeName;
        scheme.author = schemeAuthor;
        scheme.description = schemeDescription;
        scheme.domainStrategy = config.domainStrategy;
        scheme.ips = config.ips;
        scheme.domains = config.domains;

        // serialize and write out
        auto content = JsonToString(scheme.toJson());
        StringToFile(content, ACCESS_OPTIONAL_VALUE(savePath));

        // done
        // TODO: Give some success as Notification
        QvMessageBoxInfo(this, dialogTitle, tr("Your route scheme has been successfully exported!"));
    }
    catch (...)
    {
        // TODO: Give some error as Notification
    }
}

/**
 * @brief opens a save dialog and asks user to specify the save path.
 * @return the selected file path, if any
 * @author DuckSoft <realducksoft@gmail.com>
 */
std::optional<QString> RouteSettingsMatrixWidget::saveFileDialog()
{
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setOption(QFileDialog::Option::DontConfirmOverwrite, !true);
    dialog.setNameFilter(tr("QvRoute Schemes(*.json)"));
    dialog.setAcceptMode(QFileDialog::AcceptMode::AcceptSave);
    if (!dialog.exec() || dialog.selectedFiles().length() != 1)
    {
        return std::nullopt;
    }
    return dialog.selectedFiles().first();
}

/**
 * @brief opens up a dialog and asks user to choose a scheme file.
 * @return the selected file path, if any
 * @author DuckSoft <realducksoft@gmail.com>
 */
std::optional<QString> RouteSettingsMatrixWidget::openFileDialog()
{
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("QvRoute Schemes(*.json)"));
    if (!dialog.exec() || dialog.selectedFiles().length() != 1)
    {
        return std::nullopt;
    }
    return dialog.selectedFiles().first();
}
