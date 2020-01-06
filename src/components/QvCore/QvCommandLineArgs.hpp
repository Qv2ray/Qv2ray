#ifndef QVCOMMANDLINEARGS_HPP
#define QVCOMMANDLINEARGS_HPP

#include "Qv2rayBase.hpp"

namespace Qv2ray
{
    namespace CommandArgOperations
    {
        struct QvStartupOptions {
            /// No API subsystem
            bool noAPI;
        };
        enum CommandLineParseResult {
            CommandLineOk,
            CommandLineError,
            CommandLineVersionRequested,
            CommandLineHelpRequested
        };
        //
        extern QvStartupOptions StartupOption;

        class QvCommandArgParser : public QObject
        {
                Q_OBJECT
            public:
                QvCommandArgParser();
                CommandLineParseResult ParseCommandLine(QString *errorMessage);
                const QCommandLineParser *Parser()
                {
                    return &parser;
                }

            private:
                QCommandLineParser parser;
                QCommandLineOption noAPIOption;
                QCommandLineOption helpOption;
                QCommandLineOption versionOption;
        };
    }
}

using namespace Qv2ray::CommandArgOperations;
#endif
