#pragma once

#include <QDataStream>
#include <QFile>
#include <QString>
#include <QtGlobal>
#include <optional>

namespace Qv2ray::core::kernel
{
    namespace abi
    {
        enum QvKernelABIType
        {
            WIN32,
            MACH_O,
            ELF_X86,
            ELF_X86_64,
            ELF_OTHER,
        };

        enum QvKernelABICompatibility
        {
            NOPE,
            MAYBE,
            PERFECT,
        };

        inline constexpr auto COMPILED_ABI_TYPE =
#if defined(Q_OS_LINUX) && defined(Q_PROCESSOR_X86_64)
            QvKernelABIType::ELF_X86_64;
#elif defined(Q_OS_LINUX) && defined(Q_PROCESSOR_X86_32)
            QvKernelABIType::ELF_X86;
#elif defined(Q_OS_MACOS)
            QvKernelABIType::MACH_O;
#elif defined(Q_OS_WINDOWS)
            QvKernelABIType::WIN32;
#endif

        [[nodiscard]] std::pair<std::optional<QvKernelABIType>, std::optional<QString>> deduceKernelABI(const QString &pathCoreExecutable);
        [[nodiscard]] QvKernelABICompatibility inline checkCompatibility(QvKernelABIType hostType, QvKernelABIType targetType);
    } // namespace abi
} // namespace Qv2ray::core::kernel
