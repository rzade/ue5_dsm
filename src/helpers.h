/*
 * Copyright (c) 2026 Rahman Qadirzade
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <string>

std::string removeSpecialCharacters(
    const std::string& str
);

bool isSafePathComponent(
    const std::string& str
);

bool isValidPort(
    int port
);