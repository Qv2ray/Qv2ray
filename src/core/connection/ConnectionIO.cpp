#include "ConnectionIO.hpp"

#include "common/QvHelpers.hpp"

namespace Qv2ray::core::connection
{
    namespace ConnectionIO
    {
        CONFIGROOT ConvertConfigFromFile(const QString &sourceFilePath, bool importComplex)
        {
            QFile source(sourceFilePath);

            if (!source.exists())
            {
                LOG(MODULE_FILEIO, "Trying to import from an non-existing file.") return CONFIGROOT();
            }

            auto root = CONFIGROOT(JsonFromString(StringFromFile(source)));

            if (!importComplex)
            {
                root.remove("inbounds");
                root.remove("routing");
            }

            root.remove("log");
            root.remove("api");
            root.remove("stats");
            root.remove("dns");
            return root;
        }
    } // namespace ConnectionIO
} // namespace Qv2ray::core::connection
