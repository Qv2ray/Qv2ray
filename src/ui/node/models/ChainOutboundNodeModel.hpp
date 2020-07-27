#pragma once
#include "ui/node/NodeBase.hpp"

class ChainOutboundNodeModel : public NodeDataModel
{
    Q_OBJECT
  public:
    explicit ChainOutboundNodeModel(std::shared_ptr<ChainObject>);
    ~ChainOutboundNodeModel(){};

    inline QString caption() const override
    {
        return {};
    }
    inline bool captionVisible() const override
    {
        return false;
    }
    inline QString name() const override
    {
        return "ChainOutboundNodeModel";
    }
    ConnectionPolicy portOutConnectionPolicy(PortIndex) const override
    {
        return ConnectionPolicy::One;
    }
    ConnectionPolicy portInConnectionPolicy(PortIndex) const override
    {
        return ConnectionPolicy::One;
    }
    unsigned int nPorts(PortType) const override
    {
        return 1;
    }
    std::shared_ptr<NodeDataType> dataType(PortType, PortIndex) const override
    {
        return NODE_TYPE_CHAINED_OUTBOUND;
    }

  public:
    void onNodeHoverEnter() override{};
    void onNodeHoverLeave() override{};
    virtual void setInData(std::shared_ptr<NodeData> nodeData, PortIndex port) override;
    virtual void setInData(std::vector<std::shared_ptr<NodeData>> nodeData, PortIndex port) override;
    virtual std::shared_ptr<NodeData> outData(PortIndex) override
    {
        return std::make_shared<ChainOutboundData>(dataptr);
    }

    inline QWidget *embeddedWidget() override
    {
        return widget;
    }
    inline std::unique_ptr<NodeDataModel> clone() const override
    {
        return {};
    }

    void inputConnectionCreated(const QtNodes::Connection &) override;
    void inputConnectionDeleted(const QtNodes::Connection &) override;
    void outputConnectionCreated(const QtNodes::Connection &) override;
    void outputConnectionDeleted(const QtNodes::Connection &) override;
    const std::shared_ptr<const ChainObject> getData() const
    {
        return dataptr;
    }

  private:
    std::shared_ptr<ChainObject> dataptr;
    QvNodeWidget *widget;
};
