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
        QStringList GetAvailableLanguages();
        /**
         * @brief reload the translation from file
         * @param code eg: en_US, zh_CN, ...
         */
        bool InstallTranslation(const QString &);

      private:
        QStringList languages;
        std::unique_ptr<QTranslator> pTranslator;
    };
} // namespace Qv2ray::common
