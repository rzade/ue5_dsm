/*
 * Copyright (c) 2026 Rahman Qadirzade
 *
 * SPDX-License-Identifier: MIT
 */

#include "server_manager.h"


bool UE5ServMan::hasPort(
    int port)
{
    std::lock_guard<std::mutex> lock(mutex);

    for (const auto& server : servers)
    {
        if (server.port == port)
        {
            return true;
        }
    }

    return false;
}


bool UE5ServMan::hasName(
    const std::string& name)
{
    std::lock_guard<std::mutex> lock(mutex);

    for (const auto& server : servers)
    {
        if (server.name == name)
        {
            return true;
        }
    }

    return false;
}


void UE5ServMan::addServer(
    const ServerInfo& server)
{
    std::lock_guard<std::mutex> lock(mutex);

    servers.push_back(server);
}


bool UE5ServMan::getServer(
    const std::string& projectname,
    const std::string& name,
    ServerInfo& result)
{
    std::lock_guard<std::mutex> lock(mutex);

    for (const auto& server : servers)
    {
        if (
            server.projectname == projectname &&
            server.name == name
        )
        {
            result = server;

            return true;
        }
    }

    return false;
}


std::vector<ServerInfo>
UE5ServMan::getServers(
    const std::string& projectname)
{
    std::lock_guard<std::mutex> lock(mutex);

    std::vector<ServerInfo> result;

    for (const auto& server : servers)
    {
        if (server.projectname == projectname)
        {
            result.push_back(server);
        }
    }

    return result;
}


bool UE5ServMan::deleteServer(
    const std::string& projectname,
    const std::string& name,
    ServerInfo& deletedServer)
{
    std::lock_guard<std::mutex> lock(mutex);

    for (
        auto it = servers.begin();
        it != servers.end();
        ++it)
    {
        if (
            it->projectname == projectname &&
            it->name == name
        )
        {
            deletedServer = *it;

            servers.erase(it);

            return true;
        }
    }

    return false;
}