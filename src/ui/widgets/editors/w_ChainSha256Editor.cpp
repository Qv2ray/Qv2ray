#include "w_ChainSha256Editor.hpp"

#include "ui/widgets/common/WidgetUIBase.hpp"
#include "utils/QvHelpers.hpp"

#include <QDesktopServices>

ChainSha256Editor::ChainSha256Editor(QWidget *parent, const QList<QString> &chain) : QDialog(parent)
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
    const static QRegularExpression newLine("[\r\n]");
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    return str.split(newLine, Qt::SplitBehaviorFlags::SkipEmptyParts);
#else
    return str.split(newLine, QString::SkipEmptyParts);
#endif
}

std::optional<QString> ChainSha256Editor::validateError(const QList<QString> &newChain)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
    const static QRegularExpression sha256(QRegularExpression::anchoredPattern("[0-9a-fA-F]{64}"));
#else
    const static QRegularExpression sha256("^[0-9a-fA-F]{64}$");
#endif
    for (const auto &entry : newChain)
    {
        if(!sha256.match(entry).hasMatch())
            return tr("invalid SHA256: %1").arg(entry);
    }

    return std::nullopt;
}

void ChainSha256Editor::on_buttonBox_helpRequested()
{
    QDesktopServices::openUrl(QUrl("https://www.v2fly.org/config/transport.html#tlsobject"));
}
