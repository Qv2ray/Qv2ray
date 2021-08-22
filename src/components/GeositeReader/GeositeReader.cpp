#include "GeositeReader.hpp"

#include "Qv2rayBase/Qv2rayBaseFeatures.hpp"
#include "picoproto.hpp"

#include <QFile>
#include <QMap>

namespace Qv2ray::components::GeositeReader
{
    QMap<QString, QStringList> GeositeEntries;

    QStringList ReadGeoSiteFromFile(const QString &filepath, bool allowCache)
    {
        if (GeositeEntries.contains(filepath) && allowCache)
            return GeositeEntries.value(filepath);

        QStringList list;
        qInfo() << "Reading geosites from:" << filepath;
        QFile f(filepath);
        bool opened = f.open(QFile::OpenModeFlag::ReadOnly);

        if (!opened)
        {
            qInfo() << "File cannot be opened:" << filepath;
            return list;
        }

        const auto content = f.readAll();
        f.close();
        {
            picoproto::Message root;
            root.ParseFromBytes((unsigned char *) content.data(), content.size());

            list.reserve(root.GetMessageArray(1).size());
            for (const auto &geosite : root.GetMessageArray(1))
                list << QString::fromStdString(geosite->GetString(1));
        }

        qInfo() << "Loaded" << list.count() << "geosite entries from data file.";
        list.sort();
        GeositeEntries[filepath] = list;
        return list;
    }
} // namespace Qv2ray::components::geosite
