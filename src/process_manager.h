/*
 * Copyright (c) 2026 Rahman Qadirzade
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <string>

class ProcessManager
{
public:

    static bool startServer(
        const std::string& scriptPath,
        const std::string& serverUser,
        const std::string& name,
        const std::string& mapname,
        int maxplayers,
        int port,
        int& outPid
    );


    static bool stopServer(
        int pid
    );


private:

    static bool changeUser(
        const std::string& username
    );
};