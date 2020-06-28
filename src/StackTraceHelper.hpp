#pragma once

#include <QString>
#ifdef Q_OS_UNIX
    #include "backward.hpp"
#endif
#ifdef Q_OS_WIN
    #include <Windows.h>
    //
    #include <DbgHelp.h>
#endif

namespace Qv2ray
{
    class StackTraceHelper
    {
      public:
        static QString GetStackTrace()
        {
#ifdef Q_OS_UNIX
            return GetStackTraceImpl_Unix();
#elif defined(Q_OS_WIN)
            return GetStackTraceImpl_Windows();
#endif
        }

      private:
#ifdef Q_OS_UNIX
        static QString GetStackTraceImpl_Unix()
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
            SymSetOptions(SYMOPT_LOAD_ANYTHING);
            WORD numberOfFrames = CaptureStackBackTrace(0, 1024, stack, NULL);
            SYMBOL_INFO *symbol = (SYMBOL_INFO *) malloc(sizeof(SYMBOL_INFO) + (512 - 1) * sizeof(TCHAR));
            symbol->MaxNameLen = 512;
            symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
            DWORD displacement;
            IMAGEHLP_LINE64 *line = (IMAGEHLP_LINE64 *) malloc(sizeof(IMAGEHLP_LINE64));
            line->SizeOfStruct = sizeof(IMAGEHLP_LINE64);
            //
            QString msg;
            //
            for (int i = 0; i < numberOfFrames; i++)
            {
                const auto address = (DWORD64) stack[i];
                SymFromAddr(process, address, NULL, symbol);
                if (SymGetLineFromAddr64(process, address, &displacement, line))
                {
                    msg += QString("[%1]: %2 (%3:%4)\r\n").arg(symbol->Address).arg(symbol->Name).arg(line->FileName).arg(line->LineNumber);
                }
                else
                {
                    msg += QString("[*]: %1 SymGetLineFromAddr64[%2]\r\n").arg(symbol->Address).arg(symbol->Name).arg(GetLastError());
                }
            }
            return msg;
        }
#endif
    };
} // namespace Qv2ray
