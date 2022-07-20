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

#include "Qv2rayBase/Qv2rayBase_export.h"

#include <QDebug>

#define QV2RAYBASE_FEATURE(feat) ((1 / QVFEATURE_##feat) == 1)

#ifdef QT_DEBUG
#define _BOMB_ (static_cast<QObject *>(nullptr)->event(nullptr))
#else
#define _BOMB_
#endif

#define NEWLINE "\n"
