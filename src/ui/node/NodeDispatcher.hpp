#pragma once
#include "base/Qv2rayBase.hpp"

class NodeDispatcher : public QObject
{
    Q_OBJECT
  public:
    explicit NodeDispatcher(QObject *parent);
    ~NodeDispatcher();
};
