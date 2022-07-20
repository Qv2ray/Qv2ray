#include "TagLineEditorWidget.hpp"

#include <QAction>
#include <QEvent>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QTimer>

const static inline auto ClearStyle = u"#_tagframe { border: 1px solid #00ffc898; }"_qs;
const static inline auto SelectedStyle = u"#_tagframe { border: 1px solid #ffffc898; background: #40cbba00; }"_qs;

using namespace Qv2ray::components;
using namespace std::chrono_literals;

TagEntryWidget::TagEntryWidget(const QString &name, QWidget *parent) : QFrame(parent), label(new QLabel{ name }), text(name)
{
    setContextMenuPolicy(Qt::ActionsContextMenu);

    const auto deleteAction = new QAction(tr("Delete Tag"), this);
    addAction(deleteAction);
    connect(deleteAction, &QAction::triggered, this, [this] { emit OnTagDeleted(text, {}); });

    setStyleSheet(ClearStyle);
    setObjectName(u"_tagframe"_qs);
    setMouseTracking(true);

    label->installEventFilter(this);
    label->setMargin(3);

    setLayout(new QHBoxLayout);
    layout()->addWidget(label);
    layout()->setContentsMargins(0, 0, 0, 0);
}

QString TagEntryWidget::GetTag() const
{
    return text;
}

bool TagEntryWidget::event(QEvent *event)
{
    switch (event->type())
    {
        case QEvent::Enter:
        {
            setCursor(Qt::PointingHandCursor);
            setStyleSheet(SelectedStyle);
            return true;
        }
        case QEvent::Leave:
        {
            setCursor(Qt::ArrowCursor);
            setStyleSheet(ClearStyle);
            return true;
        }
        case QEvent::MouseButtonRelease:
        {
            emit OnTagClicked(text);
            return true;
        }
        default: break;
    }
    return QWidget::event(event);
}

TagsLineEdit::TagsLineEdit(QWidget *parent) : QWidget(parent), lineEdit(new QLineEdit), _layout(new FlowLayout)
{
    lineEdit->installEventFilter(this);
    setLayout(_layout);
    _layout->setContentsMargins(0, 0, 0, 0);
    _layout->addWidget(lineEdit);
}

bool TagsLineEdit::eventFilter(QObject *target, QEvent *event)
{
    if (target == lineEdit && event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Backspace)
        {
            const auto text = lineEdit->text();
            if (!text.isEmpty() || _layout->count() <= 1)
                return QWidget::eventFilter(target, event);

            const auto widget = qobject_cast<TagEntryWidget *>(_layout->itemAt(_layout->count() - 2)->widget());

            if (!widget)
            {
                qWarning() << "Invalid Widget";
                return true;
            }
            const auto tag = widget->GetTag();
            lineEdit->setText(tag);
            DeleteTag(tag);
            return true;
        }
        else if (keyEvent->key() == Qt::Key_Space || keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return || //
                 keyEvent->key() == Qt::Key_Comma || keyEvent->key() == Qt::Key_Semicolon)
        {
            const auto text = lineEdit->text().trimmed();
            if (text.isEmpty())
                return QWidget::eventFilter(target, event);
            AddTag(text);
            lineEdit->clear();
            return true;
        }
    }
    return QWidget::eventFilter(target, event);
}

void TagsLineEdit::SetTags(const QSet<QString> &_tags)
{
    blockSignals(true);
    {
        const auto count = _layout->count();
        for (auto i = 0; i < count - 1; i++)
        {
            const auto item = _layout->takeAt(0);
            delete item->widget();
            delete item;
        }
        tagsSet.clear();
    }
    blockSignals(false);

    tagsSet.reserve(_tags.size());
    for (const auto &str : _tags)
        AddTag(str);
    updateGeometry();
}

QStringList TagsLineEdit::GetTags()
{
    return { tagsSet.begin(), tagsSet.end() };
}

void TagsLineEdit::DeleteTag(const QString &s)
{
    for (auto i = _layout->count() - 2; i >= 0; i--)
    {
        const auto widget = qobject_cast<TagEntryWidget *>(_layout->itemAt(i)->widget());
        assert(widget);

        const auto tag = widget->GetTag();
        if (tag != s)
            continue;

        tagsSet.remove(s);
        emit OnTagsChanged(GetTags());
        widget->deleteLater();
        lineEdit->setFocus();
        updateGeometry();
        break;
    }
}

void TagsLineEdit::AddTag(const QString &str)
{
    if (tagsSet.contains(str))
        return;
    tagsSet << str;
    auto w = new TagEntryWidget{ str };
    _layout->addWidgetAt(w, _layout->count() - 1);
    connect(w, &TagEntryWidget::OnTagClicked, this, &TagsLineEdit::OnTagClicked);
    connect(w, &TagEntryWidget::OnTagDeleted, this, &TagsLineEdit::DeleteTag);
    emit OnTagsChanged(GetTags());
}
