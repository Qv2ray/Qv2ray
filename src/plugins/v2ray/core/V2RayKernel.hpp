#pragma once

#include "QvPlugin/Handlers/KernelHandler.hpp"

class QProcess;
class APIWorker;

const inline KernelId v2ray_kernel_id{ u"v2ray_kernel"_qs };

class V2RayKernel : public Qv2rayPlugin::Kernel::PluginKernel
{
    Q_OBJECT
  public:
    V2RayKernel();
    ~V2RayKernel();

  public:
    virtual void SetConnectionSettings(const QMap<Qv2rayPlugin::Kernel::KernelOptionFlags, QVariant> &, const IOConnectionSettings &) override{};
    virtual void SetProfileContent(const ProfileContent &) override;
    virtual bool PrepareConfigurations() override;
    virtual void Start() override;
    virtual bool Stop() override;
    virtual KernelId GetKernelId() const override
    {
        return v2ray_kernel_id;
    }

  signals:
    void OnCrashed(const QString &);
    void OnLog(const QString &);
    void OnStatsAvailable(StatisticsObject);

  private:
    std::optional<QString> ValidateConfig(const QString &path);

  private:
    ProfileContent profile;
    APIWorker *apiWorker;
    QProcess *vProcess;
    bool apiEnabled;
    bool kernelStarted = false;
    QMap<QString, QString> tagProtocolMap;
    QString configFilePath;
};

class V2RayKernelInterface : public Qv2rayPlugin::Kernel::IKernelHandler
{
  public:
    V2RayKernelInterface() : Qv2rayPlugin::Kernel::IKernelHandler(){};
    virtual QList<Qv2rayPlugin::Kernel::KernelFactory> PluginKernels() const override
    {
        Qv2rayPlugin::Kernel::KernelFactory v2ray;
        v2ray.Capabilities.setFlag(Qv2rayPlugin::Kernel::KERNELCAP_ROUTER);
        v2ray.Id = v2ray_kernel_id;
        v2ray.Name = u"V2Ray"_qs;
        v2ray.Create = std::function{ []() { return std::make_unique<V2RayKernel>(); } };
        v2ray.SupportedProtocols << u"blackhole"_qs   //
                                 << u"dns"_qs         //
                                 << u"freedom"_qs     //
                                 << u"http"_qs        //
                                 << u"loopback"_qs    //
                                 << u"shadowsocks"_qs //
                                 << u"socks"_qs       //
                                 << u"trojan"_qs      //
                                 << u"vless"_qs       //
                                 << u"vmess"_qs;
        return { v2ray };
    }
};
