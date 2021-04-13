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

#include "base/Qv2rayBase.hpp"

#include <QPainter>

#define VIEWABLE 120

// table of supposed nice steps for grid marks to get nice looking quarters of scale
const static double roundingTable[] = { 1.2, 1.6, 2, 2.4, 2.8, 3.2, 4, 6, 8 };

SpeedWidget::SpeedWidget(QWidget *parent) : QGraphicsView(parent)
{
    UpdateSpeedPlotSettings();
}

void SpeedWidget::AddPointData(QMap<SpeedWidget::GraphType, long> data)
{
    SpeedWidget::PointData point;
    point.x = QDateTime::currentMSecsSinceEpoch() / 1000;
    for (const auto &[id, data] : data.toStdMap())
    {
        if (m_properties.contains(id))
            point.y[id] = data;
    }

    dataCollection.push_back(point);

    while (dataCollection.length() > VIEWABLE)
    {
        dataCollection.removeFirst();
    }
    replot();
}

// use binary prefix standards from IEC 60027-2
// see http://en.wikipedia.org/wiki/Kilobyte
enum SizeUnit : int
{
    Byte,  // 1000^0,
    KByte, // 1000^1,
    MByte, // 1000^2,
    GByte, // 1000^3,
    TByte, // 1000^4,
    PByte, // 1000^5,
    EByte  // 1000^6
};

QString unitString(const SizeUnit unit, const bool isSpeed)
{
    const static QStringList units{ "B", "KB", "MB", "GB", "TB", "PB", "EB" };
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
        case SizeUnit::KByte:
        case SizeUnit::MByte: return 1;
        case SizeUnit::GByte: return 2;
        default: return 3;
    }
}
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

    auto calculatedUnit = SizeUnit::Byte;

    while (value > 1000)
    {
        value /= 1000;
        calculatedUnit = static_cast<SizeUnit>(static_cast<int>(calculatedUnit) + 1);
    }

    if (value > 100.0)
    {
        int roundedValue = static_cast<int>(value / 40) * 40;
        while (roundedValue < value)
            roundedValue += 40;
        return { static_cast<double>(roundedValue), calculatedUnit };
    }

    if (value > 10.0)
    {
        int roundedValue = static_cast<int>(value / 4) * 4;
        while (roundedValue < value)
            roundedValue += 4;
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

void SpeedWidget::UpdateSpeedPlotSettings()
{
    auto &Graph = GlobalConfig.uiConfig.graphConfig;

    const auto apply_penconfig = [&](StatisticsType x, QvPair<QvGraphPenConfig> y) {
        if (!Graph.colorConfig.contains(x))
            Graph.colorConfig[x] = y;
    };

    apply_penconfig(API_INBOUND, Qv2rayConfig_Graph::DefaultPen);
    apply_penconfig(API_OUTBOUND_PROXY, Graph.colorConfig[API_INBOUND]);
    apply_penconfig(API_OUTBOUND_DIRECT, Qv2rayConfig_Graph::DirectPen);

    const auto getPen = [](const QvGraphPenConfig &conf) {
        QPen p{ { conf.R, conf.G, conf.B } };
        p.setStyle(conf.style);
        p.setWidthF(conf.width);
        return p;
    };

    m_properties.clear();
    if (Graph.useOutboundStats)
    {
        m_properties[OUTBOUND_PROXY_UP] = { tr("Proxy") + " ↑", getPen(Graph.colorConfig[API_OUTBOUND_PROXY].value1) };
        m_properties[OUTBOUND_PROXY_DOWN] = { tr("Proxy") + " ↓", getPen(Graph.colorConfig[API_OUTBOUND_PROXY].value2) };
        if (Graph.hasDirectStats)
        {
            m_properties[OUTBOUND_DIRECT_UP] = { tr("Direct") + " ↑", getPen(Graph.colorConfig[API_OUTBOUND_DIRECT].value1) };
            m_properties[OUTBOUND_DIRECT_DOWN] = { tr("Direct") + " ↓", getPen(Graph.colorConfig[API_OUTBOUND_DIRECT].value2) };
        }
    }
    else
    {
        m_properties[INBOUND_UP] = { tr("Total") + " ↑", getPen(Graph.colorConfig[API_INBOUND].value1) };
        m_properties[INBOUND_DOWN] = { tr("Total") + " ↓", getPen(Graph.colorConfig[API_INBOUND].value2) };
    }
}

void SpeedWidget::Clear()
{
    dataCollection.clear();
    m_properties.clear();
    UpdateSpeedPlotSettings();
    replot();
}
void SpeedWidget::replot()
{
    viewport()->update();
}

quint64 SpeedWidget::maxYValue()
{
    quint64 maxYValue = 0;

    for (int id = 0; id < NB_GRAPHS; ++id)
        for (int i = dataCollection.size() - 1, j = 0; (i >= 0) && (j <= VIEWABLE); --i, ++j)
            if (dataCollection[i].y[id] > maxYValue)
                maxYValue = dataCollection[i].y[id];

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
    const double yMultiplier = (niceScale.arg == 0.0) ? 0.0 : (static_cast<double>(rect.height()) / niceScale.sizeInBytes());
    const double xTickSize = static_cast<double>(rect.width()) / VIEWABLE;

    for (const auto &id : m_properties.keys())
    {
        QVector<QPoint> points;

        for (int i = static_cast<int>(dataCollection.size()) - 1, j = 0; (i >= 0) && (j <= VIEWABLE); --i, ++j)
        {
            const int newX = rect.right() - j * xTickSize;
            const int newY = rect.bottom() - dataCollection[i].y[id] * yMultiplier;
            points.push_back({ newX, newY });
        }

        painter.setPen(m_properties[id].pen);
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
