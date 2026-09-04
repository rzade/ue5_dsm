# UE5 DSM

UE5 DSM (Unreal Engine 5 Dedicated Server Manager) is a lightweight C++17 application for managing Unreal Engine 5 Dedicated Servers.

It provides a simple HTTP API for:

Managing Dedicated Server builds
Creating server instances
Deleting server instances
Listing available servers
Preparing newly uploaded server builds
Requirements

UE5 DSM requires an Ubuntu/Debian-based Linux system.

## System Dependencies

Install the required packages:

sudo apt update
```
sudo apt install -y \
    build-essential \
    cmake \
    git \
    unzip
```

An installed Unreal Engine 5 version is also required for building and packaging the Unreal Engine Dedicated Server.

Build
1. Clone the Repository
git clone https://github.com/rzade/ue5_dsm.git
cd ue5_dsm

2. Create the Linux Build Directory
mkdir build-linux
cd build-linux

3. Configure the Project

Configure CMake in Release mode:

cmake .. -DCMAKE_BUILD_TYPE=Release

4. Build
cmake --build . -j1


After a successful build, the executable will be available at:

build-linux/UE5ServMan

5. Run
./UE5ServMan

Build Output

The expected build structure is:

build-linux/
├── UE5ServMan
├── ServerBuild/
└── build/

Directory / File    Description
UE5ServMan  Main UE5 DSM executable
ServerBuild/    Packaged Unreal Engine Dedicated Server builds
build/  CMake and other build-related files
Configuration

Before building UE5 DSM, configure your server address and authorization key in:

src/config.h


Set the required values:

#define HTTP_HOST "YOUR_SERVER_IP"
#define SERVER_IP "YOUR_SERVER_IP"
#define AUTH_KEY "YOUR_AUTHORIZATION_KEY"


Replace:

YOUR_SERVER_IP — Your UE5 DSM server IP address.
YOUR_AUTHORIZATION_KEY — Your API authorization key.

[!WARNING]
Never commit your real authorization key to a public GitHub repository.

Keep your private configuration out of version control or use a separate local configuration file.

API

UE5 DSM provides an HTTP API on port 1204.

All API requests use the:

POST


method.

Base URL
http://your_server_ip:1204

Authorization

All API requests require an authorization field.

Example:

{
    "authorization": "YOUR_AUTHORIZATION_KEY"
}


[!IMPORTANT]
Do not commit your real authorization key to the repository.

Endpoints
Get Server List

Returns the available servers for a project.

Endpoint
POST /api/server_list

Request
{
    "authorization": "YOUR_AUTHORIZATION_KEY",
    "projectname": "DSMFPSServer"
}

Create Server

Creates a new Unreal Engine Dedicated Server instance.

Endpoint
POST /api/create_server

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
Delete Server

Deletes an existing server instance.

Endpoint
POST /api/delete_server

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
Prepare Build

Prepares a newly uploaded Dedicated Server build.

Endpoint
POST /api/prepare_build

Request
{
    "authorization": "YOUR_AUTHORIZATION_KEY",
    "buildname": "LinuxServer"
}


[!IMPORTANT]
After uploading a new packaged server ZIP, /api/prepare_build must be called immediately.

Recommended Workflow
1. Package the Unreal Engine Dedicated Server
2. Create the server ZIP
3. Upload the new ZIP
4. Call /api/prepare_build
5. The new build is ready to be used


Example:

POST /api/prepare_build

{
    "authorization": "YOUR_AUTHORIZATION_KEY",
    "buildname": "LinuxServer"
}


The buildname must match the uploaded server build.

API Workflow

A typical server management workflow looks like this:

┌─────────────────────┐
│  Upload Server ZIP  │
└──────────┬──────────┘
           │
           ▼
┌─────────────────────┐
│   prepare_build     │
└──────────┬──────────┘
           │
           ▼
┌─────────────────────┐
│    create_server    │
└──────────┬──────────┘
           │
           ▼
┌─────────────────────┐
│    Server Running   │
└──────────┬──────────┘
           │
           ▼
┌─────────────────────┐
│    delete_server    │
└─────────────────────┘

Unreal Engine Project Configuration

Before using UE5 DSM with your Unreal Engine project, add the following configuration to:

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
MaxPlayers  Maximum number of players allowed in the game session
Network Configuration
[/Script/Engine.GameNetworkManager]
TotalNetBandwidth=600000
MaxDynamicBandwidth=80000
MinDynamicBandwidth=4000


These values configure the Unreal Engine network bandwidth limits for the Dedicated Server.

UE5 DSM Configuration
[/Script/DSMProject.DSMGameInstance]
ServerAddress=YOUR_SERVER_IP:1204
AuthKey=YOUR_AUTHORIZATION_KEY

Setting Description
ServerAddress   Address and port of the UE5 DSM API server
AuthKey Authorization key used when communicating with the UE5 DSM API
Example
[/Script/DSMProject.DSMGameInstance]
ServerAddress=164.92.158.177:1204
AuthKey=YOUR_AUTHORIZATION_KEY


[!WARNING]
Never commit your real AuthKey to a public GitHub repository.

Use a placeholder in source-controlled configuration files and provide the real key through your deployment configuration or another secure mechanism.

After changing DefaultGame.ini, you must rebuild and package your Unreal Engine Dedicated Server before uploading the new server ZIP to UE5 DSM.

Recommended Deployment Flow
┌──────────────────────────┐
│    Unreal Engine Project │
└────────────┬─────────────┘
             │
             ▼
┌──────────────────────────┐
│   Config/DefaultGame.ini │
└────────────┬─────────────┘
             │
             ▼
┌──────────────────────────┐
│ Package Dedicated Server │
└────────────┬─────────────┘
             │
             ▼
┌──────────────────────────┐
│    Create Server ZIP     │
└────────────┬─────────────┘
             │
             ▼
┌──────────────────────────┐
│      Upload ZIP          │
│       to UE5 DSM         │
└────────────┬─────────────┘
             │
             ▼
┌──────────────────────────┐
│   POST /api/prepare_build│
└────────────┬─────────────┘
             │
             ▼
┌──────────────────────────┐
│   POST /api/create_server│
└────────────┬─────────────┘
             │
             ▼
┌──────────────────────────┐
│ Dedicated Server Running │
└──────────────────────────┘


To check the available servers at any time:

POST /api/server_list

Running UE5 DSM
Start

To run UE5 DSM in the background and save the output to a log file:

nohup ./UE5ServMan > ue5_serv_man.log 2>&1 &

Stop

To stop the running UE5 DSM process:

pkill -f UE5ServMan

View Logs
tail -f ue5_serv_man.log

Dependencies

The project automatically downloads its external dependencies using CMake FetchContent.

Dependency  Version
cpp-httplib
    v0.18.3
nlohmann/json
    v3.11.3

No manual installation of these libraries is required.

C++ Standard

UE5 DSM uses C++17.

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

Security

Please keep the following values private:

AUTH_KEY
AuthKey


Do not store real authorization keys in:

Public GitHub repositories
config.h
DefaultGame.ini
Public documentation
Git history

Use environment variables, deployment configuration, or another secure secret-management mechanism whenever possible.

License

Copyright © 2026 Rahman Qadirzade

This project is licensed under the MIT License.

See the LICENSE file for the complete license text.

Author

Rahman Qadirzade