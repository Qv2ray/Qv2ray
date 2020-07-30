#pragma once
#include "base/Qv2rayBase.hpp"
#include "base/models/QvComplexConfigModels.hpp"

#include <nodes/FlowScene>

class NodeDispatcher
    : public QObject
    , public std::enable_shared_from_this<NodeDispatcher>
{
    Q_OBJECT
  public:
    explicit NodeDispatcher(QObject *parent = nullptr);
    ~NodeDispatcher();
    void InitializeScenes(QtNodes::FlowScene *rule, QtNodes::FlowScene *chain)
    {
        ruleScene = rule;
        chainScene = chain;
        connect(ruleScene, &QtNodes::FlowScene::nodeDeleted, this, &NodeDispatcher::DeleteNode);
    }

  public:
    CONFIGROOT GetFullConfig() const;
    void LoadFullConfig(const CONFIGROOT &);
    [[nodiscard]] QString CreateInbound(INBOUND);
    [[nodiscard]] QString CreateOutbound(OutboundObjectMeta);
    [[nodiscard]] QString CreateRule(RuleObject);
    bool IsNodeConstructing() const
    {
        return isOperationLocked;
    }
    void LockOperation()
    {
        isOperationLocked = true;
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
    void RequestEditChain(const ChainId &id);
    const QList<const std::shared_ptr<OutboundObjectMeta>> GetChainableOutboudns() const
    {
        for (const auto &outbound : outbounds.values())
        {
            //
        }
        return {};
    }

    template<ComplexTagNodeMode t>
    inline bool RenameTag(const QString originalTag, const QString newTag)
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
        else if constexpr (t == NODE_RULE)
        {
            bool hasExisting = rules.contains(newTag);
            if (hasExisting)
                return false;
            rules[newTag] = rules.take(originalTag);
            ruleNodes[newTag] = ruleNodes.take(originalTag);
        }
        else
        {
            Q_UNREACHABLE();
        }
        emit OnObjectTagChanged(t, originalTag, newTag);
        return true;
    }

  signals:
    void OnOutboundDeleted(const OutboundObjectMeta &object);
    //
    void OnInboundCreated(std::shared_ptr<INBOUND>, QtNodes::Node &);
    void OnOutboundCreated(std::shared_ptr<OutboundObjectMeta>, QtNodes::Node &);
    void OnRuleCreated(std::shared_ptr<RuleObject>, QtNodes::Node &);
    void OnChainOutboundCreate(std::shared_ptr<OutboundObjectMeta>, QtNodes::Node &);
    //
    void OnObjectTagChanged(ComplexTagNodeMode, const QString originalTag, const QString newTag);

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
    QtNodes::FlowScene *ruleScene;
    QtNodes::FlowScene *chainScene;
    //
    bool isOperationLocked;
    QMap<QString, std::shared_ptr<INBOUND>> inbounds;
    QMap<QString, std::shared_ptr<RuleObject>> rules;
    QMap<QString, std::shared_ptr<OutboundObjectMeta>> outbounds;
};
