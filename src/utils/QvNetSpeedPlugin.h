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
                    : Family("微软雅黑")
                    , Bold(true)
                    , Italic(false)
                    , ColorA(0), ColorR(0), ColorG(0), ColorB(0)
                    , Size(8.5),
                      Message() { }
                XTOSTRUCT(O(Bold, Italic, ColorA, ColorR, ColorG, ColorB, Size, Family, Message))
            };

            struct QvBarPage {
                int OffsetYpx;
                std::vector<QvBarLine> Lines;
                XTOSTRUCT(O(OffsetYpx, Lines))
                QvBarPage() : OffsetYpx(0) { }
            };

            struct QvNetSpeedBarConfig {
                std::vector<QvBarPage> Pages;
                XTOSTRUCT(O(Pages))
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
