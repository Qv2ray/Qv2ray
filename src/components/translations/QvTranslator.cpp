#include "QvTranslator.hpp"

#include "base/Qv2rayBase.hpp"
#include "utils/QvHelpers.hpp"

#define QV_MODULE_NAME "Translator"

using namespace Qv2ray::base;

// path searching list.
QStringList getLanguageSearchPaths()
{
    // Configuration Path
    QStringList list = QvCoreApplication->GetAssetsPaths("lang");
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
        DEBUG("Found translations: " + languages.join(" "));
    }

    bool QvTranslator::InstallTranslation(const QString &code)
    {
        for (const auto &path : searchPaths)
        {
            if (FileExistsIn(QDir(path), code + ".qm"))
            {
                DEBUG("Found " + code + " in folder: " + path);
                QTranslator *translatorNew = new QTranslator();
                bool success = translatorNew->load(code + ".qm", path);
                if (!success)
                {
                    LOG("Cannot load translation: " + code);
                }
                if (pTranslator)
                {
                    LOG("Removed translations");
                    qApp->removeTranslator(pTranslator.get());
                }
                this->pTranslator.reset(translatorNew);
                qApp->installTranslator(pTranslator.get());
                LOG("Successfully installed a translator for", code);
                return true;
            }
        }
        return false;
    }
} // namespace Qv2ray::common
