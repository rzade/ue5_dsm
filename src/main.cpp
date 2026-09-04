/*
 * Copyright (c) 2026 Rahman Qadirzade
 *
 * SPDX-License-Identifier: MIT
 */

#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <system_error>
#include <vector>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "httplib.h"
#include "../third_party/json.hpp"

#include "config.h"
#include "helpers.h"
#include "server_manager.h"
#include "process_manager.h"

namespace fs = std::filesystem;

using json = nlohmann::json;

/*
Global server manager.
*/

UE5ServMan UE5ServMan;

/*
============================================================
JSON
============================================================
*/

json serverToJson(
    const ServerInfo& server)
{
    return json{
        {"projectname", server.projectname},
        {"ip", server.ip},
        {"port", server.port},
        {"name", server.name},
        {"mapname", server.mapname},
        {"maxplayers", server.maxplayers},
        {"onlineplayers", server.onlineplayers}
    };
}

/*
============================================================
AUTH
============================================================
*/

bool authorize(
    const json& body)
{
    if (!body.contains("authorization"))
    {
        return false;
    }

    if (!body["authorization"].is_string())
    {
        return false;
    }

    const std::string authorization =
        body["authorization"].get<std::string>();

    return authorization ==
           Config::AUTH_KEY;
}

/*
============================================================
JSON RESPONSE
============================================================
*/

void jsonResponse(
    httplib::Response& response,
    const json& data,
    int status = 200)
{
    response.status = status;

    response.set_content(
        data.dump(),
        "application/json"
    );
}

/*
============================================================
SAFE PATH
============================================================
*/

bool validBuildName(
    const std::string& buildname)
{
    return isSafePathComponent(
        buildname);
}

bool validProjectName(
    const std::string& projectname)
{
    return isSafePathComponent(
        projectname);
}

/*
============================================================
PREPARE BUILD
============================================================
*/

void prepareBuild(
    const httplib::Request& request,
    httplib::Response& response)
{
    try
    {
        json body =
            json::parse(request.body);

        if (!authorize(body))
        {
            jsonResponse(
                response,
                {
                    {"success", false},
                    {"active", false},
                    {"message",
                     "Authorization Key not confirm."}
                }
            );

            return;
        }

        std::string buildname =
            body.value(
                "buildname",
                ""
            );

        if (!validBuildName(buildname))
        {
            jsonResponse(
                response,
                {
                    {"success", false},
                    {"active", false},
                    {"message",
                     "Invalid build name."}
                },
                400
            );

            return;
        }

        fs::path serverBuildDir =
            Config::SERVER_BUILD_DIRECTORY;

        fs::path zipPath =
            serverBuildDir /
            (buildname + ".zip");

        fs::path buildPath =
            serverBuildDir /
            buildname;

        /*
            no ZIP file.
        */

        if (!fs::exists(zipPath))
        {
            jsonResponse(
                response,
                {
                    {"success", false},
                    {"active", false},
                    {"message",
                     "New build version not found."}
                }
            );

            return;
        }

        /*
            Delete the old build if it exists.
        */

        if (fs::exists(buildPath))
        {
            std::error_code error;

            fs::remove_all(
                buildPath,
                error
            );

            if (error)
            {
                jsonResponse(
                    response,
                    {
                        {"success", false},
                        {"active", false},
                        {"message",
                         "Could not remove old build."}
                    },
                    500
                );

                return;
            }
        }

        fs::create_directories(
            serverBuildDir
        );

        /*
            To unzip the file, use the child process.
        */

        pid_t pid =
            fork();

        if (pid < 0)
        {
            jsonResponse(
                response,
                {
                    {"success", false},
                    {"active", false},
                    {"message",
                     "Could not start unzip."}
                },
                500
            );

            return;
        }

        if (pid == 0)
        {
            execlp(
                "unzip",
                "unzip",
                "-o",
                zipPath.c_str(),
                "-d",
                serverBuildDir.c_str(),
                static_cast<char*>(nullptr)
            );

            _exit(127);
        }

        int status = 0;

        waitpid(
            pid,
            &status,
            0
        );

        if (!WIFEXITED(status) ||
            WEXITSTATUS(status) != 0)
        {
            jsonResponse(
                response,
                {
                    {"success", false},
                    {"active", false},
                    {"message",
                     "Could not extract build."}
                },
                500
            );

            return;
        }

        /*
            Check that the build has been created.
        */

        if (!fs::exists(buildPath))
        {
            jsonResponse(
                response,
                {
                    {"success", false},
                    {"active", false},
                    {"message",
                     "Build extraction failed."}
                },
                500
            );

            return;
        }

        /*
        ========================================================
        CHOWN
        ========================================================

        Config::SERVER_USER:

            "rzade"

        Result:

            chown -R rzade:rzade ./ServerBuild/<buildname>
        */

        pid_t chownPid =
            fork();

        if (chownPid < 0)
        {
            jsonResponse(
                response,
                {
                    {"success", false},
                    {"active", false},
                    {"message",
                     "Could not start chown."}
                },
                500
            );

            return;
        }

        if (chownPid == 0)
        {
            std::string ownerGroup =
                Config::SERVER_USER +
                ":" +
                Config::SERVER_USER;

            execlp(
                "chown",
                "chown",
                "-R",
                ownerGroup.c_str(),
                buildPath.c_str(),
                static_cast<char*>(nullptr)
            );

            _exit(127);
        }

        int chownStatus = 0;

        waitpid(
            chownPid,
            &chownStatus,
            0
        );

        if (
            !WIFEXITED(chownStatus) ||
            WEXITSTATUS(chownStatus) != 0
        )
        {
            jsonResponse(
                response,
                {
                    {"success", false},
                    {"active", false},
                    {"message",
                     "Could not change build owner."}
                },
                500
            );

            return;
        }

        /*
            Delete the ZIP file.
        */

        std::error_code removeError;

        fs::remove(
            zipPath,
            removeError
        );

        jsonResponse(
            response,
            {
                {"success", true},
                {"message",
                 "Build prepare successfull."}
            }
        );
    }
    catch (const std::exception& error)
    {
        jsonResponse(
            response,
            {
                {"success", false},
                {"message", error.what()}
            },
            400
        );
    }
}

/*
============================================================
CREATE SERVER
============================================================
*/

void createServer(
    const httplib::Request& request,
    httplib::Response& response)
{
    try
    {
        json body =
            json::parse(request.body);

        if (!authorize(body))
        {
            jsonResponse(
                response,
                {
                    {"success", false},
                    {"active", false},
                    {"message",
                     "Authorization Key not confirm."}
                }
            );

            return;
        }

        std::string buildname =
            body.value(
                "buildname",
                ""
            );

        std::string projectname =
            body.value(
                "projectname",
                ""
            );

        std::string name =
            body.value(
                "name",
                ""
            );

        std::string mapname =
            body.value(
                "mapname",
                ""
            );

        int port =
            body.value(
                "port",
                0
            );

        int maxplayers =
            body.value(
                "maxplayers",
                4
            );

        if (maxplayers <= 0)
        {
            maxplayers = 4;
        }

        /*
            Validation.
        */

        if (
            !validBuildName(buildname) ||
            !validProjectName(projectname) ||
            name.empty() ||
            mapname.empty() ||
            !isValidPort(port)
        )
        {
            jsonResponse(
                response,
                {
                    {"success", false},
                    {"active", false},
                    {"message",
                     "Please check all credentials (projectname, port, name, mapname, maxplayers)."}
                }
            );

            return;
        }

        /*
            Name/mapname is being cleaned before going to the shell.
        */

        std::string cleanName =
            removeSpecialCharacters(
                name
            );

        std::string cleanMapName =
            removeSpecialCharacters(
                mapname
            );

        if (cleanName.empty())
        {
            jsonResponse(
                response,
                {
                    {"success", false},
                    {"active", false},
                    {"message",
                     "Invalid server name."}
                }
            );

            return;
        }

        if (cleanMapName.empty())
        {
            jsonResponse(
                response,
                {
                    {"success", false},
                    {"active", false},
                    {"message",
                     "Invalid map name."}
                }
            );

            return;
        }

        /*
            Build path.
        */

        fs::path buildPath =
            fs::path(
                Config::SERVER_BUILD_DIRECTORY
            ) /
            buildname;

        if (!fs::exists(buildPath))
        {
            jsonResponse(
                response,
                {
                    {"success", false},
                    {"active", false},
                    {"message",
                     "Build folder not found."}
                }
            );

            return;
        }

        /*
            Unreal .sh path.
        */

        fs::path scriptPath =
            buildPath /
            (projectname + ".sh");

        if (!fs::exists(scriptPath))
        {
            jsonResponse(
                response,
                {
                    {"success", false},
                    {"active", false},
                    {"message",
                     "Project not found."}
                }
            );

            return;
        }

        /*
            Same port already exist?
        */

        if (UE5ServMan.hasPort(port))
        {
            jsonResponse(
                response,
                {
                    {"success", true},
                    {"active", true},
                    {"message",
                     "Server already active now."}
                }
            );

            return;
        }

        /*
            Same name already exist?
        */

        if (UE5ServMan.hasName(name))
        {
            jsonResponse(
                response,
                {
                    {"success", true},
                    {"active", false},
                    {"message",
                     "Server already have."}
                }
            );

            return;
        }

        /*
            Start Server process.
        */

        int pid = -1;

        bool started =
            ProcessManager::startServer(
                scriptPath.string(),
                Config::SERVER_USER,
                cleanName,
                cleanMapName,
                maxplayers,
                port,
                pid
            );

        if (!started)
        {
            jsonResponse(
                response,
                {
                    {"success", false},
                    {"active", false},
                    {"message",
                     "Could not start server process."}
                },
                500
            );

            return;
        }

        /*
            Add to server list.
        */

        ServerInfo server;

        server.projectname =
            projectname;

        server.ip =
            Config::SERVER_IP;

        server.port =
            port;

        server.name =
            name;

        server.mapname =
            mapname;

        server.maxplayers =
            maxplayers;

        server.onlineplayers =
            0;

        server.pid =
            pid;

        UE5ServMan.addServer(
            server
        );

        jsonResponse(
            response,
            {
                {"success", true},
                {"active", false},
                {"message",
                 "Server created successfull."}
            }
        );
    }
    catch (const std::exception& error)
    {
        jsonResponse(
            response,
            {
                {"success", false},
                {"active", false},
                {"message",
                 error.what()}
            },
            400
        );
    }
}

/*
============================================================
SERVER LIST
============================================================
*/

void serverList(
    const httplib::Request& request,
    httplib::Response& response)
{
    try
    {
        json body =
            json::parse(request.body);

        if (!authorize(body))
        {
            jsonResponse(
                response,
                {
                    {"servers", json::array()},
                    {"message",
                     "Authorization Key not confirm."}
                }
            );

            return;
        }

        std::string projectname =
            body.value(
                "projectname",
                ""
            );

        auto servers =
            UE5ServMan.getServers(
                projectname
            );

        json result;

        result["servers"] =
            json::array();

        for (const auto& server : servers)
        {
            result["servers"].push_back(
                serverToJson(server)
            );
        }

        result["message"] = "";

        jsonResponse(
            response,
            result
        );
    }
    catch (const std::exception& error)
    {
        jsonResponse(
            response,
            {
                {"servers", json::array()},
                {"message",
                 error.what()}
            },
            400
        );
    }
}

/*
============================================================
SERVER DATA
============================================================
*/

void serverData(
    const httplib::Request& request,
    httplib::Response& response)
{
    try
    {
        json body =
            json::parse(request.body);

        if (!authorize(body))
        {
            jsonResponse(
                response,
                {
                    {"message",
                     "Authorization Key not confirm."}
                }
            );

            return;
        }

        std::string projectname =
            body.value(
                "projectname",
                ""
            );

        std::string name =
            body.value(
                "name",
                ""
            );

        ServerInfo server;

        if (
            UE5ServMan.getServer(
                projectname,
                name,
                server
            )
        )
        {
            jsonResponse(
                response,
                serverToJson(server)
            );
        }
        else
        {
            jsonResponse(
                response,
                {
                    {"message",
                     "Server not found."}
                }
            );
        }
    }
    catch (const std::exception& error)
    {
        jsonResponse(
            response,
            {
                {"message",
                 error.what()}
            },
            400
        );
    }
}

/*
============================================================
DELETE SERVER
============================================================
*/

void deleteServer(
    const httplib::Request& request,
    httplib::Response& response)
{
    try
    {
        json body =
            json::parse(request.body);

        if (!authorize(body))
        {
            jsonResponse(
                response,
                {
                    {"servers", json::array()},
                    {"message",
                     "Authorization Key not confirm."}
                }
            );

            return;
        }

        std::string projectname =
            body.value(
                "projectname",
                ""
            );

        std::string name =
            body.value(
                "name",
                ""
            );

        ServerInfo server;

        if (
            !UE5ServMan.getServer(
                projectname,
                name,
                server
            )
        )
        {
            jsonResponse(
                response,
                {
                    {"message",
                     "Server name not found. Check name."}
                }
            );

            return;
        }

        /*
            Turn off the actual PID.
        */

        if (server.pid > 0)
        {
            ProcessManager::stopServer(
                server.pid
            );
        }

        /*
            The server is removed from the list.
        */

        ServerInfo deleted;

        UE5ServMan.deleteServer(
            projectname,
            name,
            deleted
        );

        jsonResponse(
            response,
            {
                {"message",
                 "Server deleted."}
            }
        );
    }
    catch (const std::exception& error)
    {
        jsonResponse(
            response,
            {
                {"message",
                 error.what()}
            },
            400
        );
    }
}

/*
============================================================
CORS
============================================================
*/

void setupCors(
    httplib::Server& app)
{
    app.set_default_headers({
        {
            "Access-Control-Allow-Origin",
            "*"
        },
        {
            "Access-Control-Allow-Methods",
            "GET, POST, OPTIONS, PUT, PATCH, DELETE"
        },
        {
            "Access-Control-Allow-Headers",
            "X-Requested-With, Content-Type, Authorization"
        }
    });

    app.Options(
        ".*",
        [](const httplib::Request&,
           httplib::Response& response)
        {
            response.status = 204;
        }
    );
}

/*
============================================================
MAIN
============================================================
*/

int main()
{
    std::cout
        << "===================================="
        << std::endl;

    std::cout
        << "        Server Manager"
        << std::endl;

    std::cout
        << "===================================="
        << std::endl;

    /*
        Create the build folders.
    */

    try
    {
        fs::create_directories(
            Config::SERVER_BUILD_DIRECTORY
        );

        fs::create_directories(
            Config::WEB_DIRECTORY
        );
    }
    catch (const std::exception& error)
    {
        std::cerr
            << "Directory error: "
            << error.what()
            << std::endl;

        return 1;
    }

    /*
        HTTP server.
    */

    httplib::Server app;

    /*
        Request body maks 500 MB.
    */

    app.set_payload_max_length(
        500ULL * 1024ULL * 1024ULL
    );

    setupCors(app);

    /*
        ========================================================
        API ROUTES
        ========================================================
    */

    app.Post(
        "/api/prepare_build",
        prepareBuild
    );

    app.Post(
        "/api/create_server",
        createServer
    );

    app.Post(
        "/api/server_list",
        serverList
    );

    app.Post(
        "/api/server_data",
        serverData
    );

    app.Post(
        "/api/delete_server",
        deleteServer
    );

    /*
        ========================================================
        STATIC WEB
        ========================================================
    */

    if (
        fs::exists(
            Config::WEB_DIRECTORY
        )
    )
    {
        app.set_mount_point(
            "/",
            Config::WEB_DIRECTORY
        );

        /*
            SPA fallback.
        */

        app.set_error_handler(
            [](const httplib::Request& request,
               httplib::Response& response)
            {
                if (
                    request.method == "GET" &&
                    response.status == 404
                )
                {
                    fs::path indexPath =
                        fs::path(
                            Config::WEB_DIRECTORY
                        ) /
                        "index.html";

                    if (fs::exists(indexPath))
                    {
                        std::ifstream file(
                            indexPath,
                            std::ios::binary
                        );

                        if (file)
                        {
                            std::string content(
                                (
                                    std::istreambuf_iterator<char>(
                                        file
                                    )
                                ),
                                std::istreambuf_iterator<char>()
                            );

                            response.status = 200;

                            response.set_content(
                                content,
                                "text/html"
                            );
                        }
                    }
                }
            }
        );
    }

    /*
        ========================================================
        START
        ========================================================
    */

    std::cout
        << "HTTP + Game Server:"
        << " "
        << Config::HTTP_HOST
        << ":"
        << Config::HTTP_PORT
        << std::endl;

    if (
        !app.listen(
            Config::HTTP_HOST,
            Config::HTTP_PORT
        )
    )
    {
        std::cerr
            << "Failed to start HTTP server."
            << std::endl;

        return 1;
    }

    return 0;
}