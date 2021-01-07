#include "ChainEditorWidget.hpp"

#include "ui/widgets/node/NodeBase.hpp"

constexpr auto GRAPH_GLOBAL_OFFSET_X = -20;
constexpr auto GRAPH_GLOBAL_OFFSET_Y = -300;

ChainEditorWidget::ChainEditorWidget(std::shared_ptr<NodeDispatcher> dispatcher, QWidget *parent) : QWidget(parent), dispatcher(dispatcher)
{
    setupUi(this);
    QvMessageBusConnect(ChainEditorWidget);
    scene = new QtNodes::FlowScene(this);
    view = new QtNodes::FlowView(scene, this);
    view->scaleDown();

    if (!viewWidget->layout())
    {
        // The QWidget will take ownership of layout.
        viewWidget->setLayout(new QVBoxLayout());
    }
    auto l = viewWidget->layout();
    l->addWidget(view);
    l->setContentsMargins(0, 0, 0, 0);
    l->setSpacing(0);
    //
    connect(dispatcher.get(), &NodeDispatcher::OnChainedOutboundCreated, this, &ChainEditorWidget::OnDispatcherChainedOutboundCreated);
    connect(dispatcher.get(), &NodeDispatcher::OnChainedOutboundDeleted, this, &ChainEditorWidget::OnDispatcherChainedOutboundDeleted);
    connect(dispatcher.get(), &NodeDispatcher::OnChainedCreated, this, &ChainEditorWidget::OnDispatcherChainCreated);
    connect(dispatcher.get(), &NodeDispatcher::OnChainedDeleted, this, &ChainEditorWidget::OnDispatcherChainDeleted);
    connect(dispatcher.get(), &NodeDispatcher::OnObjectTagChanged, this, &ChainEditorWidget::OnDispatcherObjectTagChanged);
    //
    connect(dispatcher.get(), &NodeDispatcher::OnFullConfigLoadCompleted, [this]() {
        if (!chains.isEmpty())
            currentChain = chains.first();
        ShowChainLinkedList();
    });
    //
    connect(dispatcher.get(), &NodeDispatcher::RequestEditChain, this, &ChainEditorWidget::BeginEditChain);
    //
    connect(scene, &QtNodes::FlowScene::connectionCreated, this, &ChainEditorWidget::OnSceneConnectionCreated);
    connect(scene, &QtNodes::FlowScene::connectionDeleted, this, &ChainEditorWidget::OnSceneConnectionRemoved);
}

QvMessageBusSlotImpl(ChainEditorWidget)
{
    switch (msg)
    {
        MBRetranslateDefaultImpl;
        MBUpdateColorSchemeDefaultImpl;
        default: break;
    }
}

void ChainEditorWidget::OnDispatcherChainedOutboundCreated(std::shared_ptr<OutboundObjectMeta> data, QtNodes::Node &node)
{
    outboundNodes[data->getDisplayName()] = node.id();
    const auto outboundCount = outboundNodes.count();
    const static int offsets[]{ 300, 0, -300 };
    auto pos = this->pos();
    pos.setX(pos.x() + GRAPH_GLOBAL_OFFSET_X + offsets[outboundCount % 3]);
    pos.setY(pos.y() + outboundCount * 100 + GRAPH_GLOBAL_OFFSET_Y);
    scene->setNodePosition(node, pos);
}

void ChainEditorWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type())
    {
        case QEvent::LanguageChange: retranslateUi(this); break;
        default: break;
    }
}

void ChainEditorWidget::BeginEditChain(const QString &chain)
{
    const auto index = chainComboBox->findText(chain);
    if (index >= 0)
    {
        // Triggers the on_chainComboBox_currentIndexChanged function.
        chainComboBox->setCurrentIndex(index);
    }
}

void ChainEditorWidget::ShowChainLinkedList()
{
    if (dispatcher->IsNodeConstructing())
        return;
    connectionSignalBlocked = true;
    const auto connections = scene->connections();
    for (const auto &connection : connections)
    {
        scene->deleteConnection(*connection.second);
    }
    //
    // Warn: Some outbound node MAY NOT be created YET!
    if (!currentChain)
        return;
    const auto &outbounds = currentChain->outboundTags;
    for (auto index = 0; index < outbounds.count() - 1; index++)
    {
        const auto &inTag = outbounds[index + 1];
        const auto &outTag = outbounds[index];
        bool hasErrorOccured = false;
        for (const auto &tag : { inTag, outTag })
        {
            if (!outboundNodes.contains(tag))
            {
                QvMessageBoxWarn(this, tr("Chain Editor"), tr("Could not find outbound tag: %1, The chain may be corrupted").arg(tag));
                hasErrorOccured = true;
            }
        }
        if (!hasErrorOccured)
        {
            const auto &nodeIn = scene->node(outboundNodes[inTag]);
            const auto &nodeOut = scene->node(outboundNodes[outTag]);
            scene->createConnection(*nodeIn, 0, *nodeOut, 0);
        }
    }
    connectionSignalBlocked = false;
}

std::tuple<bool, QString, QStringList> ChainEditorWidget::VerifyChainLinkedList(const QUuid &ignoredConnectionId)
{
    QList<QString> resultList;
    auto conns = scene->connections();
    bool needReIterate = true;
    const auto expectedChainLength = conns.size() + uint(ignoredConnectionId.isNull());
    for (auto i = 0u; i < expectedChainLength; i++)
    {
        auto iter = conns.begin();
        // Loop against all items.
        while (iter != conns.end())
        {
#define NEED_ITERATE(x)                                                                                                                              \
    needReIterate = x;                                                                                                                               \
    continue
            const auto &id = iter->first;
            const auto &connection = iter->second;
            const auto &nodeInId = connection->getNode(QtNodes::PortType::In)->id();
            const auto &nodeOutId = connection->getNode(QtNodes::PortType::Out)->id();
            //
            const auto &nextTag = outboundNodes.key(nodeInId);
            const auto &previousTag = outboundNodes.key(nodeOutId);
            //
            // If we are ignoring this node.
            if (id == ignoredConnectionId)
            {
                iter++;
                NEED_ITERATE(false);
            }
            //
            if (resultList.isEmpty())
            {
                iter++;
                resultList << previousTag << nextTag;
                NEED_ITERATE(false);
            }

            if (resultList.last() != previousTag && resultList.first() != nextTag)
            {
                iter++;
                NEED_ITERATE(true);
                // return std::make_tuple(false, tr("Two different chains detected."), QStringList());
            }

            if (resultList.contains(previousTag) && resultList.contains(nextTag))
            {
                iter++;
                NEED_ITERATE(true);
                // return std::make_tuple(false, tr("Looped chain detected."), QStringList());
            }

            if (resultList.last() == previousTag)
            {
                resultList << nextTag;
                iter = conns.erase(iter);
                NEED_ITERATE(false);
            }

            if (resultList.front() == nextTag)
            {
                resultList.prepend(previousTag);
                iter = conns.erase(iter);
                NEED_ITERATE(false);
            }
#undef NEED_ITERATE
        }
        if ((ulong) resultList.count() == expectedChainLength)
        {
            return { true, tr("OK"), resultList };
        }
    }
    return { false, tr("There's an error in your connection."), resultList };
}

void ChainEditorWidget::on_chainComboBox_currentIndexChanged(int arg1)
{
    currentChain = chains[chainComboBox->itemText(arg1)];
    ShowChainLinkedList();
}

void ChainEditorWidget::TrySaveChainOutboudData(const QUuid &ignoredConnectionId)
{
    if (!currentChain)
    {
        QvMessageBoxWarn(this, tr("Chain Editor"), tr("Please Select a Chain"));
        return;
    }
    const auto &[result, errMessage, list] = VerifyChainLinkedList(ignoredConnectionId);
    if (!result)
    {
        RED(statusLabel);
        statusLabel->setText(errMessage);
    }
    else
    {
        BLACK(statusLabel);
        statusLabel->setText(list.join(" >> "));
        currentChain->outboundTags = list;
    }
}

void ChainEditorWidget::OnSceneConnectionCreated(const QtNodes::Connection &)
{
    if (connectionSignalBlocked)
        return;
    TrySaveChainOutboudData();
}

void ChainEditorWidget::OnSceneConnectionRemoved(const QtNodes::Connection &c)
{
    if (connectionSignalBlocked)
        return;
    TrySaveChainOutboudData(c.id());
}

void ChainEditorWidget::OnDispatcherChainedOutboundDeleted(const OutboundObjectMeta &data)
{
    const auto displayName = data.getDisplayName();
    if (outboundNodes.contains(displayName))
    {
        scene->removeNode(*scene->node(outboundNodes[displayName]));
        outboundNodes.remove(displayName);
    }
}

void ChainEditorWidget::OnDispatcherChainCreated(std::shared_ptr<OutboundObjectMeta> data)
{
    const auto displayName = data->getDisplayName();
    chains[displayName] = data;
    chainComboBox->addItem(displayName);
}

void ChainEditorWidget::OnDispatcherChainDeleted(const OutboundObjectMeta &data)
{
    const auto displayName = data.getDisplayName();
    const auto index = chainComboBox->findText(displayName);
    chainComboBox->removeItem(index);
    chains.remove(displayName);
}

void ChainEditorWidget::OnDispatcherObjectTagChanged(ComplexTagNodeMode mode, const QString originalTag, const QString newTag)
{
    if (mode == NODE_OUTBOUND)
    {
        // Simply compare if there is a match (Since no duplication of DisplayName should occur.)
        if (outboundNodes.contains(originalTag))
            outboundNodes[newTag] = outboundNodes.take(originalTag);

        // Check Chains.
        if (chains.contains(originalTag))
            chains[newTag] = chains.take(originalTag);

        const auto index = chainComboBox->findText(originalTag);
        if (index >= 0)
        {
            chainComboBox->setItemText(index, newTag);
        }

        for (const auto &chain : chains)
        {
            if (chain->outboundTags.contains(originalTag))
                chain->outboundTags.replace(chain->outboundTags.indexOf(originalTag), newTag);
        }
    }
}
