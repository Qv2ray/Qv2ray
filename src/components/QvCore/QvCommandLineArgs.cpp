#include "QvCommandLineArgs.hpp"
#include "Qv2rayBase.hpp"
#include <QCommandLineParser>


namespace Qv2ray
{
    namespace CommandArgOperations
    {
        // Instantiation
        QvStartupOptions StartupOption = QvStartupOptions{};

        QvCommandArgParser::QvCommandArgParser() : QObject(),
            noAPIOption("FAKE"), runAsRootOption("FAKE"), helpOption("FAKE"), versionOption("FAKE")
        {
            parser.setApplicationDescription(QObject::tr("Qv2ray - A cross-platform Qt frontend for V2ray."));
            parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
            //
            noAPIOption = QCommandLineOption("noAPI", QObject::tr("Disable gRPC API subsystems."));
            runAsRootOption = QCommandLineOption("I-just-wanna-run-with-root", QObject::tr("Explicitly run Qv2ray as root."));
            parser.addOption(noAPIOption);
            parser.addOption(runAsRootOption);
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
                StartupOption.noAPI = true;
            }

            if (parser.isSet(runAsRootOption)) {
                StartupOption.forceRunAsRootUser = true;
            }

            return CommandLineOk;
        }
    }
}
