#include "Qv2rayCliApplication.hpp"

Qv2rayCliApplication::Qv2rayCliApplication(int &argc, char *argv[]) : Qv2rayPlatformApplication(argc, argv)
{
}

Qv2raySetupStatus Qv2rayCliApplication::Initialize()
{
    return InitializeInternal();
}
Qv2rayExitCode Qv2rayCliApplication::RunQv2ray()
{
    RunInternal();
    return Qv2rayExitCode::QVEXIT_NORMAL;
}

void Qv2rayCliApplication::TerminateUI()
{
}
