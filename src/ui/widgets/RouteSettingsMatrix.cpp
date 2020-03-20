#include "RouteSettingsMatrix.hpp"

#include "common/QvHelpers.hpp"
#include "components/geosite/QvGeositeReader.hpp"

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
    //
}

/**
 * @brief RouteSettingsMatrixWidget::getBuiltInSchemes
 * @return
 */
QList<QAction *> RouteSettingsMatrixWidget::getBuiltInSchemes()
{
    QList<QAction *> list;

    Qv2ray::base::config::Qv2rayRouteConfig emptyScheme;
    list.append(this->schemeToAction(tr("empty scheme"), emptyScheme));

    // v2rayN scheme from 2dust/v2rayCustomRoutingList
    // License: GPL3
    Qv2ray::base::config::Qv2rayRouteConfig v2rayNScheme;
    v2rayNScheme.ips.proxy = {
        "91.108.4.0/22",  "91.108.8.0/22",  "91.108.12.0/22",   "91.108.20.0/22",   "91.108.36.0/23",
        "91.108.38.0/23", "91.108.56.0/22", "149.154.160.0/20", "149.154.164.0/22", "149.154.172.0/22",
        "74.125.0.0/16",  "173.194.0.0/16", "172.217.0.0/16",   "216.58.200.0/24",  "216.58.220.0/24",
        "91.108.56.116",  "91.108.56.0/24", "109.239.140.0/24", "149.154.167.0/24", "149.154.175.0/24",
    };
    v2rayNScheme.domains.direct = {
        "domain:12306.com",
        "domain:51ym.me",
        "domain:52pojie.cn",
        "domain:8686c.com",
        "domain:abercrombie.com",
        "domain:adobesc.com",
        "domain:air-matters.com",
        "domain:air-matters.io",
        "domain:airtable.com",
        "domain:akadns.net",
        "domain:apache.org",
        "domain:api.crisp.chat",
        "domain:api.termius.com",
        "domain:appshike.com",
        "domain:appstore.com",
        "domain:aweme.snssdk.com",
        "domain:bababian.com",
        "domain:battle.net",
        "domain:beatsbydre.com",
        "domain:bet365.com",
        "domain:bilibili.cn",
        "domain:ccgslb.com",
        "domain:ccgslb.net",
        "domain:chunbo.com",
        "domain:chunboimg.com",
        "domain:clashroyaleapp.com",
        "domain:cloudsigma.com",
        "domain:cloudxns.net",
        "domain:cmfu.com",
        "domain:culturedcode.com",
        "domain:dct-cloud.com",
        "domain:didialift.com",
        "domain:douyutv.com",
        "domain:duokan.com",
        "domain:dytt8.net",
        "domain:easou.com",
        "domain:ecitic.net",
        "domain:eclipse.org",
        "domain:eudic.net",
        "domain:ewqcxz.com",
        "domain:fir.im",
        "domain:frdic.com",
        "domain:fresh-ideas.cc",
        "domain:godic.net",
        "domain:goodread.com",
        "domain:haibian.com",
        "domain:hdslb.net",
        "domain:hollisterco.com",
        "domain:hongxiu.com",
        "domain:hxcdn.net",
        "domain:images.unsplash.com",
        "domain:img4me.com",
        "domain:ipify.org",
        "domain:ixdzs.com",
        "domain:jd.hk",
        "domain:jianshuapi.com",
        "domain:jomodns.com",
        "domain:jsboxbbs.com",
        "domain:knewone.com",
        "domain:kuaidi100.com",
        "domain:lemicp.com",
        "domain:letvcloud.com",
        "domain:lizhi.io",
        "domain:localizecdn.com",
        "domain:lucifr.com",
        "domain:luoo.net",
        "domain:mai.tn",
        "domain:maven.org",
        "domain:miwifi.com",
        "domain:moji.com",
        "domain:moke.com",
        "domain:mtalk.google.com",
        "domain:mxhichina.com",
        "domain:myqcloud.com",
        "domain:myunlu.com",
        "domain:netease.com",
        "domain:nfoservers.com",
        "domain:nssurge.com",
        "domain:nuomi.com",
        "domain:ourdvs.com",
        "domain:overcast.fm",
        "domain:paypal.com",
        "domain:paypalobjects.com",
        "domain:pgyer.com",
        "domain:qdaily.com",
        "domain:qdmm.com",
        "domain:qin.io",
        "domain:qingmang.me",
        "domain:qingmang.mobi",
        "domain:qqurl.com",
        "domain:rarbg.to",
        "domain:rrmj.tv",
        "domain:ruguoapp.com",
        "domain:sm.ms",
        "domain:snwx.com",
        "domain:soku.com",
        "domain:startssl.com",
        "domain:store.steampowered.com",
        "domain:symcd.com",
        "domain:teamviewer.com",
        "domain:tmzvps.com",
        "domain:trello.com",
        "domain:trellocdn.com",
        "domain:ttmeiju.com",
        "domain:udache.com",
        "domain:uxengine.net",
        "domain:weather.bjango.com",
        "domain:weather.com",
        "domain:webqxs.com",
        "domain:weico.cc",
        "domain:wenku8.net",
        "domain:werewolf.53site.com",
        "domain:windowsupdate.com",
        "domain:wkcdn.com",
        "domain:workflowy.com",
        "domain:xdrig.com",
        "domain:xiaojukeji.com",
        "domain:xiaomi.net",
        "domain:xiaomicp.com",
        "domain:ximalaya.com",
        "domain:xitek.com",
        "domain:xmcdn.com",
        "domain:xslb.net",
        "domain:xteko.com",
        "domain:yach.me",
        "domain:yixia.com",
        "domain:yunjiasu-cdn.net",
        "domain:zealer.com",
        "domain:zgslb.net",
        "domain:zimuzu.tv",
        "domain:zmz002.com",
        "domain:samsungdm.com",
    };

    v2rayNScheme.domains.proxy = {
        "geosite:google",   "geosite:github",       "geosite:netflix",  "geosite:steam",   "geosite:telegram",
        "geosite:tumblr",   "geosite:speedtest",    "geosite:bbc",      "domain:gvt1.com", "domain:textnow.com",
        "domain:twitch.tv", "domain:wikileaks.org", "domain:naver.com",
    };

    v2rayNScheme.domains.block = {
        "geosite:category-ads-all",
    };

    list.append(this->schemeToAction(tr("v2rayN preset"), v2rayNScheme));
    return list;
}

QAction *RouteSettingsMatrixWidget::schemeToAction(const QString &name, Qv2ray::base::config::Qv2rayRouteConfig scheme)
{
    QAction *action = new QAction();
    action->setText(name);
    connect(action, &QAction::triggered, [=] { this->SetRouteConfig(scheme); });
    return action;
}

void RouteSettingsMatrixWidget::SetRouteConfig(const config::Qv2rayRouteConfig &conf)
{
    directDomainTxt->setText(conf.domains.direct.join(NEWLINE));
    proxyDomainTxt->setText(conf.domains.proxy.join(NEWLINE));
    blockDomainTxt->setText(conf.domains.block.join(NEWLINE));
    //
    blockIPTxt->setText(conf.ips.block.join(NEWLINE));
    directIPTxt->setText(conf.ips.direct.join(NEWLINE));
    proxyIPTxt->setText(conf.ips.proxy.join(NEWLINE));
}

config::Qv2rayRouteConfig RouteSettingsMatrixWidget::GetRouteConfig() const
{
    config::Qv2rayRouteConfig conf;
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
 * @brief The Qv2rayRouteScheme struct
 * @author DuckSoft <realducksoft@gmail.com>
 */
struct Qv2rayRouteScheme : config::Qv2rayRouteConfig
{
    /**
     * @brief the name of the scheme.
     * @example "Untitled Scheme"
     */
    QString name;
    /**
     * @brief the author of the scheme.
     * @example "DuckSoft <realducksoft@gmail.com>"
     */
    QString author;
    /**
     * @brief details of this scheme.
     * @example "A scheme to bypass China mainland, while allowing bilibili to go through proxy."
     */
    QString description;

    // M: all these fields are mandatory
    XTOSTRUCT(M(name, author, description, domains, ips));
};

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
        auto content = StringFromFile(filePath.value());
        auto scheme = StructFromJsonString<Qv2rayRouteScheme>(content);

        // show the information of this scheme to user,
        // and ask user if he/she wants to import and apply this.
        auto strPrompt = tr("Import scheme '%1' by '%2'?" NEWLINE "Description: %3").arg(scheme.name, scheme.author, scheme.description);
        auto decision = QvMessageBoxAsk(this, tr("Importing Scheme"), strPrompt);

        // if user don't want to import, just leave.
        if (decision != QMessageBox::Yes)
            return;

        // write the scheme onto the window
        this->SetRouteConfig(static_cast<Qv2rayRouteConfig>(scheme));

        // done
        LOG(MODULE_SETTINGS, "Imported route config: " + scheme.name + " by: " + scheme.author)
    }
    catch (exception e)
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
        scheme.ips = config.ips;
        scheme.domains = config.domains;

        // serialize and write out
        auto content = StructToJsonString(scheme);
        StringToFile(content, savePath.value());

        // done
        // TODO: Give some success as Notification
    }
    catch (exception)
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
