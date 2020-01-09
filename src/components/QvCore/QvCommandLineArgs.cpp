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
            noAPIOption("FAKE"), helpOption("FAKE"), versionOption("FAKE")
        {
            parser.setApplicationDescription(QObject::tr("Qv2ray - A cross-platform Qt frontend for V2ray."));
            parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
            //
            noAPIOption = QCommandLineOption("noAPI", QObject::tr("Disable gRPC API subsystems."));
            parser.addOption(noAPIOption);
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

            return CommandLineOk;
        }
    }
}
