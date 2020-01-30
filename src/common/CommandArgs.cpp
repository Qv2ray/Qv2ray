#include "CommandArgs.hpp"
#include <QCommandLineParser>

namespace Qv2ray::common
{
    QvCommandArgParser::QvCommandArgParser() : QObject(),
        noAPIOption("noAPI", QObject::tr("Disable gRPC API subsystems.")),
        runAsRootOption("I-just-wanna-run-with-root", QObject::tr("Explicitly run Qv2ray as root.")),
        debugOption("debug", QObject::tr("Enable Debug Output")),
        withToolbarOption("withToolbarPlugin", QObject::tr("Enable Qv2ray network toolbar plugin")),
        //
        helpOption("FAKE"), versionOption("FAKE")
    {
        parser.setApplicationDescription(QObject::tr("Qv2ray - A cross-platform Qt frontend for V2ray."));
        parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
        //
        parser.addOption(noAPIOption);
        parser.addOption(runAsRootOption);
        parser.addOption(debugOption);
        parser.addOption(withToolbarOption);
        helpOption = parser.addHelpOption();
        versionOption = parser.addVersionOption();
    }

    CommandLineParseResult QvCommandArgParser::ParseCommandLine(QString *errorMessage)
    {
        if (!parser.parse(QCoreApplication::arguments())) {
            *errorMessage = parser.errorText();
            return CommandLineError;
        }

        if (parser.isSet(versionOption))
            return CommandLineVersionRequested;

        if (parser.isSet(helpOption))
            return CommandLineHelpRequested;

        if (parser.isSet(noAPIOption)) {
            DEBUG(MODULE_INIT, "noAPIOption is set.")
            StartupOption.noAPI = true;
        }

        if (parser.isSet(runAsRootOption)) {
            DEBUG(MODULE_INIT, "runAsRootOption is set.")
            StartupOption.forceRunAsRootUser = true;
        }

        if (parser.isSet(debugOption)) {
            DEBUG(MODULE_INIT, "debugOption is set.")
            StartupOption.debugLog = true;
        }

        if (parser.isSet(withToolbarOption)) {
            DEBUG(MODULE_INIT, "withToolbarOption is set.")
            StartupOption.enableToolbarPlguin = true;
        }

        return CommandLineOk;
    }

}
