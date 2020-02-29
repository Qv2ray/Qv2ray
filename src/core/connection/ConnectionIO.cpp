#include "ConnectionIO.hpp"

#include "common/QvHelpers.hpp"

namespace Qv2ray::core::connection
{
    namespace ConnectionIO
    {
        CONFIGROOT ConvertConfigFromFile(QString sourceFilePath, bool importComplex)
        {
            QFile source(sourceFilePath);

            if (!source.exists())
            {
                LOG(MODULE_FILEIO, "Trying to import from an non-existing file.") return CONFIGROOT();
            }

            auto root = CONFIGROOT(JsonFromString(StringFromFile(&source)));

            if (!importComplex)
            {
                JSON_ROOT_TRY_REMOVE("inbounds")
                JSON_ROOT_TRY_REMOVE("routing")
            }

            JSON_ROOT_TRY_REMOVE("log")
            JSON_ROOT_TRY_REMOVE("api")
            JSON_ROOT_TRY_REMOVE("stats")
            JSON_ROOT_TRY_REMOVE("dns")
            return root;
        }
    } // namespace ConnectionIO
} // namespace Qv2ray::core::connection
