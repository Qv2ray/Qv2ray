#pragma once

#include <QAndroidIntent>
#include <QAndroidService>

class QvVPNService : QAndroidService
{
  public:
    QvVPNService(int &argc, char *argv[]);
    QAndroidBinder *onBind(const QAndroidIntent &intent);
};
