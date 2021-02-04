#pragma once
// Qv2ray build features.

// clang-format off
#define QVFEATURE_kernel_check_abi               1
#define QVFEATURE_kernel_check_permission        1
#define QVFEATURE_kernel_set_permission          1
#define QVFEATURE_kernel_check_output            1
#define QVFEATURE_kernel_check_filename          1
// clang-format on

#define QV2RAY_HAS_FEATURE(feat) ((1 / QVFEATURE_##feat) == 1)
