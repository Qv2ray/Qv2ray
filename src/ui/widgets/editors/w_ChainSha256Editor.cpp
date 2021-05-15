#include "w_ChainSha256Editor.hpp"

#include "ui/widgets/common/WidgetUIBase.hpp"
#include "utils/QvHelpers.hpp"

#include <QDesktopServices>
#include <utility>

ChainSha256Editor::ChainSha256Editor(QWidget *parent, const QList<QString>& chain) : QDialog(parent)
{
    setupUi(this);
    chainSha256Edit->setPlainText(chain.join("\r\n"));
    QvMessageBusConnect(ChainSha256Editor);
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

void ChainSha256Editor::accept()
{
    const auto newChain = ChainSha256Editor::convertFromString(chainSha256Edit->toPlainText());
    if (const auto err = ChainSha256Editor::validateError(newChain); err)
    {
        QvMessageBoxWarn(this, tr("Invalid Certificate Hash Chain"), *err);
        return;
    }
    this->chain = newChain;
    this->QDialog::accept();
}

QList<QString> ChainSha256Editor::convertFromString(const QString &&str)
{
    const static QRegExp newLine("[\r\n]");
    return str.split(newLine, Qt::SkipEmptyParts);
}

std::optional<QString> ChainSha256Editor::validateError(const QList<QString> &newChain)
{
    const static QRegExp sha256("[0-9a-fA-F]{64}");

    for (const auto &entry : newChain)
    {
        if (!sha256.exactMatch(entry))
            return tr("invalid SHA256: %1").arg(entry);
    }

    return std::nullopt;
}

void ChainSha256Editor::on_buttonBox_helpRequested()
{
    QDesktopServices::openUrl(QUrl("https://www.v2fly.org/config/transport.html#tlsobject"));
}
