#pragma once

#include "ui/common/QvMessageBus.hpp"
#include "ui/widgets/node/NodeBase.hpp"
#include "ui_ChainEditorWidget.h"

#include <nodes/FlowScene>
#include <nodes/FlowView>
#include <tuple>

class NodeDispatcher;

class ChainEditorWidget
    : public QWidget
    , private Ui::ChainEditorWidget
{
    Q_OBJECT

  public:
    explicit ChainEditorWidget(std::shared_ptr<NodeDispatcher> dispatcher, QWidget *parent = nullptr);
    ~ChainEditorWidget()
    {
        connectionSignalBlocked = true;
    }
    auto getScene()
    {
        return scene;
    }

  protected:
    void changeEvent(QEvent *e);

  private slots:
    void on_chainComboBox_currentIndexChanged(int arg1);
    //
    void OnDispatcherChainedOutboundCreated(std::shared_ptr<OutboundObjectMeta>, QtNodes::Node &);
    void OnDispatcherChainedOutboundDeleted(const OutboundObjectMeta &);
    void OnDispatcherChainCreated(std::shared_ptr<OutboundObjectMeta>);
    void OnDispatcherChainDeleted(const OutboundObjectMeta &);
    //
    void OnDispatcherObjectTagChanged(ComplexTagNodeMode, const QString originalTag, const QString newTag);
    //
    void OnSceneConnectionCreated(const QtNodes::Connection &);
    void OnSceneConnectionRemoved(const QtNodes::Connection &);

  private:
    void BeginEditChain(const QString &chain);
    void updateColorScheme(){};
    void ShowChainLinkedList();
    std::tuple<bool, QString, QStringList> VerifyChainLinkedList(const QUuid &ignoredConnectionId);
    void TrySaveChainOutboudData(const QUuid &ignoredConnectionId = QUuid());
    QvMessageBusSlotDecl;

  private:
    bool connectionSignalBlocked = false;
    QtNodes::FlowScene *scene;
    QtNodes::FlowView *view;
    std::shared_ptr<NodeDispatcher> dispatcher;
    //
    QMap<QString, QUuid> outboundNodes;
    std::shared_ptr<OutboundObjectMeta> currentChain;
    QMap<QString, std::shared_ptr<OutboundObjectMeta>> chains;
};
