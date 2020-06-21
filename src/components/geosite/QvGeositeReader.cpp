#include "QvGeositeReader.hpp"

#ifndef ANDROID
    #include "v2ray_geosite.pb.h"
#endif

namespace Qv2ray::components::geosite
{
    QMap<QString, QStringList> GeositeEntries;
    QStringList ReadGeoSiteFromFile(const QString &filepath)
    {
        if (GeositeEntries.contains(filepath))
        {
            return GeositeEntries.value(filepath);
        }
        else
        {
            QStringList list;
#ifndef ANDROID
            LOG(MODULE_FILEIO, "Reading geosites from: " + filepath)
            //
            GOOGLE_PROTOBUF_VERIFY_VERSION;
            //
            QFile f(filepath);
            bool opened = f.open(QFile::OpenModeFlag::ReadOnly);

            if (!opened)
            {
                LOG(MODULE_FILEIO, "File cannot be opened: " + filepath)
                return list;
            }

            auto content = f.readAll();
            f.close();
            //
            GeoSiteList sites;
            sites.ParseFromArray(content.data(), content.size());

            for (const auto &e : sites.entry())
            {
                // We want to use lower string.
                list << QString::fromStdString(e.country_code()).toLower();
            }

            LOG(MODULE_FILEIO, "Loaded " + QSTRN(list.count()) + " geosite entries from data file.")
            // Optional:  Delete all global objects allocated by libprotobuf.
            google::protobuf::ShutdownProtobufLibrary();
#endif
            list.sort();
            GeositeEntries[filepath] = list;
            return list;
        }
    }
} // namespace Qv2ray::components::geosite
