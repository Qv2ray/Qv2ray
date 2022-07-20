#pragma once

#include <QString>
#include <optional>

std::pair<bool, std::optional<QString>> ValidateKernel(const QString &corePath, const QString &assetsPath, const QStringList &arguments);
