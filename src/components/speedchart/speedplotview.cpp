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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * In addition, as a special exception, the copyright holders give permission to
 * link this program with the OpenSSL project's "OpenSSL" library (or with
 * modified versions of it that use the same license as the "OpenSSL" library),
 * and distribute the linked executables. You must obey the GNU General Public
 * License in all respects for all of the code used other than "OpenSSL".  If you
 * modify file(s), you may extend this exception to your version of the file(s),
 * but you are not obligated to do so. If you do not wish to do so, delete this
 * exception statement from your version.
 */

#include "speedplotview.hpp"

#include <QLocale>
#include <QPainter>
#include <QPen>
#include <list>

#include <QCoreApplication>

#define VIEWABLE 120

// use binary prefix standards from IEC 60027-2
// see http://en.wikipedia.org/wiki/Kilobyte
enum class SizeUnit {
    Byte,       // 1024^0,
    KibiByte,   // 1024^1,
    MebiByte,   // 1024^2,
    GibiByte,   // 1024^3,
    TebiByte,   // 1024^4,
    PebiByte,   // 1024^5,
    ExbiByte    // 1024^6,
    // int64 is used for sizes and thus the next units can not be handled
    // ZebiByte,   // 1024^7,
    // YobiByte,   // 1024^8
};

namespace
{
    const struct {
        const char *source;
        const char *comment;
    } units[] = {
        QT_TRANSLATE_NOOP3("misc", "B", "bytes"),
        QT_TRANSLATE_NOOP3("misc", "KiB", "kibibytes (1024 bytes)"),
        QT_TRANSLATE_NOOP3("misc", "MiB", "mebibytes (1024 kibibytes)"),
        QT_TRANSLATE_NOOP3("misc", "GiB", "gibibytes (1024 mibibytes)"),
        QT_TRANSLATE_NOOP3("misc", "TiB", "tebibytes (1024 gibibytes)"),
        QT_TRANSLATE_NOOP3("misc", "PiB", "pebibytes (1024 tebibytes)"),
        QT_TRANSLATE_NOOP3("misc", "EiB", "exbibytes (1024 pebibytes)")
    };
}

QString unitString(const SizeUnit unit, const bool isSpeed)
{
    const auto &unitString = units[static_cast<int>(unit)];
    QString ret = QCoreApplication::translate("misc", unitString.source, unitString.comment);

    if (isSpeed)
        ret += QCoreApplication::translate("misc", "/s", "per second");

    return ret;
}

int friendlyUnitPrecision(const SizeUnit unit)
{
    // friendlyUnit's number of digits after the decimal point
    switch (unit) {
        case SizeUnit::Byte:
            return 0;

        case SizeUnit::KibiByte:
        case SizeUnit::MebiByte:
            return 1;

        case SizeUnit::GibiByte:
            return 2;

        default:
            return 3;
    }
}

qlonglong sizeInBytes(qreal size, const SizeUnit unit)
{
    for (int i = 0; i < static_cast<int>(unit); ++i)
        size *= 1024;

    return size;
}

namespace
{
    // table of supposed nice steps for grid marks to get nice looking quarters of scale
    const double roundingTable[] = {1.2, 1.6, 2, 2.4, 2.8, 3.2, 4, 6, 8};

    struct SplittedValue {
        double arg;
        SizeUnit unit;
        qint64 sizeInBytes() const
        {
            return ::sizeInBytes(arg, unit);
        }
    };

    SplittedValue getRoundedYScale(double value)
    {
        if (value == 0.0) return {0, SizeUnit::Byte};

        if (value <= 12.0) return {12, SizeUnit::Byte};

        SizeUnit calculatedUnit = SizeUnit::Byte;

        while (value > 1024) {
            value /= 1024;
            calculatedUnit = static_cast<SizeUnit>(static_cast<int>(calculatedUnit) + 1);
        }

        if (value > 100.0) {
            int roundedValue = static_cast<int>(value / 40) * 40;

            while (roundedValue < value)
                roundedValue += 40;

            return {static_cast<double>(roundedValue), calculatedUnit};
        }

        if (value > 10.0) {
            int roundedValue = static_cast<int>(value / 4) * 4;

            while (roundedValue < value)
                roundedValue += 4;

            return {static_cast<double>(roundedValue), calculatedUnit};
        }

        for (const auto &roundedValue : roundingTable) {
            if (value <= roundedValue)
                return {roundedValue, calculatedUnit};
        }

        return {10.0, calculatedUnit};
    }

    QString formatLabel(const double argValue, const SizeUnit unit)
    {
        // check is there need for digits after decimal separator
        const int precision = (argValue < 10) ? friendlyUnitPrecision(unit) : 0;
        return QLocale::system().toString(argValue, 'f', precision)
               + QString::fromUtf8(" ")
               + unitString(unit, true);
    }
}

SpeedPlotView::SpeedPlotView(QWidget *parent)
    : QGraphicsView(parent)
    , m_currentData(&m_datahalfMin)
{
    QPen greenPen;
    greenPen.setWidthF(1.5);
    greenPen.setColor(QColor(134, 196, 63));
    QPen bluePen;
    bluePen.setWidthF(1.5);
    bluePen.setColor(QColor(50, 153, 255));
    m_properties[UP] = GraphProperties(tr("Total Upload"), bluePen);
    m_properties[DOWN] = GraphProperties(tr("Total Download"), greenPen);
}

void SpeedPlotView::Clear()
{
    m_datahalfMin.clear();
    replot();
}

void SpeedPlotView::pushPoint(const SpeedPlotView::PointData &point)
{
    m_datahalfMin.push_back(point);

    while (m_datahalfMin.length() > VIEWABLE) {
        m_datahalfMin.removeFirst();
    }
}

void SpeedPlotView::replot()
{
    viewport()->update();
}

QList<SpeedPlotView::PointData> &SpeedPlotView::getCurrentData()
{
    return *m_currentData;
}

quint64 SpeedPlotView::maxYValue()
{
    auto &queue = getCurrentData();
    quint64 maxYValue = 0;

    for (int id = UP; id < NB_GRAPHS; ++id) {
        // 30 is half min
        for (int i = queue.size() - 1, j = 0; (i >= 0) && (j <= VIEWABLE); --i, ++j) {
            if (queue[i].y[id] > maxYValue)
                maxYValue = queue[i].y[id];
        }
    }

    return maxYValue;
}

void SpeedPlotView::paintEvent(QPaintEvent *)
{
    QPainter painter(viewport());
    QRect fullRect = viewport()->rect();
    QRect rect = viewport()->rect();
    QFontMetrics fontMetrics = painter.fontMetrics();
    rect.adjust(4, 4, 0, -4); // Add padding
    const SplittedValue niceScale = getRoundedYScale(maxYValue());
    rect.adjust(0, fontMetrics.height(), 0, 0); // Add top padding for top speed text
    // draw Y axis speed labels
    const QVector<QString> speedLabels = {
        formatLabel(niceScale.arg, niceScale.unit),
        formatLabel((0.75 * niceScale.arg), niceScale.unit),
        formatLabel((0.50 * niceScale.arg), niceScale.unit),
        formatLabel((0.25 * niceScale.arg), niceScale.unit),
        formatLabel(0.0, niceScale.unit),
    };
    int yAxisWidth = 0;

    for (const QString &label : speedLabels)
        if (fontMetrics.horizontalAdvance(label) > yAxisWidth)
            yAxisWidth = fontMetrics.horizontalAdvance(label);

    int i = 0;

    for (const QString &label : speedLabels) {
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
    painter.setPen(gridPen);
    painter.drawLine(fullRect.left(), rect.top(), rect.right(), rect.top());
    painter.drawLine(fullRect.left(), rect.top() + 0.25 * rect.height(), rect.right(), rect.top() + 0.25 * rect.height());
    painter.drawLine(fullRect.left(), rect.top() + 0.50 * rect.height(), rect.right(), rect.top() + 0.50 * rect.height());
    painter.drawLine(fullRect.left(), rect.top() + 0.75 * rect.height(), rect.right(), rect.top() + 0.75 * rect.height());
    painter.drawLine(fullRect.left(), rect.bottom(), rect.right(), rect.bottom());
    const int TIME_AXIS_DIVISIONS = 6;

    for (int i = 0; i < TIME_AXIS_DIVISIONS; ++i) {
        const int x = rect.left() + (i * rect.width()) / TIME_AXIS_DIVISIONS;
        painter.drawLine(x, fullRect.top(), x, fullRect.bottom());
    }

    // Set antialiasing for graphs
    painter.setRenderHints(QPainter::Antialiasing);
    // draw graphs
    rect.adjust(3, 0, 0, 0); // Need, else graphs cross left gridline
    const double yMultiplier = (niceScale.arg == 0.0) ? 0.0 : (static_cast<double>(rect.height()) / niceScale.sizeInBytes());
    const double xTickSize = static_cast<double>(rect.width()) / VIEWABLE;
    auto &queue = getCurrentData();

    for (int id = UP; id < NB_GRAPHS; ++id) {
        QVector<QPoint> points;

        for (int i = static_cast<int>(queue.size()) - 1, j = 0; (i >= 0) && (j <= VIEWABLE); --i, ++j) {
            int newX = rect.right() - j * xTickSize;
            int newY = rect.bottom() - queue[i].y[id] * yMultiplier;
            points.push_back(QPoint(newX, newY));
        }

        painter.setPen(m_properties[static_cast<GraphID>(id)].pen);
        painter.drawPolyline(points.data(), points.size());
    }

    // draw legend
    QPoint legendTopLeft(rect.left() + 4, fullRect.top() + 4);
    double legendHeight = 0;
    int legendWidth = 0;

    for (const auto &property : m_properties) {
        if (fontMetrics.horizontalAdvance(property.name) > legendWidth)
            legendWidth = fontMetrics.horizontalAdvance(property.name);

        legendHeight += 1.5 * fontMetrics.height();
    }

    QRectF legendBackgroundRect(QPoint(legendTopLeft.x() - 4, legendTopLeft.y() - 4), QSizeF(legendWidth + 8, legendHeight + 8));
    QColor legendBackgroundColor = QWidget::palette().color(QWidget::backgroundRole());
    legendBackgroundColor.setAlpha(128);  // 50% transparent
    painter.fillRect(legendBackgroundRect, legendBackgroundColor);
    i = 0;

    for (const auto &property : m_properties) {
        int nameSize = fontMetrics.horizontalAdvance(property.name);
        double indent = 1.5 * (i++) * fontMetrics.height();
        painter.setPen(property.pen);
        painter.drawLine(legendTopLeft + QPointF(0, indent + fontMetrics.height()),
                         legendTopLeft + QPointF(nameSize, indent + fontMetrics.height()));
        painter.drawText(QRectF(legendTopLeft + QPointF(0, indent), QSizeF(2 * nameSize, fontMetrics.height())),
                         property.name, QTextOption(Qt::AlignVCenter));
    }
}

SpeedPlotView::GraphProperties::GraphProperties()
{
}

SpeedPlotView::GraphProperties::GraphProperties(const QString &name, const QPen &pen)
    : name(name)
    , pen(pen)
{
}
