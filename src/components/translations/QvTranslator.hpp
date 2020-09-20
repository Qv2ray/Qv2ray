#pragma once
#include <QString>
#include <QTranslator>
#include <memory>
#include <optional>

namespace Qv2ray::common
{
    class QvTranslator
    {
      public:
        explicit QvTranslator();

      public:
        /**
         * @brief get the available languages.
         * @return (if available) languages (zh_CN, en_US, ...)
         */
        const inline QStringList GetAvailableLanguages() const
        {
            return languages;
        }
        /**
         * @brief reload the translation from file
         * @param code eg: en_US, zh_CN, ...
         */
        bool InstallTranslation(const QString &);

      private:
        void refreshTranslations();
        QStringList languages;
        QStringList searchPaths;
        std::unique_ptr<QTranslator> pTranslator;
    };
    inline std::unique_ptr<common::QvTranslator> Qv2rayTranslator;
} // namespace Qv2ray::common

using namespace Qv2ray::common;
