#pragma once
// Qv2ray build features.

// clang-format off
// clang-format on

#define QV2RAY_FEATURE(feat) ((1 / QVFEATURE_##feat) == 1)
