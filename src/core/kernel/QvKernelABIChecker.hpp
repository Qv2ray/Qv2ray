#pragma once

#include "base/Qv2rayFeatures.hpp"

#if QV2RAY_FEATURE(kernel_check_abi)

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
            ABI_ELF_AARCH64,
            ABI_ELF_ARM,
            ABI_ELF_OTHER,
            ABI_TRUSTED,
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
#elif defined(Q_OS_LINUX) && defined(Q_PROCESSOR_ARM_64)
            QvKernelABIType::ABI_ELF_AARCH64;
#elif defined(Q_OS_LINUX) && defined(Q_PROCESSOR_ARM_V7)
            QvKernelABIType::ABI_ELF_ARM;
#else
            QvKernelABIType::ABI_TRUSTED;
#define QV2RAY_TRUSTED_ABI
#endif

        std::pair<std::optional<QvKernelABIType>, std::optional<QString>> deduceKernelABI(const QString &pathCoreExecutable);
        QvKernelABICompatibility checkCompatibility(QvKernelABIType hostType, QvKernelABIType targetType);
        QString abiToString(QvKernelABIType abi);
    } // namespace abi
} // namespace Qv2ray::core::kernel

#endif
