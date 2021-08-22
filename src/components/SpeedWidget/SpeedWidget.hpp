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

#include <QGraphicsView>
#include <QMap>
#include <QPen>

class SpeedWidget : public QGraphicsView
{
    Q_OBJECT
  public:
    enum GraphType
    {
        INBOUND_UP,
        INBOUND_DOWN,
        OUTBOUND_PROXY_UP,
        OUTBOUND_PROXY_DOWN,
        OUTBOUND_DIRECT_UP,
        OUTBOUND_DIRECT_DOWN,
        OUTBOUND_BLOCK_UP,
        OUTBOUND_BLOCK_DOWN,
        NB_GRAPHS,
    };
    struct PointData
    {
        qint64 x;
        quint64 y[NB_GRAPHS];
        PointData()
        {
            for (auto i = 0; i < NB_GRAPHS; i++)
                y[i] = 0;
        }
    };

    explicit SpeedWidget(QWidget *parent = nullptr);
    void UpdateSpeedPlotSettings();
    void AddPointData(QMap<SpeedWidget::GraphType, long> data);
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
    QList<PointData> dataCollection;

    QMap<GraphType, GraphProperties> m_properties;
};
