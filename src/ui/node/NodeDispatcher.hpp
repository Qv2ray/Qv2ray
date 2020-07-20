#pragma once
#include "base/Qv2rayBase.hpp"
#include "base/models/QvComplexConfigModels.hpp"

class NodeDispatcher : public QObject
{
    Q_OBJECT
  public:
    explicit NodeDispatcher(QObject *parent);
    ~NodeDispatcher();

  public:
    [[nodiscard]] QString CreateInbound(INBOUND);
    [[nodiscard]] QString CreateOutbound(OutboundObjectMeta);
    [[nodiscard]] QString CreateRule(RuleObject);

  public:
    const inline QStringList GetInboundTags() const
    {
        return inbounds.keys();
    }
    const inline QStringList GetOutboundTags() const
    {
        return outbounds.keys();
    }
    const inline QStringList GetRuleTags() const
    {
        return rules.keys();
    }
    inline int InboundsCount() const
    {
        return inbounds.count();
    }
    inline int RulesCount() const
    {
        return rules.count();
    }
    inline int OutboundsCount() const
    {
        return outbounds.count();
    }

  signals:
    void OnInboundCreated(std::shared_ptr<INBOUND>);
    void OnOutboundCreated(std::shared_ptr<OutboundObjectMeta>);
    void OnRuleCreated(std::shared_ptr<RuleObject>);

  signals:
    void OnInboundOutboundNodeHovered(const ProtocolSettingsInfoObject &);

  private:
    QMap<QString, INBOUND> inbounds;
    QMap<QString, RuleObject> rules;
    QMap<QString, OutboundObjectMeta> outbounds;
};
