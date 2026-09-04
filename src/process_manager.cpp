/*
 * Copyright (c) 2026 Rahman Qadirzade
 *
 * SPDX-License-Identifier: MIT
 */

#include "process_manager.h"

#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>

#include <iostream>
#include <vector>
#include <string>


bool ProcessManager::changeUser(
    const std::string& username)
{
    if (username.empty())
    {
        return true;
    }

    struct passwd* pw =
        getpwnam(username.c_str());

    if (!pw)
    {
        std::cerr
            << "User not found: "
            << username
            << std::endl;

        return false;
    }

    /*
        If the process is root,
        switch to the target user.
    */

    if (geteuid() == 0)
    {
        if (initgroups(
                username.c_str(),
                pw->pw_gid) != 0)
        {
            perror("initgroups");

            return false;
        }

        if (setgid(pw->pw_gid) != 0)
        {
            perror("setgid");

            return false;
        }

        if (setuid(pw->pw_uid) != 0)
        {
            perror("setuid");

            return false;
        }

        return true;
    }

    /*
        If it's not root, check if it's already running with the target user.
    */

    if (geteuid() == pw->pw_uid)
    {
        return true;
    }

    std::cerr
        << "Cannot switch to user: "
        << username
        << std::endl;

    return false;
}


bool ProcessManager::startServer(
    const std::string& scriptPath,
    const std::string& serverUser,
    const std::string& name,
    const std::string& mapname,
    int maxplayers,
    int port,
    int& outPid)
{
    outPid = -1;

    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork");

        return false;
    }


    /*
        ============================================================
        CHILD
        ============================================================
    */

    if (pid == 0)
    {
        /*
            New process group.

            So, during the delete process:

            kill(-pid, SIGTERM)

            we can close this process group.
        */

        if (setsid() == -1)
        {
            _exit(100);
        }


        /*
            User to be executed.
        */

        if (!changeUser(serverUser))
        {
            _exit(101);
        }


        /*
            Navigate to the folder where the script is located.

            For example:

            ./ServerBuild/Build1/MyProject.sh

            will be:

            ./ServerBuild/Build1

            the working directory.
        */

        std::string directory =
            scriptPath;

        std::size_t slash =
            directory.find_last_of('/');

        if (slash != std::string::npos)
        {
            directory =
                directory.substr(
                    0,
                    slash);

            if (directory.empty())
            {
                directory = ".";
            }

            if (chdir(directory.c_str()) != 0)
            {
                _exit(102);
            }

            scriptPath;
        }


        /*
            Script filename.
        */

        std::string scriptName =
            scriptPath;

        if (slash != std::string::npos)
        {
            scriptName =
                scriptPath.substr(
                    slash + 1);
        }


        /*
            Will be sent to Unreal script 
            argument: 

            DServerStart, 
            name=XXX, 
            mapname=XXX, 
            maxplayers=XXX, 
            DServerEnd, 
            -port=XXX
        */

        std::string arg1 =
            "DServerStart,name=" +
            name +
            ",mapname=" +
            mapname +
            ",maxplayers=" +
            std::to_string(maxplayers) +
            ",DServerEnd";

        std::string arg2 =
            "-port=" +
            std::to_string(port);


        /*
            Run the script with /bin/bash.

            No shell string interpolation.

            Values from the user are sent as separate argv files.
        */

        execl(
            "/bin/bash",
            "bash",
            scriptName.c_str(),
            arg1.c_str(),
            arg2.c_str(),
            static_cast<char*>(nullptr)
        );


        /*
            exec failed.
        */

        _exit(103);
    }


    /*
        ============================================================
        PARENT
        ============================================================
    */

    outPid =
        static_cast<int>(pid);

    std::cout
        << "Server started. PID: "
        << outPid
        << std::endl;

    return true;
}


bool ProcessManager::stopServer(
    int pid)
{
    if (pid <= 0)
    {
        return false;
    }

    /*
        First, SIGTERM to the process group.

        Negative PID = process group.
    */

    if (kill(
            -static_cast<pid_t>(pid),
            SIGTERM) != 0)
    {
        /*
            The process may have already closed.
        */

        if (errno != ESRCH)
        {
            perror("kill SIGTERM");

            return false;
        }
    }


    /*
        Wait a moment.

        Here we simply check for 5 seconds.
    */

    for (int i = 0; i < 50; ++i)
    {
        int status = 0;

        pid_t result =
            waitpid(
                static_cast<pid_t>(pid),
                &status,
                WNOHANG
            );

        if (result == pid)
        {
            return true;
        }

        if (result == -1)
        {
            if (errno == ECHILD)
            {
                return true;
            }

            break;
        }

        usleep(100000);
    }


    /*
        If it's still working, SIGKILL.
    */

    if (kill(
            -static_cast<pid_t>(pid),
            SIGKILL) != 0)
    {
        if (errno != ESRCH)
        {
            perror("kill SIGKILL");

            return false;
        }
    }


    /*
        Prevent zombies from forming.
    */

    int status = 0;

    waitpid(
        static_cast<pid_t>(pid),
        &status,
        0
    );

    return true;
}