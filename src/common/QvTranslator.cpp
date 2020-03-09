#include "QvTranslator.hpp"

#include "base/Qv2rayLog.hpp"
#include "common/QvHelpers.hpp"

#include <QApplication>
#include <QDir>
#include <QStandardPaths>
#include <QString>
#include <QStringBuilder>
#include <QTranslator>
#include <memory>

using namespace Qv2ray::base;

// path searching list.
QStringList getLanguageSearchPaths()
{
    return {
        // Configuration Path
        QV2RAY_CONFIG_DIR + "lang",
//
#ifdef EMBED_TRANSLATIONS
        // If the translations have been embedded.
        QString(":/translations/"),
#endif
#ifdef QV2RAY_TRANSLATION_PATH
        // Platform-specific dir, if specified.
        QString(QV2RAY_TRANSLATION_PATH),
#endif
#ifdef Q_OS_LINUX
        // Linux platform directories.
        QString("/usr/local/share/qv2ray/lang"),
        QString("/usr/share/qv2ray/lang"),
#elif defined(Q_OS_MAC)
        // macOS platform directories.
        QDir(QApplication::applicationDirPath() + "/../Resources/lang").absolutePath(),
#else
        // This is the default behavior on Windows
        QApplication::applicationDirPath() + "/lang",
#endif
    };
};

namespace Qv2ray::common
{
    QvTranslator::QvTranslator()
    {
        LOG(MODULE_UI, "QvTranslator constructor.")
        GetAvailableLanguages();
    }

    QStringList QvTranslator::GetAvailableLanguages()
    {
        languages.clear();
        for (auto path : getLanguageSearchPaths())
        {
            languages << QDir(path).entryList(QStringList{ "*.qm" }, QDir::Hidden | QDir::Files);
        }
        std::transform(languages.begin(), languages.end(), languages.begin(), [](QString &fileName) { return fileName.replace(".qm", ""); });
        languages.removeDuplicates();
        LOG(MODULE_UI, "Found translations: " + languages.join(" "))
        return languages;
    }

    bool QvTranslator::InstallTranslation(const QString &code)
    {
        for (auto path : getLanguageSearchPaths())
        {
            if (FileExistsIn(QDir(path), code + ".qm"))
            {
                LOG(MODULE_UI, "Found " + code + " in folder: " + path)
                QTranslator *translatorNew = new QTranslator();
                translatorNew->load(code + ".qm", path);
                if (pTranslator)
                {
                    LOG(MODULE_INIT, "Removed translations")
                    qApp->removeTranslator(pTranslator.get());
                }
                this->pTranslator.reset(translatorNew);
                qApp->installTranslator(pTranslator.get());
                return true;
            }
        }
        return false;
    }
} // namespace Qv2ray::common
