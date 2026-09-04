<div align="center">
UE5 DSM
Unreal Engine 5 Dedicated Server Manager

A lightweight C++17 server management application for deploying, preparing and managing Unreal Engine 5 Dedicated Servers.

<br>






<br>

Build • Prepare • Create • Manage • Delete

</div>
Overview

UE5 DSM (Unreal Engine 5 Dedicated Server Manager) is a lightweight C++17 application designed to simplify the management of Unreal Engine 5 Dedicated Servers.

UE5 DSM provides an HTTP API that allows external applications, tools and Unreal Engine projects to manage Dedicated Server instances programmatically.

What can UE5 DSM do?
📦 Upload and prepare Dedicated Server builds
🚀 Create Dedicated Server instances
🛑 Delete running server instances
📋 List available servers
🌐 Provide a simple HTTP API
🔐 Protect API requests with authorization
⚡ Lightweight C++17 implementation
🐧 Designed for Ubuntu/Debian-based Linux servers
Architecture

The general architecture looks like this:

                         ┌─────────────────────────┐
                         │   Unreal Engine Project  │
                         │                         │
                         │      DSMGameInstance    │
                         └────────────┬────────────┘
                                      │
                                      │ HTTP API
                                      ▼
                         ┌─────────────────────────┐
                         │        UE5 DSM          │
                         │                         │
                         │   Dedicated Server      │
                         │      Manager            │
                         └────────────┬────────────┘
                                      │
                    ┌─────────────────┼─────────────────┐
                    │                 │                 │
                    ▼                 ▼                 ▼
             ┌────────────┐    ┌────────────┐    ┌────────────┐
             │ Server ZIP │    │ Server     │    │ Server     │
             │   Builds   │    │ Instance 1 │    │ Instance 2 │
             └────────────┘    └────────────┘    └────────────┘


UE5 DSM acts as the management layer between your Unreal Engine project and the Dedicated Server instances.

Features
Server Management

UE5 DSM exposes a simple HTTP API for managing server instances.

Feature Endpoint    Description
Server List POST /api/server_list   List available servers
Create Server   POST /api/create_server Create a new server
Delete Server   POST /api/delete_server Delete a server
Prepare Build   POST /api/prepare_build Prepare an uploaded build
Requirements

UE5 DSM is designed for Ubuntu/Debian-based Linux systems.

System Requirements
Linux
C++17 compiler
CMake
Git
Unzip
Unreal Engine 5

Install Dependencies
sudo apt update

sudo apt install -y \
    build-essential \
    cmake \
    git \
    unzip


An installed Unreal Engine 5 version is required for building and packaging the Unreal Engine Dedicated Server.

Quick Start
1. Clone
git clone https://github.com/rzade/ue5_dsm.git
cd ue5_dsm

2. Configure

Edit:

src/config.h


Configure your server:

#define HTTP_HOST "YOUR_SERVER_IP"
#define SERVER_IP "YOUR_SERVER_IP"
#define AUTH_KEY "YOUR_AUTHORIZATION_KEY"

3. Build
mkdir build-linux
cd build-linux

cmake .. -DCMAKE_BUILD_TYPE=Release

cmake --build . -j1

4. Run
./UE5ServMan


UE5 DSM will start and expose its HTTP API on port:

1204

Build Output

After a successful build:

build-linux/
├── UE5ServMan
├── ServerBuild/
└── build/

Path    Description
UE5ServMan  Main executable
ServerBuild/    Packaged Dedicated Server builds
build/  CMake build files
Configuration
UE5 DSM Configuration

Edit:

src/config.h


Example:

#define HTTP_HOST "YOUR_SERVER_IP"
#define SERVER_IP "YOUR_SERVER_IP"
#define AUTH_KEY "YOUR_AUTHORIZATION_KEY"

Configuration Values
Variable    Description
HTTP_HOST   UE5 DSM HTTP server address
SERVER_IP   Dedicated Server IP address
AUTH_KEY    API authorization key

[!WARNING]
Never commit your real AUTH_KEY to a public GitHub repository.

Use a placeholder in source-controlled files and provide the real secret through a secure deployment mechanism.

API

UE5 DSM exposes an HTTP API on port 1204.

Base URL
http://YOUR_SERVER_IP:1204


All endpoints use the POST method.

Authorization

Every API request requires an authorization field.

{
    "authorization": "YOUR_AUTHORIZATION_KEY"
}


[!IMPORTANT]
Keep your authorization key private.

API Reference
POST /api/server_list

Returns the available servers for a project.

Request
{
    "authorization": "YOUR_AUTHORIZATION_KEY",
    "projectname": "DSMFPSServer"
}

POST /api/create_server

Creates a new Unreal Engine Dedicated Server instance.

Request
{
    "authorization": "YOUR_AUTHORIZATION_KEY",
    "buildname": "LinuxServer",
    "projectname": "DSMFPSServer",
    "port": 7779,
    "name": "World_map",
    "mapname": "Map_a_WP",
    "maxplayers": 8
}

Parameters
Parameter   Type    Description
authorization   string  API authorization key
buildname   string  Dedicated Server build name
projectname string  Unreal Engine project name
port    integer Server game port
name    string  Server instance name
mapname string  Unreal Engine map name
maxplayers  integer Maximum number of players
POST /api/delete_server

Deletes an existing server instance.

Request
{
    "authorization": "YOUR_AUTHORIZATION_KEY",
    "projectname": "DSMFPSServer",
    "name": "World_map"
}

Parameters
Parameter   Type    Description
authorization   string  API authorization key
projectname string  Unreal Engine project name
name    string  Server instance name
POST /api/prepare_build

Prepares a newly uploaded Dedicated Server build.

Request
{
    "authorization": "YOUR_AUTHORIZATION_KEY",
    "buildname": "LinuxServer"
}


[!IMPORTANT]
/api/prepare_build must be called immediately after uploading a new packaged server ZIP.

The buildname must match the uploaded server build.

Server Lifecycle

The typical server lifecycle is:

       ┌──────────────────────┐
       │   Dedicated Server   │
       │      ZIP Upload      │
       └──────────┬───────────┘
                  │
                  ▼
       ┌──────────────────────┐
       │   prepare_build      │
       └──────────┬───────────┘
                  │
                  ▼
       ┌──────────────────────┐
       │    create_server     │
       └──────────┬───────────┘
                  │
                  ▼
       ┌──────────────────────┐
       │   Server Running     │
       └──────────┬───────────┘
                  │
                  ▼
       ┌──────────────────────┐
       │    delete_server     │
       └──────────────────────┘

Unreal Engine Integration

UE5 DSM can be integrated directly into an Unreal Engine project.

Add the following configuration to:

Config/DefaultGame.ini

Complete Configuration
[/Script/Engine.GameSession]
bRequiresPushToTalk=true
MaxPlayers=100

[/Script/Engine.GameNetworkManager]
TotalNetBandwidth=600000
MaxDynamicBandwidth=80000
MinDynamicBandwidth=4000

[/Script/DSMProject.DSMGameInstance]
ServerAddress=YOUR_SERVER_IP:1204
AuthKey=YOUR_AUTHORIZATION_KEY

Game Session
[/Script/Engine.GameSession]
bRequiresPushToTalk=true
MaxPlayers=100

Setting Description
bRequiresPushToTalk Enables the Push-to-Talk requirement
MaxPlayers  Maximum number of players
Network Configuration
[/Script/Engine.GameNetworkManager]
TotalNetBandwidth=600000
MaxDynamicBandwidth=80000
MinDynamicBandwidth=4000


These settings configure Unreal Engine network bandwidth limits for the Dedicated Server.

UE5 DSM Configuration
[/Script/DSMProject.DSMGameInstance]
ServerAddress=YOUR_SERVER_IP:1204
AuthKey=YOUR_AUTHORIZATION_KEY

Setting Description
ServerAddress   UE5 DSM API server address and port
AuthKey API authorization key
Example
[/Script/DSMProject.DSMGameInstance]
ServerAddress=164.92.158.177:1204
AuthKey=YOUR_AUTHORIZATION_KEY


[!WARNING]
Do not commit your real AuthKey to GitHub.

Use a placeholder in DefaultGame.ini and provide the real key through your deployment configuration.

After changing DefaultGame.ini, rebuild and package your Unreal Engine Dedicated Server before uploading the new ZIP.

Deployment Workflow

The recommended deployment process is:

┌─────────────────────────┐
│ Unreal Engine Project   │
└────────────┬────────────┘
             │
             ▼
┌─────────────────────────┐
│ Config/DefaultGame.ini  │
└────────────┬────────────┘
             │
             ▼
┌─────────────────────────┐
│ Package Dedicated       │
│ Server                  │
└────────────┬────────────┘
             │
             ▼
┌─────────────────────────┐
│ Create Server ZIP       │
└────────────┬────────────┘
             │
             ▼
┌─────────────────────────┐
│ Upload ZIP to UE5 DSM   │
└────────────┬────────────┘
             │
             ▼
┌─────────────────────────┐
│ POST /api/prepare_build │
└────────────┬────────────┘
             │
             ▼
┌─────────────────────────┐
│ POST /api/create_server │
└────────────┬────────────┘
             │
             ▼
┌─────────────────────────┐
│ Dedicated Server Running│
└─────────────────────────┘

Running UE5 DSM
Start

Run UE5 DSM in the background:

nohup ./UE5ServMan > ue5_serv_man.log 2>&1 &

Stop
pkill -f UE5ServMan

View Logs
tail -f ue5_serv_man.log

Dependencies

External dependencies are automatically downloaded using CMake FetchContent.

Dependency  Version
cpp-httplib v0.18.3
nlohmann/json   v3.11.3

No manual installation is required.

C++ Standard

UE5 DSM uses C++17.

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

Project Structure

A simplified project structure:

ue5_dsm/
│
├── src/
│   ├── config.h
│   └── ...
│
├── CMakeLists.txt
├── LICENSE
└── README.md


After building:

ue5_dsm/
│
├── src/
│
├── build-linux/
│   ├── UE5ServMan
│   ├── ServerBuild/
│   └── build/
│
├── CMakeLists.txt
├── LICENSE
└── README.md

Security

UE5 DSM uses an authorization key to protect API requests.

Keep the following values private:

AUTH_KEY
AuthKey

Never store real credentials in:
❌ Public GitHub repositories
❌ config.h
❌ DefaultGame.ini
❌ README files
❌ Public documentation
❌ Git history
Recommended

Use:

Environment variables
Deployment secrets
Private configuration files
Secret management systems

[!CAUTION]
If an authorization key has already been committed to a public Git repository, consider it compromised and rotate it immediately.

License

Copyright © 2026 Rahman Qadirzade

This project is licensed under the MIT License.

See the LICENSE file for the complete license text.

Author

Rahman Qadirzade

<div align="center">
UE5 DSM

Unreal Engine 5 Dedicated Server Management

Made with C++17

</div>