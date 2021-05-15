#include "w_ChainSha256Editor.hpp"

#include "ui/widgets/common/WidgetUIBase.hpp"
#include "utils/QvHelpers.hpp"

ChainSha256Editor::ChainSha256Editor(QString raw, QWidget *parent) : QDialog(parent)
{
    setupUi(this);
    QvMessageBusConnect(ChainSha256Editor);

    original = raw;
    final = raw;
    chainSha256Edit->setPlainText(raw);
}

QvMessageBusSlotImpl(ChainSha256Editor)
{
    switch (msg)
    {
        MBShowDefaultImpl;
        MBHideDefaultImpl;
        MBRetranslateDefaultImpl;
        case UPDATE_COLORSCHEME: break;
    }
}

QString ChainSha256Editor::OpenEditor()
{
    int resultCode = this->exec();
    return resultCode == QDialog::Accepted ? final : original;
}

ChainSha256Editor::~ChainSha256Editor()
{
}

void ChainSha256Editor::on_chainSha256Edit_textChanged()
{
    final = chainSha256Edit->toPlainText();
}
