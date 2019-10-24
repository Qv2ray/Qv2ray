#ifndef QVNETSPEEDBARJSON_H
#define QVNETSPEEDBARJSON_H

#include "x2struct/x2struct.hpp"
#include <QtGui>
//
// For Windows
#ifdef _WIN32
#include <windows.h>
#endif
//
#define QV2RAY_NETSPEED_PLUGIN_PIPE_NAME_LINUX "Qv2ray_NetSpeed_Widget_LocalSocket"
#define QV2RAY_NETSPEED_PLUGIN_PIPE_NAME_WIN "\\\\.\\pipe\\qv2ray_desktop_netspeed_toolbar_pipe"
//
namespace Qv2ray
{
    namespace Utils
    {
        namespace NetSpeedPlugin
        {
            /// NO NOT CHANGE THE ORDER
            static const QMap<int, QString> NetSpeedPluginMessages {
                { 0, QObject::tr("Custom Text")},
                // Current Status
                { 101, QObject::tr("Current Time") },
                { 102, QObject::tr("Current Date") },
                { 103, QObject::tr("Current Qv2ray Version") },
                { 104, QObject::tr("Current Connection Name") },
                { 105, QObject::tr("Current Connection Status") },
                // Speeds
                { 201, QObject::tr("Total Upload Speed") },
                { 202, QObject::tr("Total Download Speed") },
                { 203, QObject::tr("Upload Speed for Specific Tag") },
                { 204, QObject::tr("Download Speed for Specific Tag") },
                // Datas
                { 301, QObject::tr("Total Uploaded Data") },
                { 302, QObject::tr("Total Downloaded Data") },
                { 303, QObject::tr("Uploaded Data for Specific Tag") },
                { 304, QObject::tr("Downloaded Data for Specific Tag") }
            };
            void StartProcessingPlugins(QWidget *mainWindow);
            void StopProcessingPlugins();
#ifdef _WIN32
            namespace _win
            {
                void StartNamedPipeThread();
                void KillNamedPipeThread();
            }
#endif
#ifdef __linux__
            namespace _linux
            {
                // This function is called within a QThread
                // Actually it should the entrypoint of a thread.
                void StartMessageQThread();
                void StopMessageQThread();

            }
#endif

            struct QvBarLine {
                std::string     Family;
                bool            Bold;
                bool            Italic;
                int             ColorA;
                int             ColorR;
                int             ColorG;
                int             ColorB;
                int             ContentType;
                double          Size;
                std::string     Message;
                QvBarLine()
                    : Family("Consolas")
                    , Bold(true)
                    , Italic(false)
                    , ColorA(255), ColorR(255), ColorG(255), ColorB(255)
                    , ContentType(0)
                    , Size(9),
                      Message() { }
                XTOSTRUCT(O(Bold, Italic, ColorA, ColorR, ColorG, ColorB, Size, Family, Message, ContentType))
            };

            struct QvBarPage {
                int OffsetYpx;
                std::vector<QvBarLine> Lines;
                XTOSTRUCT(O(OffsetYpx, Lines))
                QvBarPage() : OffsetYpx(5) { }
            };

            struct QvNetSpeedBarConfig {
                std::vector<QvBarPage> Pages;
                XTOSTRUCT(O(Pages))
            };

            QString GetAnswerToRequest(const QString &pchRequest);

        }
    } // namespace Utils
} // namespace Qv2ray
//
//

//
//
using namespace Qv2ray::Utils::NetSpeedPlugin;
#endif // QVNETSPEEDBARJSON_H
