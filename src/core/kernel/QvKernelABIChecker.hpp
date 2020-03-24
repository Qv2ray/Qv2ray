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
            ABI_WIN32,
            ABI_MACH_O,
            ABI_ELF_X86,
            ABI_ELF_X86_64,
            ABI_ELF_OTHER,
        };

        enum QvKernelABICompatibility
        {
            ABI_NOPE,
            ABI_MAYBE,
            ABI_PERFECT,
        };

        inline constexpr auto COMPILED_ABI_TYPE =
#if defined(Q_OS_LINUX) && defined(Q_PROCESSOR_X86_64)
            QvKernelABIType::ABI_ELF_X86_64;
#elif defined(Q_OS_LINUX) && defined(Q_PROCESSOR_X86_32)
            QvKernelABIType::ABI_ELF_X86;
#elif defined(Q_OS_MACOS)
            QvKernelABIType::ABI_MACH_O;
#elif defined(Q_OS_WINDOWS)
            QvKernelABIType::ABI_WIN32;
#endif

        [[nodiscard]] std::pair<std::optional<QvKernelABIType>, std::optional<QString>> deduceKernelABI(const QString &pathCoreExecutable);
        [[nodiscard]] QvKernelABICompatibility checkCompatibility(QvKernelABIType hostType, QvKernelABIType targetType);
        [[nodiscard]] QString abiToString(QvKernelABIType abi);
    } // namespace abi
} // namespace Qv2ray::core::kernel
