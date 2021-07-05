#include "QvGeositeReader.hpp"
#include "pb_decode.h"

#ifndef ANDROID
#include "v2ray_geosite.pb.h"
#endif

#define QV_MODULE_NAME "GeositeReader"
namespace {
    bool decode_country_code(pb_istream_t *stream, const pb_field_t *field, void **arg){
        uint8_t buffer[1024] = {0};
        if (stream->bytes_left > sizeof(buffer) - 1)
            return false;
        if (!pb_read(stream, buffer, stream->bytes_left))
            return false;
        auto list = reinterpret_cast<QStringList*>(*arg);
        list->append(QString::fromUtf8(reinterpret_cast<const char *>(buffer)));
        return true;
    }
    bool decode_entry(pb_istream_t *stream, const pb_field_t *field, void **arg){
        v2ray_core_app_router_GeoSite geoSite={};
        geoSite.country_code.funcs.decode=decode_country_code;
        geoSite.country_code.arg=*arg;
        auto status = pb_decode(stream, v2ray_core_app_router_GeoSite_fields, &geoSite);
        if(!status){
            return false;
        }
        return true;
    }
    bool read_callback(pb_istream_t *stream, uint8_t *buf, size_t count)
    {
        auto file = reinterpret_cast<QFile*>(stream->state);
        quint64 result;
        if (count == 0)
            return true;
        result=file->read(reinterpret_cast<char *>(buf),static_cast<qint64>(count));
        if (result == 0)
            stream->bytes_left = 0; /* EOF */
        return result == count;
    }
}
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
            LOG("Reading geosites from: " + filepath);
            //
            //
            QFile f(filepath);
            bool opened = f.open(QFile::OpenModeFlag::ReadOnly);

            if (!opened)
            {
                LOG("File cannot be opened: " + filepath);
                return list;
            }
            v2ray_core_app_router_GeoSiteList siteList={};
            siteList.entry.funcs.decode= decode_entry;
            siteList.entry.arg=&list;
            pb_istream_t stream = {&read_callback, &f, SIZE_MAX};
            auto status = pb_decode(&stream, v2ray_core_app_router_GeoSiteList_fields, &siteList);
            if(!status){
                LOG("Decoding failed: " + QString(PB_GET_ERROR(&stream)));
                return list;
            }
            LOG("Loaded " + QSTRN(list.count()) + " geosite entries from data file.");
#endif
            list.sort();
            GeositeEntries[filepath] = list;
            return list;
        }
    }
} // namespace Qv2ray::components::geosite
