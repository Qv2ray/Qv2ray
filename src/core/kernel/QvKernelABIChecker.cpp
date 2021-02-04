#include "QvKernelABIChecker.hpp"

#include <iostream>

#if QV2RAY_FEATURE(kernel_check_abi)

namespace Qv2ray::core::kernel::abi
{
    QvKernelABICompatibility checkCompatibility(QvKernelABIType hostType, QvKernelABIType targetType)
    {
#ifndef QV2RAY_TRUSTED_ABI
        switch (hostType)
        {
            case ABI_WIN32:
            case ABI_MACH_O:
            case ABI_ELF_AARCH64:
            case ABI_ELF_ARM:
            case ABI_ELF_X86: return targetType == hostType ? ABI_PERFECT : ABI_NOPE;
            case ABI_ELF_X86_64: return targetType == hostType ? ABI_PERFECT : targetType == ABI_ELF_X86 ? ABI_MAYBE : ABI_NOPE;
            case ABI_ELF_OTHER: return targetType == hostType ? ABI_PERFECT : ABI_MAYBE;
            case ABI_TRUSTED: return ABI_PERFECT;
            default: return ABI_MAYBE;
        }
#else
        return ABI_PERFECT;
#endif
    }

    std::pair<std::optional<QvKernelABIType>, std::optional<QString>> deduceKernelABI(const QString &pathCoreExecutable)
    {
#ifdef QV2RAY_TRUSTED_ABI
        return { QvKernelABIType::ABI_TRUSTED, std::nullopt };
#else
        QFile file(pathCoreExecutable);
        if (!file.exists())
            return { std::nullopt, QObject::tr("core executable file %1 does not exist").arg(pathCoreExecutable) };

        if (!file.open(QIODevice::ReadOnly))
            return { std::nullopt, QObject::tr("cannot open core executable file %1 in read-only mode").arg(pathCoreExecutable) };

        if (file.atEnd())
            return { std::nullopt, QObject::tr("core executable file %1 is an empty file").arg(pathCoreExecutable) };

        const QByteArray arr = file.read(0x100);
        if (arr.length() < 0x100)
            return { std::nullopt, QObject::tr("core executable file %1 is too short to be executed").arg(pathCoreExecutable) };

        if (quint32 elfMagicMaybe; QDataStream(arr) >> elfMagicMaybe, 0x7F454C46u == elfMagicMaybe)
        {
            quint16 elfInstruction;
            if (QDataStream stream(arr); stream.skipRawData(0x12), stream >> elfInstruction, elfInstruction == 0x3E00u)
                return { QvKernelABIType::ABI_ELF_X86_64, std::nullopt };
            else if (elfInstruction == 0x0300u)
                return { QvKernelABIType::ABI_ELF_X86, std::nullopt };
            else if (elfInstruction == 0xB700u)
                return { QvKernelABIType::ABI_ELF_AARCH64, std::nullopt };
            else if (elfInstruction == 0x2800u)
                return { QvKernelABIType::ABI_ELF_ARM, std::nullopt };
            else
                return { QvKernelABIType::ABI_ELF_OTHER, std::nullopt };
        }
        else if (quint16 dosMagicMaybe; QDataStream(arr) >> dosMagicMaybe, dosMagicMaybe == 0x4D5Au)
            return { QvKernelABIType::ABI_WIN32, std::nullopt };
        else if (quint32 machOMagicMaybe; QDataStream(arr) >> machOMagicMaybe, machOMagicMaybe == 0xCFFAEDFEu)
            return { QvKernelABIType::ABI_MACH_O, std::nullopt };
        else
            return { std::nullopt, QObject::tr("cannot deduce the type of core executable file %1").arg(pathCoreExecutable) };
#endif
    }

    QString abiToString(QvKernelABIType abi)
    {
        switch (abi)
        {
            case ABI_WIN32: return QObject::tr("Windows PE executable");
            case ABI_MACH_O: return QObject::tr("macOS Mach-O executable");
            case ABI_ELF_X86: return QObject::tr("ELF x86 executable");
            case ABI_ELF_X86_64: return QObject::tr("ELF amd64 executable");
            case ABI_ELF_AARCH64: return QObject::tr("ELF arm64 executable");
            case ABI_ELF_ARM: return QObject::tr("ELF arm executable");
            case ABI_ELF_OTHER: return QObject::tr("other ELF executable");
            case ABI_TRUSTED: return QObject::tr("trusted abi");
            default: return QObject::tr("unknown abi");
        }
    }
} // namespace Qv2ray::core::kernel::abi

#endif
