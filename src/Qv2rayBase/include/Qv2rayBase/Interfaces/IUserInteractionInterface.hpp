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

#pragma once

#include <QString>
#include <QUrl>

namespace Qv2rayBase
{
    enum class MessageOpt
    {
        // clang-format off
            OK,     Cancel,
            Yes,    No,
            Ignore
        // clang-format on
    };

    namespace Interfaces
    {
        class IUserInteractionInterface
        {
          public:
            virtual ~IUserInteractionInterface() = default;
            virtual void p_MessageBoxWarn(const QString &title, const QString &text) = 0;
            virtual void p_MessageBoxInfo(const QString &title, const QString &text) = 0;
            virtual MessageOpt p_MessageBoxAsk(const QString &title, const QString &text, const QList<MessageOpt> &options) = 0;
            virtual void p_OpenURL(const QUrl &url) = 0;
        };
    } // namespace Interfaces
} // namespace Qv2rayBase
