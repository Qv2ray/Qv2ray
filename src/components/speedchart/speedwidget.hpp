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
#pragma once

#include <QComboBox>
#include <QGraphicsView>
#include <QPen>
#include <QWidget>

class QHBoxLayout;
class QLabel;
class QMenu;
class QVBoxLayout;

class SpeedWidget : public QGraphicsView
{
    Q_OBJECT
  public:
    enum GraphID
    {
        INBOUND_UP /*           */ = 0b0000,
        INBOUND_DOWN /*         */ = 0b0001,
        OUTBOUND_PROXY_UP /*    */ = 0b0010,
        OUTBOUND_PROXY_DOWN /*  */ = 0b0011,
        OUTBOUND_DIRECT_UP /*   */ = 0b0100,
        OUTBOUND_DIRECT_DOWN /* */ = 0b0111,
        NB_GRAPHS
    };
    struct PointData
    {
        qint64 x;
        quint64 y[NB_GRAPHS];
    };

    explicit SpeedWidget(QWidget *parent = nullptr);
    void UpdateSpeedPlotSettings(bool isOutboundGraph, bool hasDirectGraph);
    void AddPointData(QMap<SpeedWidget::GraphID, long> data);
    void PushPoint(const PointData &point);
    void Clear();
    void replot();

  protected:
    void paintEvent(QPaintEvent *event) override;

  private:
    struct GraphProperties
    {
        GraphProperties(){};
        GraphProperties(const QString &name, const QPen &pen) : name(name), pen(pen){};
        QString name;
        QPen pen;
    };

    quint64 maxYValue();
    QList<PointData> m_datahalfMin;
    QList<PointData> *m_currentData;

    bool isOutbound;
    bool hasDirectLine;

    QMap<GraphID, GraphProperties> m_properties;
};
