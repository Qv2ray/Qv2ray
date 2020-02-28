#pragma once
#include <QString>
#include <QTranslator>
#include <memory>

namespace Qv2ray::common
{
    class QvTranslator
    {
      public:
        QvTranslator(const QString &lang)
        {
            QTranslator *translator = new QTranslator();
            translator->load(lang + ".qm", ":/translations/");
            this->pTranslator.reset(translator);
        }

      public:
        std::unique_ptr<QTranslator> pTranslator;
    };
} // namespace Qv2ray::common
