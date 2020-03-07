#pragma once
#include <QString>
#include <QTranslator>
#include <memory>

namespace Qv2ray::common
{
    class QvTranslator
    {
      public:
        explicit QvTranslator()
        {
            this->reloadTranslationDirs();
        }

      public:
        /**
         * @brief search and deduce a directory for translation file
         * @return (if any) the deduced path
         *
         * @author DuckSoft <realducksoft@gmail.com>
         * @todo add some debug output
         */
        static std::optional<QString> deduceTranslationDir();
        /**
         * @brief get the available languages.
         * @return (if available) languages (zh_CN, en_US, ...)
         */
        std::optional<QStringList> getAvailableLanguages();
        /**
         * @brief reload the translation from file
         * @param code eg: en_US, zh_CN, ...
         */
        void reloadTranslation(const QString &);
        inline bool isTranslationAvailable() const
        {
            return this->translationDir.has_value();
        };
        inline void reloadTranslationDirs()
        {
            this->translationDir = this->deduceTranslationDir();
        };

      private:
        /**
         * @brief test the translation directory
         * @param targetDir the directory to judge
         * @return if the translation directory seems good to use
         */
        static bool testTranslationDir(const QString &);

      public:
        std::unique_ptr<QTranslator> pTranslator;
        std::optional<QString> translationDir;
    };
} // namespace Qv2ray::common
