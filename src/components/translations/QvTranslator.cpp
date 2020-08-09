#include "QvTranslator.hpp"

#include "base/Qv2rayBase.hpp"
#include "common/QvHelpers.hpp"

using namespace Qv2ray::base;

// path searching list.
QStringList getLanguageSearchPaths()
{
    // Configuration Path
    QStringList list = Qv2rayAssetsPaths("lang");
#ifdef EMBED_TRANSLATIONS
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
        GetAvailableLanguages();
    }

    QStringList QvTranslator::GetAvailableLanguages()
    {
        languages.clear();
        for (const auto &path : getLanguageSearchPaths())
        {
            languages << QDir(path).entryList(QStringList{ "*.qm" }, QDir::Hidden | QDir::Files);
        }
        std::transform(languages.begin(), languages.end(), languages.begin(), [](QString &fileName) { return fileName.replace(".qm", ""); });
        languages.removeDuplicates();
        DEBUG(MODULE_UI, "Found translations: " + languages.join(" "))
        return languages;
    }

    bool QvTranslator::InstallTranslation(const QString &code)
    {
        for (const auto &path : getLanguageSearchPaths())
        {
            if (FileExistsIn(QDir(path), code + ".qm"))
            {
                DEBUG(MODULE_UI, "Found " + code + " in folder: " + path)
                QTranslator *translatorNew = new QTranslator();
                translatorNew->load(code + ".qm", path);
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
