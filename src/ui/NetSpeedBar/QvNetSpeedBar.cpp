#include <QThread>
#include "w_MainWindow.hpp"

#include "QvNetSpeedPlugin.hpp"
#include "QvUtils.hpp"
namespace Qv2ray
{
    namespace Components
    {
        namespace NetSpeedPlugin
        {
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
            void StartProcessingPlugins()
            {
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
                auto instance = MainWindow::mwInstance;

                if (instance == nullptr || instance->vinstance == nullptr) {
                    LOG(MODULE_PLUGIN, "MainWindow != nullptr Assertion failed!")
                    return "{}";
                }

                auto vinstance = instance->vinstance;
                //
                auto req = pchRequest.trimmed();
                config = GetGlobalConfig();
                QString reply = "{}";

                if (req == "START") {
                    emit instance->Connect();
                } else if (req == "STOP") {
                    emit instance->DisConnect();
                } else if (req == "RESTART") {
                    emit instance->ReConnect();
                }

                auto BarConfig = config.toolBarConfig;

                for (size_t i = 0; i < BarConfig.Pages.size(); i++) {
                    for (size_t j = 0; j < BarConfig.Pages[i].Lines.size(); j++) {
#define CL BarConfig.Pages[i].Lines[j]

                        switch (CL.ContentType) {
                            case 0: {
                                // Custom Text
                                // We do nothing...
                                break;
                            }

                            case 101: {
                                // Current Time
                                CL.Message = QTime().currentTime().toString("hh:mm:ss");
                                break;
                            }

                            case 102: {
                                // Current Date
                                CL.Message = QDate().currentDate().toString("yyyy-MM-dd");
                                break;
                            }

                            case 103: {
                                // Current Qv2ray Version
                                CL.Message = QV2RAY_VERSION_STRING;
                                break;
                            }

                            case 104: {
                                // Current Connection Name
                                CL.Message = instance->CurrentConnectionName;
                                break;
                            }

                            case 105: {
                                // Current Connection Status
                                switch (instance->vinstance->ConnectionStatus) {
                                    case STARTED: {
                                        CL.Message = QObject::tr("Connected");
                                        break;
                                    }

                                    case STOPPED: {
                                        CL.Message = QObject::tr("Disconnected");
                                        break;
                                    }

                                    case STARTING: {
                                        CL.Message = QObject::tr("Connecting");
                                        break;
                                    }
                                }

                                break;
                            }

                            case 201: {
                                // Total upload speed;
                                CL.Message = FormatBytes(vinstance->getAllSpeedUp()) + "/s";
                                break;
                            }

                            case 202: {
                                // Total download speed;
                                CL.Message = FormatBytes(vinstance->getAllSpeedDown()) + "/s";
                                break;
                            }

                            case 203: {
                                // Upload speed for tag
                                CL.Message = FormatBytes(vinstance->getTagSpeedUp(CL.Message)) + "/s";
                                break;
                            }

                            case 204: {
                                // Download speed for tag
                                CL.Message = FormatBytes(vinstance->getTagSpeedDown(CL.Message)) + "/s";
                                break;
                            }

                            case 301: {
                                // Total Upload
                                CL.Message = FormatBytes(vinstance->getAllDataUp());
                                break;
                            }

                            case 302: {
                                // Total download
                                CL.Message = FormatBytes(vinstance->getAllDataDown());
                                break;
                            }

                            case 303: {
                                // Upload for tag
                                CL.Message = FormatBytes(vinstance->getTagDataUp(CL.Message));
                                break;
                            }

                            case 304: {
                                // Download for tag
                                CL.Message = FormatBytes(vinstance->getTagDataDown(CL.Message));
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
