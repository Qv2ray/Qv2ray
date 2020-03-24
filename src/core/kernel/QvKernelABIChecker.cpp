#include "QvKernelABIChecker.hpp"

#include <iostream>

namespace Qv2ray::core::kernel::abi
{
    [[nodiscard]] QvKernelABICompatibility inline checkCompatibility(QvKernelABIType hostType, QvKernelABIType targetType)
    {
        switch (hostType)
        {
            case ABI_WIN32: [[fallthrough]];
            case ABI_MACH_O: [[fallthrough]];
            case ABI_ELF_X86: return targetType == hostType ? ABI_PERFECT : ABI_NOPE;
            case ABI_ELF_X86_64: return targetType == hostType ? ABI_PERFECT : targetType == ABI_ELF_X86 ? ABI_MAYBE : ABI_NOPE;
            case ABI_ELF_OTHER: return targetType == hostType ? ABI_PERFECT : ABI_MAYBE;
            default: return ABI_MAYBE;
        }
    }

    [[nodiscard]] std::pair<std::optional<QvKernelABIType>, std::optional<QString>> deduceKernelABI(const QString &pathCoreExecutable)
    {
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
            if (QDataStream stream(arr); stream.skipRawData(0x12), stream >> elfInstruction, elfInstruction == 0x003Eu)
                return { QvKernelABIType::ABI_ELF_X86_64, std::nullopt };
            else if (elfInstruction == 0x0003u)
                return { QvKernelABIType::ABI_ELF_X86, std::nullopt };
            else
                return { QvKernelABIType::ABI_ELF_OTHER, std::nullopt };
        }
        else if (quint16 dosMagicMaybe; QDataStream(arr) >> dosMagicMaybe, dosMagicMaybe == 0x4D5Au)
            return { QvKernelABIType::ABI_WIN32, std::nullopt };
        else if (quint32 machOMagicMaybe; QDataStream(arr) >> machOMagicMaybe, machOMagicMaybe == 0xCAFEBABEu)
            return { QvKernelABIType::ABI_MACH_O, std::nullopt };
        else
            return { std::nullopt, QObject::tr("cannot deduce the type of core executable file %1").arg(pathCoreExecutable) };
    }

} // namespace Qv2ray::core::kernel::abi
