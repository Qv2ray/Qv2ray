#include "components/plugins/toolbar/QvToolbar.hpp"

#include "common/QvHelpers.hpp"
#include "core/handler/ConfigHandler.hpp"

#include <QThread>

namespace Qv2ray::components::plugins
{
    namespace Toolbar
    {
        void StopProcessingPlugins()
        {
            //#ifdef Q_OS_LINUX
            //            //_linux::StopMessageQThread();
            //#endif
            //#ifdef Q_OS_WIN
            //            //_win::KillNamedPipeThread();
            //#endif
        }

        /// Public Function - CALL ONLY ONCE -
        /// To start processing plugins' command.
        void StartProcessingPlugins()
        {
            //#ifdef Q_OS_LINUX
            //        //_linux::StartMessageQThread();
            //#endif
            //#ifdef Q_OS_WIN
            //        //_win::StartNamedPipeThread();
            //#endif
        }
        QString GetAnswerToRequest(const QString &)
        {
            //            auto req = pchRequest.trimmed();
            //            QString reply = "{}";

            //            if (req == "START")
            //            {
            //                emit ConnectionManager->RestartConnection();
            //            }
            //            else if (req == "STOP")
            //            {
            //                emit ConnectionManager->StopConnection();
            //            }
            //            else if (req == "RESTART")
            //            {
            //                emit ConnectionManager->RestartConnection();
            //            }

            //            auto BarConfig = GlobalConfig.toolBarConfig;

            //            for (auto i = 0; i < BarConfig.Pages.size(); i++)
            //            {
            //                for (auto j = 0; j < BarConfig.Pages[i].Lines.size(); j++)
            //                {
            //#define CL BarConfig.Pages[i].Lines[j]

            //                    switch (CL.ContentType)
            //                    {
            //                        case 0:
            //                        {
            //                            // Custom Text
            //                            // We do nothing...
            //                            break;
            //                        }

            //                        case 101:
            //                        {
            //                            // Current Time
            //                            CL.Message = QTime().currentTime().toString("hh:mm:ss");
            //                            break;
            //                        }

            //                        case 102:
            //                        {
            //                            // Current Date
            //                            CL.Message = QDate().currentDate().toString("yyyy-MM-dd");
            //                            break;
            //                        }

            //                        case 103:
            //                        {
            //                            // Current Qv2ray Version
            //                            CL.Message = QV2RAY_VERSION_STRING;
            //                            break;
            //                        }

            //                        case 104:
            //                        {
            //                            // Current Connection Name
            //                            CL.Message = GetDisplayName(KernelInstance->CurrentConnection());
            //                            break;
            //                        }

            //                        case 105:
            //                        {
            //                            // Current Connection Status
            //                            CL.Message = KernelInstance->CurrentConnection() == NullConnectionId ? QObject::tr("Not connected") :
            //                                                                                                   QObject::tr("Connected");
            //                            break;
            //                        }

            //                            // case 201:
            //                            //{
            //                            //    // Total upload speed;
            //                            //    CL.Message = FormatBytes(get<0>(GetConnectionUsageAmount())) + "/s";
            //                            //    break;
            //                            //}
            //                            //
            //                            // case 202:
            //                            //{
            //                            //    // Total download speed;
            //                            //    CL.Message = FormatBytes(vinstance->getAllSpeedDown()) + "/s";
            //                            //    break;
            //                            //}
            //                            //
            //                            // case 203:
            //                            //{
            //                            //    // Upload speed for tag
            //                            //    CL.Message = FormatBytes(vinstance->getTagSpeedUp(CL.Message)) + "/s";
            //                            //    break;
            //                            //}
            //                            //
            //                            // case 204:
            //                            //{
            //                            //    // Download speed for tag
            //                            //    CL.Message = FormatBytes(vinstance->getTagSpeedDown(CL.Message)) + "/s";
            //                            //    break;
            //                            //}

            //                        case 301:
            //                        {
            //                            // Total Upload
            //                            CL.Message = FormatBytes(get<0>(GetConnectionUsageAmount(KernelInstance->CurrentConnection())));
            //                            break;
            //                        }

            //                        case 302:
            //                        {
            //                            // Total download
            //                            CL.Message = FormatBytes(get<1>(GetConnectionUsageAmount(KernelInstance->CurrentConnection())));
            //                            break;
            //                        }

            //                            // case 303:
            //                            //{
            //                            //    // Upload for tag
            //                            //    CL.Message = FormatBytes(vinstance->getTagDataUp(CL.Message));
            //                            //    break;
            //                            //}
            //                            //
            //                            // case 304:
            //                            //{
            //                            //    // Download for tag
            //                            //    CL.Message = FormatBytes(vinstance->getTagDataDown(CL.Message));
            //                            //    break;
            //                            //}

            //                        case 305:
            //                        {
            //                            // Connection latency
            //                            CL.Message = QSTRN(GetConnectionLatency(KernelInstance->CurrentConnection())) + " ms";
            //                            break;
            //                        }
            //                        default:
            //                        {
            //                            CL.Message = "Not Implemented";
            //                            break;
            //                        }
            //                    }
            //                }
            //            }
            //#undef CL
            //          reply = JsonToString(BarConfig.toJson());
            return "Stopped Supporting";
        }
    } // namespace Toolbar
} // namespace Qv2ray::components::plugins
