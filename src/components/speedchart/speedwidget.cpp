/*
 * Bittorrent Client using Qt and libtorrent.
 * Copyright (C) 2015 Anton Lashkov <lenton_91@mail.ru>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 *
 * In addition, as a special exception, the copyright holders give permission to
 * link this program with the OpenSSL project's "OpenSSL" library (or with
 * modified versions of it that use the same license as the "OpenSSL" library),
 * and distribute the linked executables. You must obey the GNU General Public
 * License in all respects for all of the code used other than "OpenSSL".  If
 * you modify file(s), you may extend this exception to your version of the
 * file(s), but you are not obligated to do so. If you do not wish to do so,
 * delete this exception statement from your version.
 */

#include "speedwidget.hpp"

#include <QDateTime>
#include <QHBoxLayout>
#include <QLabel>
#include <QLocale>
#include <QMenu>
#include <QPainter>
#include <QPen>
#include <QTimer>
#include <QVBoxLayout>
#include <list>

SpeedWidget::SpeedWidget(QWidget *parent) : QGraphicsView(parent), m_currentData(&m_datahalfMin)
{
    //    m_layout = new QVBoxLayout(this);
    //    m_layout->setContentsMargins(0, 0, 0, 0);
    //    m_layout->setSpacing(3);
    //    m_hlayout = new QHBoxLayout();
    //    m_hlayout->setContentsMargins(0, 0, 0, 0);
    //    m_hlayout->addStretch();
    //    m_plot = new SpeedWidget(this);
    //    m_layout->addLayout(m_hlayout);
    //    m_layout->addWidget(m_plot);
    //    m_plot->show();
}

void SpeedWidget::AddPointData(QMap<SpeedWidget::GraphID, long> data)
{
    SpeedWidget::PointData point;
    point.x = QDateTime::currentMSecsSinceEpoch() / 1000;
    for (const auto &[id, data] : data.toStdMap())
    {
        point.y[id] = data;
    }
    PushPoint(point);
    replot();
}

#define VIEWABLE 120

// use binary prefix standards from IEC 60027-2
// see http://en.wikipedia.org/wiki/Kilobyte
enum SizeUnit : int
{
    Byte,     // 1024^0,
    KibiByte, // 1024^1,
    MebiByte, // 1024^2,
    GibiByte, // 1024^3,
    TebiByte, // 1024^4,
    PebiByte, // 1024^5,
    ExbiByte  // 1024^6
};

QString unitString(const SizeUnit unit, const bool isSpeed)
{
    const static QStringList units{ "B", "KiB", "MiB", "GiB", "TiB", "EiB" };
    auto unitString = units[unit];
    if (isSpeed)
        unitString += "/s";
    return unitString;
}

int friendlyUnitPrecision(const SizeUnit unit)
{
    // friendlyUnit's number of digits after the decimal point
    switch (unit)
    {
        case SizeUnit::Byte: return 0;
        case SizeUnit::KibiByte:
        case SizeUnit::MebiByte: return 1;
        case SizeUnit::GibiByte: return 2;
        default: return 3;
    }
}

namespace
{
    // table of supposed nice steps for grid marks to get nice looking quarters
    // of scale
    const static double roundingTable[] = { 1.2, 1.6, 2, 2.4, 2.8, 3.2, 4, 6, 8 };
    struct SplittedValue
    {
        double arg;
        SizeUnit unit;
        qint64 sizeInBytes() const
        {
            auto size = arg;
            for (int i = 0; i < static_cast<int>(unit); ++i)
            {
                size *= 1024;
            }
            return size;
        }
    };

    SplittedValue getRoundedYScale(double value)
    {
        if (value == 0.0)
            return { 0, SizeUnit::Byte };

        if (value <= 12.0)
            return { 12, SizeUnit::Byte };

        SizeUnit calculatedUnit = SizeUnit::Byte;

        while (value > 1024)
        {
            value /= 1024;
            calculatedUnit = static_cast<SizeUnit>(static_cast<int>(calculatedUnit) + 1);
        }

        if (value > 100.0)
        {
            int roundedValue = static_cast<int>(value / 40) * 40;
            while (roundedValue < value) roundedValue += 40;
            return { static_cast<double>(roundedValue), calculatedUnit };
        }

        if (value > 10.0)
        {
            int roundedValue = static_cast<int>(value / 4) * 4;
            while (roundedValue < value) roundedValue += 4;
            return { static_cast<double>(roundedValue), calculatedUnit };
        }

        for (const auto &roundedValue : roundingTable)
        {
            if (value <= roundedValue)
                return { roundedValue, calculatedUnit };
        }

        return { 10.0, calculatedUnit };
    }

    QString formatLabel(const double argValue, const SizeUnit unit)
    {
        // check is there need for digits after decimal separator
        const int precision = (argValue < 10) ? friendlyUnitPrecision(unit) : 0;
        return QLocale::system().toString(argValue, 'f', precision) + " " + unitString(unit, true);
    }
} // namespace

void SpeedWidget::UpdateSpeedPlotSettings(bool isOutboundGraph, bool hasDirectGraph)
{
    this->isOutbound = isOutboundGraph;
    this->hasDirectLine = hasDirectGraph;
    //
    QPen greenPen{ QColor(134, 196, 63) };
    greenPen.setWidthF(1.5);
    QPen bluePen{ QColor(50, 153, 255) };
    bluePen.setWidthF(1.5);
    //
    m_properties.clear();
    if (isOutboundGraph)
    {
        m_properties[OUTBOUND_PROXY_UP] = GraphProperties(tr("Proxy Upload"), bluePen);
        m_properties[OUTBOUND_PROXY_DOWN] = GraphProperties(tr("Proxy Download"), greenPen);
        if (hasDirectLine)
        {
            QPen cyanPen{ QColor(0, 210, 240) };
            cyanPen.setWidthF(1.5);
            QPen orangePen{ QColor(255, 220, 42) };
            orangePen.setWidthF(1.5);
            m_properties[OUTBOUND_DIRECT_UP] = GraphProperties(tr("Direct Upload"), cyanPen);
            m_properties[OUTBOUND_DIRECT_DOWN] = GraphProperties(tr("Direct Download"), orangePen);
        }
    }
    else
    {
        m_properties[INBOUND_UP] = GraphProperties(tr("Total Inbound Upload"), bluePen);
        m_properties[INBOUND_DOWN] = GraphProperties(tr("Total Inbound Download"), greenPen);
    }
}

void SpeedWidget::Clear()
{
    m_datahalfMin.clear();
    replot();
}

void SpeedWidget::PushPoint(const SpeedWidget::PointData &point)
{
    m_datahalfMin.push_back(point);

    while (m_datahalfMin.length() > VIEWABLE)
    {
        m_datahalfMin.removeFirst();
    }
}

void SpeedWidget::replot()
{
    viewport()->update();
}

quint64 SpeedWidget::maxYValue()
{
    quint64 maxYValue = 0;

    for (int id = 0; id < NB_GRAPHS; ++id)
    {
        // 30 is half min
        for (int i = m_currentData->size() - 1, j = 0; (i >= 0) && (j <= VIEWABLE); --i, ++j)
        {
            if (m_currentData->at(i).y[id] > maxYValue)
                maxYValue = m_currentData->at(i).y[id];
        }
    }

    return maxYValue;
}

void SpeedWidget::paintEvent(QPaintEvent *)
{
    const auto fullRect = viewport()->rect();
    auto rect = viewport()->rect();
    rect.adjust(4, 4, 0, -4); // Add padding
    const auto niceScale = getRoundedYScale(maxYValue());
    // draw Y axis speed labels
    const QVector<QString> speedLabels = {
        formatLabel(niceScale.arg, niceScale.unit),
        formatLabel((0.75 * niceScale.arg), niceScale.unit),
        formatLabel((0.50 * niceScale.arg), niceScale.unit),
        formatLabel((0.25 * niceScale.arg), niceScale.unit),
        formatLabel(0.0, niceScale.unit),
    };

    QPainter painter(viewport());
    painter.setRenderHints(QPainter::Antialiasing);
    //
    const auto fontMetrics = painter.fontMetrics();
    rect.adjust(0, fontMetrics.height(), 0, 0); // Add top padding for top speed text
    //
    int yAxisWidth = 0;
    for (const auto &label : speedLabels)
        if (fontMetrics.horizontalAdvance(label) > yAxisWidth)
            yAxisWidth = fontMetrics.horizontalAdvance(label);

    int i = 0;
    for (const auto &label : speedLabels)
    {
        QRectF labelRect(rect.topLeft() + QPointF(-yAxisWidth, (i++) * 0.25 * rect.height() - fontMetrics.height()),
                         QSizeF(2 * yAxisWidth, fontMetrics.height()));
        painter.drawText(labelRect, label, Qt::AlignRight | Qt::AlignTop);
    }

    // draw grid lines
    rect.adjust(yAxisWidth + 4, 0, 0, 0);
    QPen gridPen;
    gridPen.setStyle(Qt::DashLine);
    gridPen.setWidthF(1);
    gridPen.setColor(QColor(128, 128, 128, 128));
    // Set antialiasing for graphs
    painter.setPen(gridPen);
    painter.drawLine(fullRect.left(), rect.top(), rect.right(), rect.top());
    painter.drawLine(fullRect.left(), rect.top() + 0.25 * rect.height(), rect.right(), rect.top() + 0.25 * rect.height());
    painter.drawLine(fullRect.left(), rect.top() + 0.50 * rect.height(), rect.right(), rect.top() + 0.50 * rect.height());
    painter.drawLine(fullRect.left(), rect.top() + 0.75 * rect.height(), rect.right(), rect.top() + 0.75 * rect.height());
    painter.drawLine(fullRect.left(), rect.bottom(), rect.right(), rect.bottom());

    constexpr auto TIME_AXIS_DIVISIONS = 6;
    for (int i = 0; i < TIME_AXIS_DIVISIONS; ++i)
    {
        const int x = rect.left() + (i * rect.width()) / TIME_AXIS_DIVISIONS;
        painter.drawLine(x, fullRect.top(), x, fullRect.bottom());
    }

    // draw graphs
    // Need, else graphs cross left gridline
    rect.adjust(3, 0, 0, 0);
    //
    const double yMultiplier = std::max(niceScale.arg, (static_cast<double>(rect.height()) / niceScale.sizeInBytes()));
    // const double yMultiplier = (niceScale.arg == 0.0) ? 0.0 : (static_cast<double>(rect.height()) / niceScale.sizeInBytes());
    //
    const double xTickSize = static_cast<double>(rect.width()) / VIEWABLE;

    for (int id = 0; id < NB_GRAPHS; ++id)
    {
        QVector<QPoint> points;

        for (int i = static_cast<int>(m_currentData->size()) - 1, j = 0; (i >= 0) && (j <= VIEWABLE); --i, ++j)
        {
            const int newX = rect.right() - j * xTickSize;
            const int newY = rect.bottom() - m_currentData->at(i).y[id] * yMultiplier;
            points.push_back({ newX, newY });
        }

        painter.setPen(m_properties[static_cast<GraphID>(id)].pen);
        painter.drawPolyline(points.data(), points.size());
    }

    // draw legend
    double legendHeight = 0;
    int legendWidth = 0;

    for (const auto &property : m_properties)
    {
        if (fontMetrics.horizontalAdvance(property.name) > legendWidth)
            legendWidth = fontMetrics.horizontalAdvance(property.name);

        legendHeight += 1.5 * fontMetrics.height();
    }

    {
        QPoint legendTopLeft(rect.left() + 4, fullRect.top() + 4);
        QRectF legendBackgroundRect(QPoint(legendTopLeft.x() - 4, legendTopLeft.y() - 4), QSizeF(legendWidth + 8, legendHeight + 8));
        auto legendBackgroundColor = QWidget::palette().color(QWidget::backgroundRole());
        legendBackgroundColor.setAlpha(128); // 50% transparent
        painter.fillRect(legendBackgroundRect, legendBackgroundColor);

        int i = 0;
        for (const auto &property : m_properties)
        {
            int nameSize = fontMetrics.horizontalAdvance(property.name);
            double indent = 1.5 * (i++) * fontMetrics.height();
            painter.setPen(property.pen);
            painter.drawLine(legendTopLeft + QPointF(0, indent + fontMetrics.height()),
                             legendTopLeft + QPointF(nameSize, indent + fontMetrics.height()));
            painter.drawText(QRectF(legendTopLeft + QPointF(0, indent), QSizeF(2 * nameSize, fontMetrics.height())), property.name,
                             QTextOption(Qt::AlignVCenter));
        }
    }
}
