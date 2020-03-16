#include "CommandArgs.hpp"

#include "base/Qv2rayBase.hpp"

namespace Qv2ray::common
{
    QvCommandArgParser::QvCommandArgParser()
        : QObject(), noAPIOption("noAPI", QObject::tr("Disable gRPC API subsystems.")),                 //
          runAsRootOption("I-just-wanna-run-with-root", QObject::tr("Explicitly run Qv2ray as root.")), //
          debugOption("debug", QObject::tr("Enable Debug Output")),                                     //
          hiDpiOption("hiDPI", QObject::tr("Enable HiDPI support for Qt")),                             //
          forceHiDpiOption("hiDPI", QObject::tr("Force enable HiDPI support for Qt")),                  //
          withToolbarOption("withToolbarPlugin", QObject::tr("Enable Qv2ray network toolbar plugin")),  //
          //
          helpOption("FAKE"), versionOption("FAKE")
    {
        parser.setApplicationDescription(QObject::tr("Qv2ray - A cross-platform Qt frontend for V2ray."));
        parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
        //
        parser.addOption(noAPIOption);
        parser.addOption(runAsRootOption);
        parser.addOption(debugOption);
        parser.addOption(hiDpiOption);
        parser.addOption(withToolbarOption);
        helpOption = parser.addHelpOption();
        versionOption = parser.addVersionOption();
    }

    CommandLineParseResult QvCommandArgParser::ParseCommandLine(QString *errorMessage)
    {
        if (!parser.parse(QCoreApplication::arguments()))
        {
            *errorMessage = parser.errorText();
            return CommandLineError;
        }

        if (parser.isSet(versionOption))
            return CommandLineVersionRequested;

        if (parser.isSet(helpOption))
            return CommandLineHelpRequested;

        if (parser.isSet(noAPIOption))
        {
            DEBUG(MODULE_INIT, "noAPIOption is set.")
            StartupOption.noAPI = true;
        }

        if (parser.isSet(runAsRootOption))
        {
            DEBUG(MODULE_INIT, "runAsRootOption is set.")
            StartupOption.forceRunAsRootUser = true;
        }

        if (parser.isSet(debugOption))
        {
            DEBUG(MODULE_INIT, "debugOption is set.")
            StartupOption.debugLog = true;
        }

        if (parser.isSet(hiDpiOption))
        {
            DEBUG(MODULE_INIT, "hiDPI is set.")
            StartupOption.hiDPI = true;
        }

        if (parser.isSet(forceHiDpiOption))
        {
            DEBUG(MODULE_INIT, "forceHiDPI is set.")
            StartupOption.forceHiDPI = true;
        }

        if (parser.isSet(withToolbarOption))
        {
            DEBUG(MODULE_INIT, "withToolbarOption is set.")
            StartupOption.enableToolbarPlguin = true;
        }

        return CommandLineOk;
    }

} // namespace Qv2ray::common
