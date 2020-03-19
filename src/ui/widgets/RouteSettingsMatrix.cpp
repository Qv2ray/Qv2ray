#include "RouteSettingsMatrix.hpp"

#include "common/QvHelpers.hpp"
#include "components/geosite/QvGeositeReader.hpp"

RouteSettingsMatrixWidget::RouteSettingsMatrixWidget(QWidget *parent) : QWidget(parent)
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
}

void RouteSettingsMatrixWidget::SetRouteConfig(const config::Qv2rayRouteConfig conf, const QString &assetsDirPath)
{
    directDomainTxt->setText(conf.directDomains.join(NEWLINE));
    proxyDomainTxt->setText(conf.proxyDomains.join(NEWLINE));
    blockDomainTxt->setText(conf.blockDomains.join(NEWLINE));
    //
    blockIPTxt->setText(conf.blockIPs.join(NEWLINE));
    directIPTxt->setText(conf.directIPs.join(NEWLINE));
    proxyIPTxt->setText(conf.proxyIPs.join(NEWLINE));
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

config::Qv2rayRouteConfig RouteSettingsMatrixWidget::GetRouteConfig() const
{
    config::Qv2rayRouteConfig conf;
    conf.blockDomains = SplitLines(blockDomainTxt->toPlainText().replace(" ", ""));
    conf.directDomains = SplitLines(directDomainTxt->toPlainText().replace(" ", ""));
    conf.proxyDomains = SplitLines(proxyDomainTxt->toPlainText().replace(" ", ""));
    //
    conf.blockIPs = SplitLines(blockIPTxt->toPlainText().replace(" ", ""));
    conf.directIPs = SplitLines(directIPTxt->toPlainText().replace(" ", ""));
    conf.proxyIPs = SplitLines(proxyIPTxt->toPlainText().replace(" ", ""));
    return conf;
}

RouteSettingsMatrixWidget::~RouteSettingsMatrixWidget()
{
}
