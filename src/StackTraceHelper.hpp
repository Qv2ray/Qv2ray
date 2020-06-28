#pragma once

#include <QString>
#ifdef Q_OS_LINUX
    #include "backward.hpp"
#endif

namespace Qv2ray
{
    class StackTraceHelper
    {
      public:
        static QString GetStackTrace()
        {
#ifdef Q_OS_LINUX
            return GetStackTraceImpl_Linux();
#elif defined(Q_OS_WIN)
            return GetStackTraceImpl_Windows();
#endif
        }

      private:
#ifdef Q_OS_LINUX
        static QString GetStackTraceImpl_Linux()
        {
            using namespace backward;
            StackTrace st;
            st.load_here();
            QString msg;
            TraceResolver tr;
            tr.load_stacktrace(st);
            //
            Printer p;
            std::stringstream o;
            p.print(st, o);
            msg += QString::fromStdString(o.str());
            msg += "\r\n";
            msg += "====================================== END OF FULL STACKTRACE ======================================";
            msg += "\r\n";
            for (size_t i = 0; i < st.size(); ++i)
            {
                ResolvedTrace trace = tr.resolve(st[i]);
                msg += QString("# %1 %2 [%3]\r\n")
                           .arg(trace.object_filename.c_str())
                           .arg(trace.object_function.c_str())
                           .arg(reinterpret_cast<size_t>(trace.addr));
            }
            return msg;
        }
#endif
#ifdef Q_OS_WIN
        static QString GetStackTraceImpl_Windows()
        {
            void *stack[1024];
            HANDLE process = GetCurrentProcess();
            SymInitialize(process, NULL, TRUE);
            WORD numberOfFrames = CaptureStackBackTrace(0, 1024, stack, NULL);
            SYMBOL_INFO *symbol = (SYMBOL_INFO *) malloc(sizeof(SYMBOL_INFO) + (512 - 1) * sizeof(TCHAR));
            symbol->MaxNameLen = 512;
            symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
            DWORD displacement;
            IMAGEHLP_LINE64 *line = (IMAGEHLP_LINE64 *) malloc(sizeof(IMAGEHLP_LINE64));
            line->SizeOfStruct = sizeof(IMAGEHLP_LINE64);
            for (int i = 0; i < numberOfFrames; i++)
            {
                DWORD64 address = (DWORD64)(stack[i]);
                SymFromAddr(process, address, NULL, symbol);
                if (SymGetLineFromAddr64(process, address, &displacement, line))
                {
                    printf("\tat %s in %s: line: %lu: address: 0x%0X\n", symbol->Name, line->FileName, line->LineNumber, symbol->Address);
                }
                else
                {
                    printf("\tSymGetLineFromAddr64 returned error code %lu.\n", GetLastError());
                    printf("\tat %s, address 0x%0X.\n", symbol->Name, symbol->Address);
                }
            }
            return 0;
        }
#endif
    };
} // namespace Qv2ray
