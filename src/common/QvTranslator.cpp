#include "QvTranslator.hpp"

#include "base/Qv2rayLog.hpp"

#include <QApplication>
#include <QDir>
#include <QStandardPaths>
#include <QString>
#include <QStringBuilder>
#include <QTranslator>
#include <memory>

using namespace Qv2ray::base;

namespace Qv2ray::common
{
    std::optional<QStringList> QvTranslator::getAvailableLanguages()
    {
        if (!this->translationDir)
            return std::nullopt;

        auto languages = QDir(this->translationDir.value()).entryList(QStringList{ "*.qm" }, QDir::Hidden | QDir::Files);

        if (languages.empty())
            return std::nullopt;

        std::transform(languages.begin(), languages.end(), languages.begin(), [](QString &fileName) { return fileName.replace(".qm", ""); });
        LOG(MODULE_UI, "Found translations: " + languages.join(" "))
        return languages;
    }

    void QvTranslator::reloadTranslation(const QString &code)
    {
        if (!translationDir)
            return;

        QTranslator *translatorNew = new QTranslator();
        translatorNew->load(code + ".qm", this->translationDir.value());

        this->pTranslator.reset(translatorNew);
    }

    std::optional<QString> QvTranslator::deduceTranslationDir()
    {
        // path searching list.
        QStringList searchPaths = {
            // 1st: application dir
            QApplication::applicationDirPath() + "/translations",
#ifdef QV2RAY_TRANSLATION_PATH
            // 2nd: platform-specific dir
            QString(QV2RAY_TRANSLATION_PATH),
#endif
        };
        // 3rd: standard path dirs
        searchPaths << QStandardPaths::locateAll(QStandardPaths::DataLocation, "translations", QStandardPaths::LocateDirectory);
        //
        // iterate through the paths
        for (auto path : searchPaths)
        {
            DEBUG(MODULE_UI, "Testing for translation path: " + path)
            if (QvTranslator::testTranslationDir(path))
            {
                return path;
            }
        }

        // sadly, none match
        return std::nullopt;
    }

    bool QvTranslator::testTranslationDir(const QString &targetDir)
    {
        const auto translations = QDir(targetDir).entryList(QStringList{ "*.qm" }, QDir::Hidden | QDir::Files);

        /// @todo: add some debug traces

        return !translations.empty();
    }
} // namespace Qv2ray::common
