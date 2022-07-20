//  Qv2rayBase, the modular feature-rich infrastructure library for Qv2ray.
//  Copyright (C) 2021 Moody and relavent Qv2ray contributors.
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "Qv2rayBase/private/Common/SettingsUpgrade_p.hpp"

#include "Qv2rayBase/Common/Settings.hpp"
#include "Qv2rayBase/Qv2rayBaseLibrary.hpp"

namespace Qv2rayBase::_private
{
    bool UpgradeConfigInc(int fromVersion, const QJsonObject &original)
    {
        auto root = original;
        switch (fromVersion)
        {
            default:
            {
                Qv2rayBaseLibrary::Warn(QObject::tr("Configuration Upgrade Failed"),
                                        QObject::tr("Unsupported config version number: ") + QString::number(fromVersion) + NEWLINE + NEWLINE +
                                            QObject::tr("Please go to https://github.com/Qv2ray/Qv2ray/issues to check for related announcements."));
                qInfo() << "The configuration version of your old Qv2ray installation is out-of-date and that version is not supported anymore.";
                qInfo() << "Please try to update to an intermediate version of Qv2ray first.";
                return false;
            }
        }
        root[u"config_version"_qs] = root[u"config_version"_qs].toInt() + 1;
        return true;
    }

    bool MigrateSettings(QJsonObject &original)
    {
        const auto fileVersion = original[u"config_version"_qs].toInt(QV2RAY_SETTINGS_VERSION);
        qInfo() << "Migrating config from version" << fileVersion;

        for (int i = fileVersion; i < QV2RAY_SETTINGS_VERSION; i++)
            if (!UpgradeConfigInc(i, original))
                return false;

        original[u"config_version"_qs] = QV2RAY_SETTINGS_VERSION;
        return true;
    }
} // namespace Qv2rayBase::_private
