#include <QThread>
#include "QvNetSpeedPlugin.hpp"
#include "QvUtils.hpp"

#include "w_MainWindow.hpp"
namespace Qv2ray
{
    namespace Utils
    {
        namespace NetSpeedPlugin
        {
            static MainWindow *mainWindow;
            static Qv2rayConfig config;
            void StopProcessingPlugins()
            {
#ifdef Q_OS_LINUX
                _linux::StopMessageQThread();
#endif
#ifdef Q_OS_WIN
                _win::KillNamedPipeThread();
#endif
            }

            /// Public Function - CALL ONLY ONCE -
            /// To start processing plugins' command.
            void StartProcessingPlugins(QWidget *_mainWindow)
            {
                mainWindow = static_cast<MainWindow *>(_mainWindow);
                config = GetGlobalConfig();
#ifdef Q_OS_LINUX
                _linux::StartMessageQThread();
#endif
#ifdef Q_OS_WIN
                _win::StartNamedPipeThread();
#endif
            }
            QString GetAnswerToRequest(const QString &pchRequest)
            {
                auto req = pchRequest.trimmed();
                config = GetGlobalConfig();
                QString reply = "{}";

                if (req == "START") {
                    emit mainWindow->Connect();
                } else if (req == "STOP") {
                    emit mainWindow->DisConnect();
                } else if (req == "RESTART") {
                    emit mainWindow->ReConnect();
                }

                auto BarConfig = config.toolBarConfig;

                for (size_t i = 0; i < BarConfig.Pages.size(); i++) {
                    for (size_t j = 0; j < BarConfig.Pages[i].Lines.size(); j++) {
#define CL BarConfig.Pages[i].Lines[j]
#define STATS_ENABLE_CHECK if(!config.enableStats) { CL.Message = QObject::tr("Stats is not enabled").toStdString(); break;}

                        switch (CL.ContentType) {
                            case 0: {
                                // Custom Text
                                // We do nothing...
                                break;
                            }

                            case 101: {
                                // Current Time
                                CL.Message = QTime().currentTime().toString("hh:mm:ss").toStdString();
                                break;
                            }

                            case 102: {
                                // Current Date
                                CL.Message = QDate().currentDate().toString("yyyy-MM-dd").toStdString();
                                break;
                            }

                            case 103: {
                                // Current Qv2ray Version
                                CL.Message = QV2RAY_VERSION_STRING;
                                break;
                            }

                            case 104: {
                                // Current Connection Name
                                CL.Message = mainWindow->CurrentConnectionName.toStdString();
                                break;
                            }

                            case 105: {
                                // Current Connection Status
                                switch (mainWindow->vinstance->VCoreStatus) {
                                    case STARTED: {
                                        CL.Message = QObject::tr("Connected").toStdString();
                                        break;
                                    }

                                    case STOPPED: {
                                        CL.Message = QObject::tr("Disconnected").toStdString();
                                        break;
                                    }

                                    case STARTING: {
                                        CL.Message = QObject::tr("Connecting").toStdString();
                                        break;
                                    }
                                }

                                break;
                            }

                            case 201: {
                                // Total upload speed;
                                STATS_ENABLE_CHECK
                                CL.Message = (mainWindow->totalSpeedUp + "/s").toStdString();
                                break;
                            }

                            case 202: {
                                // Total download speed;
                                STATS_ENABLE_CHECK
                                CL.Message = (mainWindow->totalSpeedDown + "/s").toStdString();
                                break;
                            }

                            case 203: {
                                // Upload speed for tag
                                STATS_ENABLE_CHECK
                                auto data = mainWindow->vinstance->dataTransferSpeed[QSTRING(CL.Message) + "_up"];
                                CL.Message = FormatBytes(data).toStdString() + "/s";
                                break;
                            }

                            case 204: {
                                STATS_ENABLE_CHECK
                                // Download speed for tag
                                auto data = mainWindow->vinstance->dataTransferSpeed[QSTRING(CL.Message) + "_down"];
                                CL.Message = FormatBytes(data).toStdString() + "/s";
                                break;
                            }

                            case 301: {
                                // Total Upload
                                STATS_ENABLE_CHECK
                                CL.Message = (mainWindow->totalDataUp).toStdString();
                                break;
                            }

                            case 302: {
                                // Total download
                                STATS_ENABLE_CHECK
                                CL.Message = (mainWindow->totalDataDown).toStdString();
                                break;
                            }

                            case 303: {
                                // Upload for tag
                                STATS_ENABLE_CHECK
                                auto data = mainWindow->vinstance->totalDataTransfered[QSTRING(CL.Message) + "_up"];
                                CL.Message = FormatBytes(data).toStdString();
                                break;
                            }

                            case 304: {
                                // Download for tag
                                STATS_ENABLE_CHECK
                                auto data = mainWindow->vinstance->totalDataTransfered[QSTRING(CL.Message) + "_down"];
                                CL.Message = FormatBytes(data).toStdString();
                                break;
                            }

                            default: {
                                CL.Message = "Not Supported?";
                                break;
                            }
                        }
                    }
                }

                reply = StructToJsonString(BarConfig);
                return reply;
            }
        }
    }
}
