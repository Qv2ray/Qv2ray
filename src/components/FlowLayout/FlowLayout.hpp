#pragma once

#include <QLayout>
#include <QRect>
#include <QStyle>

namespace Qv2ray::components
{
    class FlowLayout : public QLayout
    {
        Q_OBJECT

      public:
        explicit FlowLayout(int margin = -1, int hSpacing = -1, int vSpacing = -1, QWidget *parent = nullptr);
        ~FlowLayout();

        int count() const override;
        int horizontalSpacing() const;
        int verticalSpacing() const;
        Qt::Orientations expandingDirections() const override;
        bool hasHeightForWidth() const override;
        int heightForWidth(int) const override;
        QLayoutItem *itemAt(int index) const override;
        QSize minimumSize() const override;
        void setGeometry(const QRect &rect) override;
        QSize sizeHint() const override;
        QLayoutItem *takeAt(int index) override;
        void addItem(QLayoutItem *item) override;
        void addWidgetAt(QWidget *item, int index);

      private:
        int doLayout(QRect rect, bool testOnly) const;
        int smartSpacing(QStyle::PixelMetric pm) const;

        QList<QLayoutItem *> itemList;
        int m_hSpace;
        int m_vSpace;
    };
} // namespace Qv2ray::components
