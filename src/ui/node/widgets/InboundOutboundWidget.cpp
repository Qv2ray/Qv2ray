#include "InboundOutboundWidget.hpp"

#include "base/Qv2rayBase.hpp"
#include "core/CoreUtils.hpp"
#include "ui/editors/w_InboundEditor.hpp"
#include "ui/editors/w_JsonEditor.hpp"
#include "ui/editors/w_OutboundEditor.hpp"

InboundOutboundWidget::InboundOutboundWidget(WidgetMode mode, std::shared_ptr<NodeDispatcher> _d, QWidget *parent) : QvNodeWidget(_d, parent)
{
    workingMode = mode;
    setupUi(this);
}

void InboundOutboundWidget::setValue(std::shared_ptr<INBOUND> data)
{
    assert(workingMode == MODE_INBOUND);
    inboundObject = data;
    tagTxt->setText(getTag(*data));
}

void InboundOutboundWidget::setValue(std::shared_ptr<OutboundObjectMeta> data)
{
    assert(workingMode == MODE_OUTBOUND);
    outboundObject = data;
    tagTxt->setText(getTag(outboundObject->realOutbound));
}

void InboundOutboundWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type())
    {
        case QEvent::LanguageChange:
        {
            retranslateUi(this);
            break;
        }
        default: break;
    }
}

void InboundOutboundWidget::on_editBtn_clicked()
{
    switch (workingMode)
    {
        case MODE_INBOUND:
        {
            InboundEditor editor{ *inboundObject, parentWidget() };
            *inboundObject = editor.OpenEditor();
            // Set tag
            const auto newTag = getTag(*inboundObject);
            tagTxt->setText(newTag);
            (*inboundObject)["tag"] = newTag;
            break;
        }
        case MODE_OUTBOUND:
        {
            OutboundEditor editor{ outboundObject->realOutbound, parentWidget() };
            outboundObject->realOutbound = editor.OpenEditor();
            // Set tag
            const auto newTag = getTag(outboundObject->realOutbound);
            tagTxt->setText(newTag);
            outboundObject->realOutbound["tag"] = newTag;
            break;
        }
    }
}

void InboundOutboundWidget::on_editJsonBtn_clicked()
{
    switch (workingMode)
    {
        case MODE_INBOUND:
        {
            JsonEditor editor{ *inboundObject, parentWidget() };
            *inboundObject = INBOUND{ editor.OpenEditor() };
            const auto newTag = getTag(*inboundObject);
            // Set tag
            tagTxt->setText(newTag);
            (*inboundObject)["tag"] = newTag;
            break;
        }
        case MODE_OUTBOUND:
        {
            JsonEditor editor{ outboundObject->realOutbound, parentWidget() };
            outboundObject->realOutbound = OUTBOUND{ editor.OpenEditor() };
            const auto newTag = getTag(outboundObject->realOutbound);
            // Set tag
            tagTxt->setText(newTag);
            outboundObject->realOutbound["tag"] = newTag;
            break;
        }
    }
}

void InboundOutboundWidget::on_tagTxt_textEdited(const QString &arg1)
{
    switch (workingMode)
    {
        case MODE_INBOUND:
        {
            (*inboundObject)["tag"] = arg1;
            break;
        }
        case MODE_OUTBOUND:
        {
            outboundObject->realOutbound["tag"] = arg1;
            break;
        }
    }
}
