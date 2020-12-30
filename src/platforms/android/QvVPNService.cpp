#include "QvVPNService.hpp"

#include <QtAndroid>

QvVPNService::QvVPNService(int &argc, char *argv[]) : QAndroidService(argc, argv)
{
}

QAndroidBinder *QvVPNService::onBind(const QAndroidIntent &intent)
{
    return nullptr;
}
