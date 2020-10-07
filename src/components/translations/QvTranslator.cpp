#include "QvTranslator.hpp"

#include "base/Qv2rayBase.hpp"
#include "utils/QvHelpers.hpp"

using namespace Qv2ray::base;

// path searching list.
QStringList getLanguageSearchPaths()
{
    // Configuration Path
    QStringList list = Qv2rayAssetsPaths("lang");
#ifdef QV2RAY_EMBED_TRANSLATIONS
    // If the translations have been embedded.
    list << QString(":/translations/");
#endif
#ifdef QV2RAY_TRANSLATION_PATH
    // Platform-specific dir, if specified.
    list << QString(QV2RAY_TRANSLATION_PATH);
#endif
    return list;
}

namespace Qv2ray::common
{
    QvTranslator::QvTranslator()
    {
        refreshTranslations();
    }

    void QvTranslator::refreshTranslations()
    {
        searchPaths = getLanguageSearchPaths();
        languages.clear();
        for (const auto &path : searchPaths)
        {
            languages << QDir(path).entryList({ "*.qm" }, QDir::Hidden | QDir::Files);
        }
        std::transform(languages.begin(), languages.end(), languages.begin(), [](QString &fileName) { return fileName.replace(".qm", ""); });
        languages.removeDuplicates();
        DEBUG(MODULE_UI, "Found translations: " + languages.join(" "))
    }

    bool QvTranslator::InstallTranslation(const QString &code)
    {
        for (const auto &path : searchPaths)
        {
            if (FileExistsIn(QDir(path), code + ".qm"))
            {
                DEBUG(MODULE_UI, "Found " + code + " in folder: " + path)
                QTranslator *translatorNew = new QTranslator();
                bool success = translatorNew->load(code + ".qm", path);
                if (!success)
                {
                    LOG(MODULE_UI, "Cannot load translation: " + code)
                }
                if (pTranslator)
                {
                    LOG(MODULE_UI, "Removed translations")
                    qApp->removeTranslator(pTranslator.get());
                }
                this->pTranslator.reset(translatorNew);
                qApp->installTranslator(pTranslator.get());
                LOG(MODULE_UI, "Successfully installed a translator for " + code)
                return true;
            }
        }
        return false;
    }
} // namespace Qv2ray::common
