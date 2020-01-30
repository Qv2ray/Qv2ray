#pragma once
#include "base/Qv2rayBase.hpp"
namespace Qv2ray::common
{
    enum CommandLineParseResult {
        CommandLineOk,
        CommandLineError,
        CommandLineVersionRequested,
        CommandLineHelpRequested
    };
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
            QCommandLineOption runAsRootOption;
            QCommandLineOption debugOption;
            QCommandLineOption withToolbarOption;
            QCommandLineOption helpOption;
            QCommandLineOption versionOption;
    };
}

using namespace Qv2ray::common;
