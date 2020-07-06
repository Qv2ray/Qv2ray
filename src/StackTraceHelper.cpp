#include "StackTraceHelper.hpp"
namespace Qv2ray
{
#ifdef Q_OS_UNIX
    QString StackTraceHelper::GetStackTraceImpl_Unix()
    {
        backward::StackTrace st;
        backward::TraceResolver resolver;
        st.load_here();
        resolver.load_stacktrace(st);
        //
    #ifdef QV2RAY_PRINT_FULL_BACKTRACE
        backward::Printer p;
        std::stringstream o;
        p.print(st, o);
        QString msg;
        msg += QString::fromStdString(o.str());
        return msg;
    #else
        QString msg;
        for (size_t i = 0; i < st.size(); i++)
        {
            /*
             * It works because in real life, most signals are not around failures of the memory allocator (ie: malloc) itself.
             * As long as you can allocate memory, you are pretty ok.
             * Now, here is an example where backward will deadlock:
             *
             *     you buffer overflow inside your allocator data structure
             *     you ask your allocator to free or allocate something
             *     allocator acquires some locks
             *     allocator shit itself because its datastructures are corrupted
             *     signal is raised, backward-cpp kicks in
             *     while walking the stack, backward-cpp tries to allocate memory, calling your allocator...
             *     allocator tries to acquires some locks...
             *
             *     oops, deadlock.
             */
            auto trace = resolver.resolve(st[i]);
            QString sourceFile;
            if (!trace.source.filename.empty())
                sourceFile = QString("%0:[%1:%2]").arg(trace.source.filename.c_str()).arg(trace.source.line).arg(trace.source.col);
            else
                sourceFile = "[N/A]";

            // #Index: [ADDRESS] Function File Line:Col
            msg += QString("#%1: [%2] %3 in %4 --> %5\r\n")
                       .arg(i)
                       .arg(reinterpret_cast<size_t>(trace.addr))
                       .arg(trace.object_function.c_str())
                       .arg(trace.object_filename.c_str())
                       .arg(sourceFile);
        }
        return msg;
    #endif
    }
#endif

#ifdef Q_OS_WIN
    QString StackTraceHelper::GetStackTraceImpl_Windows()
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
                msg += QString("[%1]: %2 SymGetLineFromAddr64[%3]\r\n").arg(symbol->Address).arg(symbol->Name).arg(GetLastError());
            }
        }
        return msg;
    }
#endif
} // namespace Qv2ray
