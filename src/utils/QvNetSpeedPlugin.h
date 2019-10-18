#ifndef QVNETSPEEDBARJSON_H
#define QVNETSPEEDBARJSON_H

#include "3rdparty/x2struct/x2struct.hpp"
#include <QtGui>
//
// For Windows
#ifdef _WIN32
#include <windows.h>
#endif
//
namespace Qv2ray
{
    namespace Utils
    {
        namespace NetSpeedPlugin
        {
            void StartProcessingPlugins(QWidget *mainWindow);
#ifdef _WIN32
            namespace _win
            {
                void StartNamedPipeThread();
            }
#endif

            struct QvBarLine {
                std::string  Family;
                bool         Bold;
                bool         Italic;
                // This idiot should be DWORD
                // --> Which is unsigned long
                // ----> Which x2struct does not support.
                long long    Color;
                double       Size;
                std::string        Message;
                QvBarLine()
                    : Family(u8"微软雅黑")
                    , Bold(true)
                    , Italic(false)
                    , Color(0x00FFFFFF)
                    , Size(8.5),
                      Message() { }
                XTOSTRUCT(O(Bold, Italic, Color, Size, Family, Message))
            };

            struct QvBarPage {
                int                    OffsetYpx;
                std::vector<QvBarLine> Lines;
                QvBarPage() : OffsetYpx(0) { }
                XTOSTRUCT(O(OffsetYpx, Lines))
            };

            struct QvNetSpeedBarConfig {
                std::vector<QvBarPage> Pages;
                XTOSTRUCT(O(Pages))
                QvNetSpeedBarConfig() { }
            };

            QString GetAnswerToRequest(QString pchRequest);

        }
    } // namespace Utils
} // namespace Qv2ray
//
//

//
//
using namespace Qv2ray::Utils::NetSpeedPlugin;
#endif // QVNETSPEEDBARJSON_H
