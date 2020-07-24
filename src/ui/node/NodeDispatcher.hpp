#pragma once
#include "base/Qv2rayBase.hpp"
#include "base/models/QvComplexConfigModels.hpp"

#include <nodes/FlowScene>

enum TagNodeMode
{
    NODE_INBOUND,
    NODE_OUTBOUND
};

class NodeDispatcher
    : public QObject
    , public std::enable_shared_from_this<NodeDispatcher>
{
    Q_OBJECT
  public:
    explicit NodeDispatcher(QtNodes::FlowScene *, QObject *parent = nullptr);
    ~NodeDispatcher();

  public:
    CONFIGROOT GetFullConfig() const;
    void LoadFullConfig(const CONFIGROOT &);
    [[nodiscard]] QString CreateInbound(INBOUND);
    [[nodiscard]] QString CreateOutbound(OutboundObjectMeta);
    [[nodiscard]] QString CreateRule(RuleObject);
    bool IsNodeConstructing() const
    {
        return isConstructing;
    }

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

  public:
    void DeleteNode(const QtNodes::Node &node);

    template<TagNodeMode t>
    inline bool RenameTag(const QString &originalTag, const QString &newTag)
    {
        if constexpr (t == NODE_INBOUND)
        {
            bool hasExisting = inbounds.contains(newTag);
            if (hasExisting)
                return false;
            inbounds[newTag] = inbounds.take(originalTag);
            inboundNodes[newTag] = inboundNodes.take(originalTag);
        }
        else if constexpr (t == NODE_OUTBOUND)
        {
            bool hasExisting = outbounds.contains(newTag);
            if (hasExisting)
                return false;
            outbounds[newTag] = outbounds.take(originalTag);
            outboundNodes[newTag] = outboundNodes.take(originalTag);
        }
        return true;
    }

  signals:
    void OnOutboundDeleted(const OutboundObjectMeta &object);
    //
    void OnInboundCreated(std::shared_ptr<INBOUND>, QtNodes::Node &);
    void OnOutboundCreated(std::shared_ptr<OutboundObjectMeta>, QtNodes::Node &);
    void OnRuleCreated(std::shared_ptr<RuleObject>, QtNodes::Node &);

  signals:
    void OnInboundOutboundNodeHovered(const QString &tag, const ProtocolSettingsInfoObject &);

  private:
    void RestoreConnections();

  private:
    QString defaultOutbound;
    QMap<QString, QUuid> inboundNodes;
    QMap<QString, QUuid> outboundNodes;
    QMap<QString, QUuid> ruleNodes;
    //
    QtNodes::FlowScene *scene;
    bool isConstructing;
    QMap<QString, std::shared_ptr<INBOUND>> inbounds;
    QMap<QString, std::shared_ptr<RuleObject>> rules;
    QMap<QString, std::shared_ptr<OutboundObjectMeta>> outbounds;
};
