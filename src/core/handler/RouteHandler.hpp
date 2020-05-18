#pragma once

#include <QObject>

class RouteHandler : public QObject
{
    Q_OBJECT
  public:
    explicit RouteHandler(QObject *parent = nullptr);

  signals:
};
