#pragma once
#include <QApplication>
#include <QDir>
#include <QStandardPaths>
#include <QString>
#include <QStringBuilder>
#include <QTranslator>
#include <memory>

namespace Qv2ray::common
{
    class QvTranslator
    {
      public:
        QvTranslator()
        {
            this->reloadTranslationDirs();
        }

      public:
        static std::optional<QString> deduceTranslationDir();
        std::optional<QStringList> getAvailableLanguages();
        void reloadTranslation(const QString &);
        inline bool isTranslationAvailable()
        {
            return this->translationDir.has_value();
        };
        inline void reloadTranslationDirs()
        {
            this->translationDir = this->deduceTranslationDir();
        };

      private:
        static bool testTranslationDir(const QString &);

      public:
        std::unique_ptr<QTranslator> pTranslator;
        std::optional<QString> translationDir;
    };

    /**
     * @brief get the available languages.
     * @return (if available) languages (zh_CN, en_US, ...)
     */
    std::optional<QStringList> QvTranslator::getAvailableLanguages()
    {
        if (!this->translationDir)
            return std::nullopt;

        auto languages = QDir(this->translationDir.value()).entryList(QStringList{ "*.qm" }, QDir::Hidden | QDir::Files);

        if (languages.empty())
            return std::nullopt;

        std::transform(languages.begin(), languages.end(), languages.begin(), [](QString &fileName) { return fileName.replace(".qm", ""); });

        return languages;
    }

    /**
     * @brief reload the translation from file
     * @param code eg: en_US, zh_CN, ...
     */
    void QvTranslator::reloadTranslation(const QString &code)
    {
        if (!translationDir)
            return;

        QTranslator *translatorNew = new QTranslator();
        translatorNew->load(code + ".qm", this->translationDir.value());

        this->pTranslator.reset(translatorNew);
    }

    /**
     * @brief search and deduce a directory for translation file
     * @return (if any) the deduced path
     *
     * @author DuckSoft <realducksoft@gmail.com>
     * @todo add some debug output
     */
    std::optional<QString> QvTranslator::deduceTranslationDir()
    {
        // path searching list.
        auto searchPaths = { // 1st: application dir
                             QApplication::applicationDirPath() + "/translations",
#ifdef QV2RAY_TRANSLATION_PATH
                             // 2nd: platform-specific dir
                             QString(QV2RAY_TRANSLATION_PATH),
#endif
                             // 3rd: standard path dirs
                             QStandardPaths::locate(QStandardPaths::DataLocation, "translations", QStandardPaths::LocateDirectory)
        };

        // iterate through the paths
        for (auto path : searchPaths)
        {
            if (QvTranslator::testTranslationDir(path))
            {
                return path;
            }
        }

        // sadly, none match
        return std::nullopt;
    }

    /**
     * @brief test the translation directory
     * @param targetDir the directory to judge
     * @return if the translation directory seems good to use
     */
    bool QvTranslator::testTranslationDir(const QString &targetDir)
    {
        const auto translations = QDir(targetDir).entryList(QStringList{ "*.qm" }, QDir::Hidden | QDir::Files);

        /// @todo: add some debug traces

        return !translations.empty();
    }
} // namespace Qv2ray::common
