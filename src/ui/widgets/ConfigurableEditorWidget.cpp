#include "ConfigurableEditorWidget.hpp"

#include <QCheckBox>
#include <QDialog>
#include <QDoubleSpinBox>
#include <QEventLoop>
#include <QFormLayout>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QToolButton>

ConfigurableEditor::ConfigurableEditor(_ElementType arrayType, const QList<_EditorInfo> &subObjects, QWidget *parent) : QWidget(parent), isArrayType(true)
{
    arrayItemListView = new QListWidget(this);

    QPushButton *addBtn = new QPushButton(this);
    QPushButton *removeBtn = new QPushButton(this);
    addBtn->setIcon(QIcon::fromTheme(u"add"_qs));
    removeBtn->setIcon(QIcon::fromTheme(u"remove"_qs));

    _EditorInfo contentObjectInfo;
    contentObjectInfo.key = u"item"_qs;
    contentObjectInfo.name = tr("Item");
    contentObjectInfo.type = arrayType;
    contentObjectInfo.objectElements = subObjects;

    ConfigurableEditor *contentWidget = new ConfigurableEditor({ contentObjectInfo }, this);
    contentWidget->setEnabled(false);

    {
        // Setup layouts
        QHBoxLayout *horizontalLayout = new QHBoxLayout();
        horizontalLayout->addWidget(addBtn);
        horizontalLayout->addWidget(removeBtn);

        QVBoxLayout *verticalLayout = new QVBoxLayout();
        verticalLayout->addWidget(arrayItemListView);
        verticalLayout->addLayout(horizontalLayout);

        QGridLayout *gridLayout = new QGridLayout(this);
        gridLayout->addLayout(verticalLayout, 0, 0, 1, 1);
        gridLayout->addWidget(contentWidget, 0, 1, 1, 1);
        gridLayout->setColumnStretch(1, 1);
    }

    layout()->setContentsMargins(0, 0, 0, 0);

    connect(arrayItemListView, &QListWidget::currentItemChanged, this,
            [this, contentWidget](QListWidgetItem *current, QListWidgetItem *previous)
            {
                if (previous)
                {
                    const auto content = contentWidget->GetContent().toMap().value(u"item"_qs);
                    arrayContentList.replace(arrayItemListView->indexFromItem(previous).row(), content);
                    previous->setText(content.toString().isEmpty() ? u"[Item]"_qs : content.toString());
                }
                contentWidget->setEnabled(current != nullptr);

                if (current)
                    contentWidget->SetContent(QVariantMap{ { u"item"_qs, arrayContentList.at(arrayItemListView->indexFromItem(current).row()) } });
            });

    connect(addBtn, &QToolButton::clicked, this,
            [this, _arrayType = arrayType]()
            {
                const auto index = arrayItemListView->currentRow() + 1;
                arrayContentList.insert(index, QVariant(QMetaType((int) _arrayType)));
                arrayItemListView->insertItem(index, u"Item"_qs);
                arrayItemListView->setCurrentRow(index);
            });

    connect(removeBtn, &QToolButton::clicked, this,
            [this, contentWidget]()
            {
                const auto item = arrayItemListView->currentItem();
                if (!item)
                    return;
                const auto index = arrayItemListView->indexFromItem(item).row();
                delete arrayItemListView->takeItem(index);
                arrayContentList.removeAt(index);
                contentWidget->setEnabled(false);
            });
}

ConfigurableEditor::ConfigurableEditor(const QList<_EditorInfo> &_info, QWidget *parent) : QWidget(parent), isArrayType(false), info(_info)
{
    const auto _layout = new QFormLayout;
    setLayout(_layout);
    _layout->setContentsMargins(0, 0, 0, 0);

    for (const auto &[key, name, type, childObjectChildren, childArrayType] : info)
    {
        QWidget *widget;
        switch (type)
        {
            case _ElementType::Bool:
            {
                widget = new QCheckBox(this);
                connect((QCheckBox *) widget, &QCheckBox::toggled, this, [this, _key = key](bool b) { this->contentObject.insert(_key, b); });
                break;
            }
            case _ElementType::Double:
            {
                widget = new QDoubleSpinBox(this);
                connect((QDoubleSpinBox *) widget, &QDoubleSpinBox::valueChanged, this, [this, _key = key](double b) { this->contentObject.insert(_key, b); });
                break;
            }
            case _ElementType::Integer:
            {
                widget = new QSpinBox(this);
                connect((QSpinBox *) widget, &QSpinBox::valueChanged, this, [this, _key = key](int b) { this->contentObject.insert(_key, b); });
                break;
            }
            case _ElementType::String:
            {
                widget = new QLineEdit(this);
                connect((QLineEdit *) widget, &QLineEdit::textEdited, this,
                        [this, _key = key](const QString &str)
                        {
                            if (str.isEmpty())
                                this->contentObject.remove(_key);
                            else
                                this->contentObject.insert(_key, str);
                        });
                break;
            }
            case _ElementType::Object:
            case _ElementType::Array:
            {
                widget = new QPushButton(tr("Edit") + u" "_qs + name, this);
                if (type == _ElementType::Object)
                {
                    const auto func = [this, _key = key, _children = childObjectChildren]()
                    {
                        QDialog dialog{ this };
                        dialog.setLayout(new QGridLayout);
                        dialog.layout()->setContentsMargins(9, 9, 9, 9);
                        ConfigurableEditor d{ _children, &dialog };
                        d.SetContent(this->contentObject.value(_key, QVariantMap{}));
                        dialog.layout()->addWidget(&d);
                        dialog.exec();
                        this->contentObject.insert(_key, d.GetContent());
                    };
                    connect((QPushButton *) widget, &QPushButton::clicked, this, func);
                }
                else
                {
                    const auto func = [this, _key = key, _arrayType = childArrayType, _children = childObjectChildren]()
                    {
                        QDialog dialog{ this };
                        dialog.setLayout(new QGridLayout);
                        dialog.layout()->setContentsMargins(9, 9, 9, 9);
                        ConfigurableEditor d{ _arrayType, _children, &dialog };
                        d.SetContent(this->contentObject.value(_key, QVariantList{}));
                        dialog.layout()->addWidget(&d);
                        dialog.exec();
                        this->contentObject.insert(_key, d.GetContent());
                    };
                    connect((QPushButton *) widget, &QPushButton::clicked, this, func);
                }
                break;
            }
        }

        widget->setObjectName(key);
        _layout->addRow(new QLabel(name, this), widget);
    }
}

void ConfigurableEditor::SetContent(const QVariant &c)
{
    if (isArrayType)
    {
        assert(arrayItemListView);
        arrayContentList = c.isValid() ? c.toList() : QVariantList{};
        for (const auto &item : arrayContentList)
            arrayItemListView->addItem(item.toString().isEmpty() ? u"[Item]"_qs : item.toString());
    }
    else
    {
        contentObject = c.toMap();
        for (const auto &[key, name, type, childObjectChildren, childArrayType] : info)
        {
            auto widget = findChild<QWidget *>(key);
            if (!widget)
            {
                qWarning() << "Failed to find child object named" << key;
                continue;
            }

            switch (type)
            {
                case _ElementType::Bool:
                {
                    qobject_cast<QCheckBox *>(widget)->setChecked(contentObject.value(key).toBool());
                    break;
                }
                case _ElementType::Double:
                {
                    qobject_cast<QDoubleSpinBox *>(widget)->setValue(contentObject.value(key).toDouble());
                    break;
                }
                case _ElementType::Integer:
                {
                    qobject_cast<QSpinBox *>(widget)->setValue(contentObject.value(key).toLongLong());
                    break;
                }
                case _ElementType::String:
                {
                    qobject_cast<QLineEdit *>(widget)->setText(contentObject.value(key).toString());
                    break;
                }
                case _ElementType::Object:
                case _ElementType::Array:
                {
                    // Will set content when click on the button.
                    break;
                }
            }
        }
    }
}

QVariant ConfigurableEditor::GetContent() const
{
    return isArrayType ? QVariant{ arrayContentList } : QVariant{ contentObject };
}
