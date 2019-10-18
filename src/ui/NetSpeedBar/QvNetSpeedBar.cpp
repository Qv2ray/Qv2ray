#include "QvNetSpeedPlugin.h"
#include "QvUtils.h"

#include "w_MainWindow.h"
namespace Qv2ray
{
    namespace Utils
    {
        namespace NetSpeedPlugin
        {
            static MainWindow *mainWindow;
            static Qv2rayConfig config;
            void StartProcessingPlugins(QWidget *_mainWindow)
            {
                mainWindow = static_cast<MainWindow *>(_mainWindow);
                config = GetGlobalConfig();
#ifdef _WIN32
                _win::StartNamedPipeThread();
#endif
            }
            QString GetAnswerToRequest(QString pchRequest)
            {
                QString reply = "{}";

                if (pchRequest == "START") {
                    emit mainWindow->Connect();
                } else if (pchRequest == "STOP") {
                    emit mainWindow->DisConnect();
                } else if (pchRequest == "RESTART") {
                    emit mainWindow->ReConnect();
                } else {
                    //config.speedBarConfig;
                    auto conf = QvNetSpeedBarConfig();
                    QvBarLine line;
                    line.Family = "宋体";
                    line.Italic = true;
                    line.Message = "中文！！！yingwen!!!" + QDateTime().toString(Qt::TextDate).toStdString();
                    QvBarLine line2;
                    line2.Color = 0x00FF00FF;
                    line2.Family = "Consolas";
                    line2.Message = "中文！！！yingwen!!!" + QDateTime().toString(Qt::TextDate).toStdString();
                    QvBarPage page;
                    page.Lines.push_back(line);
                    page.Lines.push_back(line2);
                    page.OffsetYpx = 0;
                    conf.Pages.push_back(page);
                    reply = StructToJsonString(conf);
                    //
                }

                return reply;
            }
        }
    }
}
