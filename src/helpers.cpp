/*
 * Copyright (c) 2026 Rahman Qadirzade
 *
 * SPDX-License-Identifier: MIT
 */

#include "helpers.h"

#include <cctype>

std::string removeSpecialCharacters(
    const std::string& str)
{
    std::string result;

    result.reserve(str.size());

    for (unsigned char c : str)
    {
        if (
            (c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') ||
            c == '_' ||
            c == ' '
        )
        {
            result += static_cast<char>(c);
        }
    }

    return result;
}


bool isSafePathComponent(
    const std::string& str)
{
    if (str.empty())
    {
        return false;
    }

    if (str == "." || str == "..")
    {
        return false;
    }

    /*
        Path traversal obstruction.
    */

    if (str.find("..") != std::string::npos)
    {
        return false;
    }

    if (
        str.find('/') != std::string::npos ||
        str.find('\\') != std::string::npos
    )
    {
        return false;
    }

    for (unsigned char c : str)
    {
        if (
            (c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') ||
            c == '_' ||
            c == '-' ||
            c == '.'
        )
        {
            continue;
        }

        return false;
    }

    return true;
}


bool isValidPort(
    int port)
{
    return port >= 1 && port <= 65535;
}