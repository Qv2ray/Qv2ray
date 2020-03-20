#include "RouteSettingsMatrix.hpp"

#include "common/QvHelpers.hpp"
#include "components/geosite/QvGeositeReader.hpp"

RouteSettingsMatrixWidget::RouteSettingsMatrixWidget(const QString &assetsDirPath, QWidget *parent)
    : QWidget(parent), assetsDirPath(assetsDirPath)
{
    setupUi(this);
    //
    directDomainTxt = new AutoCompleteTextEdit("geosite", this);
    proxyDomainTxt = new AutoCompleteTextEdit("geosite", this);
    blockDomainTxt = new AutoCompleteTextEdit("geosite", this);
    //
    directIPTxt = new AutoCompleteTextEdit("geoip", this);
    proxyIPTxt = new AutoCompleteTextEdit("geoip", this);
    blockIPTxt = new AutoCompleteTextEdit("geoip", this);
    //
    auto sourceStringsDomain = ReadGeoSiteFromFile(assetsDirPath + "/geosite.dat");
    directDomainTxt->SetSourceStrings(sourceStringsDomain);
    proxyDomainTxt->SetSourceStrings(sourceStringsDomain);
    blockDomainTxt->SetSourceStrings(sourceStringsDomain);
    //
    auto sourceStringsIP = ReadGeoSiteFromFile(assetsDirPath + "/geoip.dat");
    directIPTxt->SetSourceStrings(sourceStringsIP);
    proxyIPTxt->SetSourceStrings(sourceStringsIP);
    blockIPTxt->SetSourceStrings(sourceStringsIP);
    //
    directTxtLayout->addWidget(directDomainTxt, 0, 0);
    proxyTxtLayout->addWidget(proxyDomainTxt, 0, 0);
    blockTxtLayout->addWidget(blockDomainTxt, 0, 0);
    //
    directIPLayout->addWidget(directIPTxt, 0, 0);
    proxyIPLayout->addWidget(proxyIPTxt, 0, 0);
    blockIPLayout->addWidget(blockIPTxt, 0, 0);
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
