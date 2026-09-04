/*
 * Copyright (c) 2026 Rahman Qadirzade
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <string>
#include <vector>
#include <mutex>

struct ServerInfo
{
    std::string projectname;
    std::string ip;

    int port = 0;

    std::string name;
    std::string mapname;

    int maxplayers = 4;
    int onlineplayers = 0;

    /*
        Linux process PID.
    */

    int pid = -1;
};


class UE5ServMan
{
public:

    bool hasPort(
        int port
    );

    bool hasName(
        const std::string& name
    );

    void addServer(
        const ServerInfo& server
    );

    bool getServer(
        const std::string& projectname,
        const std::string& name,
        ServerInfo& result
    );

    std::vector<ServerInfo> getServers(
        const std::string& projectname
    );

    bool deleteServer(
        const std::string& projectname,
        const std::string& name,
        ServerInfo& deletedServer
    );

private:

    std::vector<ServerInfo> servers;

    mutable std::mutex mutex;
};