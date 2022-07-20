#include "QvPlugin/Common/EditorCreatorDefs.hpp"

#include <QWidget>

QT_BEGIN_NAMESPACE
class QListWidget;
QT_END_NAMESPACE

class ConfigurableEditor : public QWidget
{
    Q_OBJECT

    using _ElementType = Qv2rayPlugin::Common::EditorCreator::ElementType;
    using _EditorInfo = Qv2rayPlugin::Common::EditorCreator::EditorInfo;

  public:
    explicit ConfigurableEditor(_ElementType arrayType, const QList<_EditorInfo> &subObjects, QWidget *parent = nullptr);
    explicit ConfigurableEditor(const QList<_EditorInfo> &_info, QWidget *parent = nullptr);
    void SetContent(const QVariant &c);
    QVariant GetContent() const;

  private:
    const bool isArrayType;

    // For objects
    QVariantMap contentObject;
    const QList<_EditorInfo> info;

    // For arrays
    QListWidget *arrayItemListView;
    QVariantList arrayContentList;
};
