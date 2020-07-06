#pragma once

#include <QString>
#ifdef Q_OS_UNIX
    #include "backward.hpp"

    #include <vector>
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
        static QString GetStackTraceImpl_Unix();
#elif defined(Q_OS_WIN)
        static QString GetStackTraceImpl_Windows();
#endif
    };
} // namespace Qv2ray
