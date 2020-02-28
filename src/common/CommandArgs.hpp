#pragma once

#include <QCommandLineParser>

namespace Qv2ray::common
{
    enum CommandLineParseResult
    {
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
} // namespace Qv2ray::common

using namespace Qv2ray::common;
