#include "OutboundWidget.hpp"

#include "base/Qv2rayBase.hpp"
#include "core/CoreUtils.hpp"
#include "ui/editors/w_JsonEditor.hpp"
#include "ui/editors/w_OutboundEditor.hpp"

OutboundWidget::OutboundWidget(QWidget *parent) : QvNodeWidget(parent)
{
    setupUi(this);
}

void OutboundWidget::setValue(std::shared_ptr<OutboundObjectMeta> data)
{
    outboundObject = data;
    tagTxt->setText(getTag(outboundObject->realOutbound));
}

void OutboundWidget::changeEvent(QEvent *e)
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

void OutboundWidget::on_editBtn_clicked()
{
    OutboundEditor editor{ outboundObject->realOutbound, parentWidget() };
    outboundObject->realOutbound = editor.OpenEditor();
    // Set tag
    const auto newTag = getTag(outboundObject->realOutbound);
    tagTxt->setText(newTag);
    outboundObject->realOutbound["tag"] = newTag;
}

void OutboundWidget::on_editJsonBtn_clicked()
{
    JsonEditor editor{ outboundObject->realOutbound, parentWidget() };
    outboundObject->realOutbound = OUTBOUND{ editor.OpenEditor() };
    const auto newTag = getTag(outboundObject->realOutbound);
    // Set tag
    tagTxt->setText(newTag);
    outboundObject->realOutbound["tag"] = newTag;
}

void OutboundWidget::on_tagTxt_textEdited(const QString &arg1)
{
    outboundObject->realOutbound["tag"] = arg1;
}
