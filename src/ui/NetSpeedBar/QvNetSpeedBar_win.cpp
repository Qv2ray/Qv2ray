#include <QtCore>
#ifdef Q_OS_WIN
#include "QvNetSpeedPlugin.hpp"
#include "QvUtils.hpp"
#include <windows.h>
namespace Qv2ray
{
    namespace Components
    {
        namespace NetSpeedPlugin
        {
            namespace _win
            {
                // Private Headers
                constexpr int BUFSIZE = 10240;
                DWORD WINAPI NamedPipeMasterThread(LPVOID lpvParam);
                DWORD WINAPI InstanceThread(LPVOID);
                static LPVOID ThreadHandle;
                static bool isExiting = false;

                //
                void KillNamedPipeThread()
                {
                    isExiting = true;
                    TerminateThread(ThreadHandle, 0);
                }
                //
                void StartNamedPipeThread()
                {
                    auto hThread = CreateThread(nullptr, 0, NamedPipeMasterThread, nullptr, 0, nullptr);

                    if (hThread == nullptr) {
                        LOG(MODULE_PLUGIN, "CreateThread failed, GLE=" + to_string(GetLastError()))
                        return;
                    } else CloseHandle(hThread);
                }

                DWORD WINAPI NamedPipeMasterThread(LPVOID lpvParam)
                {
                    Q_UNUSED(lpvParam)
                    BOOL   fConnected = FALSE;
                    DWORD  dwThreadId = 0;
                    HANDLE hPipe = INVALID_HANDLE_VALUE;
                    auto lpszPipename = QString(QV2RAY_NETSPEED_PLUGIN_PIPE_NAME_WIN).toStdWString();

                    while (!isExiting) {
                        //printf("Pipe Server: Main thread awaiting client connection on %s\n", lpszPipename.c_str());
                        hPipe = CreateNamedPipe(lpszPipename.c_str(), PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, BUFSIZE, BUFSIZE, 0, nullptr);

                        if (hPipe == INVALID_HANDLE_VALUE) {
                            LOG(MODULE_PLUGIN, "CreateNamedPipe failed, GLE=" + to_string(GetLastError()))
                            return static_cast<DWORD>(-1);
                        }

                        fConnected = ConnectNamedPipe(hPipe, nullptr) ? true : (GetLastError() == ERROR_PIPE_CONNECTED);

                        if (fConnected) {
                            LOG(MODULE_PLUGIN, "Client connected, creating a processing thread")
                            ThreadHandle = CreateThread(nullptr, 0, InstanceThread, hPipe, 0, &dwThreadId);

                            if (ThreadHandle == nullptr) {
                                LOG(MODULE_PLUGIN, "CreateThread failed, GLE=" + to_string(GetLastError()))
                                return static_cast<DWORD>(-1);
                            } else CloseHandle(ThreadHandle);
                        } else CloseHandle(hPipe);
                    }

                    return 0;
                }

                DWORD WINAPI InstanceThread(LPVOID lpvParam)
                {
                    DWORD cbBytesRead = 0, cbReplyBytes = 0, cbWritten = 0;
                    BOOL fSuccess = false;
                    HANDLE hPipe = static_cast<HANDLE>(lpvParam);
                    TCHAR pchRequest[BUFSIZE] = { 0 };

                    while (!isExiting) {
                        fSuccess = ReadFile(hPipe, pchRequest, BUFSIZE * sizeof(TCHAR), &cbBytesRead, nullptr);

                        if (!fSuccess || cbBytesRead == 0) {
                            if (GetLastError() == ERROR_BROKEN_PIPE) {
                                LOG(MODULE_PLUGIN, "InstanceThread: client disconnected, GLE=" + to_string(GetLastError()))
                            } else {
                                LOG(MODULE_PLUGIN, "InstanceThread ReadFile failed, GLE=" + to_string(GetLastError()))
                            }

                            break;
                        }

                        auto req = QString::fromStdWString(pchRequest);
                        QString replyQString = "{}";

                        if (!isExiting) {
                            replyQString = GetAnswerToRequest(req);
                            //
                            // REPLY as std::string
                            std::string pchReply = replyQString.toUtf8().constData();
                            cbReplyBytes = static_cast<DWORD>(pchReply.length() + 1) * sizeof(CHAR);
                            //cbReplyBytes = static_cast<DWORD>(replyQString.length() + 1) * sizeof(TCHAR);
                            //
                            fSuccess = WriteFile(hPipe, pchReply.c_str(), cbReplyBytes, &cbWritten, nullptr);

                            if (!fSuccess || cbReplyBytes != cbWritten) {
                                LOG(MODULE_PLUGIN, "InstanceThread WriteFile failed, GLE=" + to_string(GetLastError()))
                                break;
                            }
                        }
                    }

                    FlushFileBuffers(hPipe);
                    DisconnectNamedPipe(hPipe);
                    CloseHandle(hPipe);
                    return 1;
                }
            }
        }
    }
}
#endif
