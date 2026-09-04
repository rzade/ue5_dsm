UE5 DSM

UE5 DSM (Unreal Engine 5 Dedicated Server Manager) is a lightweight C++17 application for managing Unreal Engine 5 dedicated servers.

Requirements

Ubuntu/Debian-based Linux systems:

sudo apt update

sudo apt install -y \
    build-essential \
    cmake \
    git \
    unzip


An installed Unreal Engine version is also required for building and packaging the Unreal Engine Dedicated Server.

Build

Clone the repository:

git clone https://github.com/rzade/ue5_dsm.git
cd ue5_dsm


Create the Linux build directory:

mkdir build-linux
cd build-linux


Configure the project in Release mode:

cmake .. -DCMAKE_BUILD_TYPE=Release


Build the project:

cmake --build . -j1


After a successful build, the executable will be available at:

build-linux/UE5ServMan


Run the application:

./UE5ServMan

Build Output

The expected build structure is:

build-linux/
├── UE5ServMan
├── ServerBuild/
└── build/


UE5ServMan is the main executable.

ServerBuild/ contains the packaged Unreal Engine Dedicated Server.

build/ contains build-related files generated during the build process.

Configuration

Before building UE5 DSM, configure your server address and authorization key in config.h.

Open:

src/config.h


Set your own server IP and authorization key:

#define HTTP_HOST "YOUR_SERVER_IP"
#define SERVER_IP "YOUR_SERVER_IP"
#define AUTH_KEY "YOUR_AUTHORIZATION_KEY"


Replace:

YOUR_SERVER_IP with your UE5 DSM server IP address.
YOUR_AUTHORIZATION_KEY with your API authorization key.

Important: Do not commit your real authorization key to a public GitHub repository. Keep your private config.h out of version control or use a separate local configuration file.

API

The server manager provides a HTTP API on port 1204.

All API requests use the POST method.

Authorization

API requests require an authorization field.

For security reasons, do not commit your real authorization key to the repository.

Use your own authorization value:

{
    "authorization": "YOUR_AUTHORIZATION_KEY"
}

Get Server List

Returns the available servers for a project.

Endpoint:

POST http://your_server_ip:1204/api/server_list


Request:

{
    "authorization": "YOUR_AUTHORIZATION_KEY",
    "projectname": "DSMFPSServer"
}

Create Server

Creates a new Unreal Engine Dedicated Server instance.

Endpoint:

POST http://your_server_ip:1204/api/create_server


Request:

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

Endpoint:

POST http://your_server_ip:1204/api/delete_server


Request:

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

Prepares the newly uploaded Dedicated Server build.

Endpoint:

POST http://your_server_ip:1204/api/prepare_build


Request:

{
    "authorization": "YOUR_AUTHORIZATION_KEY",
    "buildname": "LinuxServer"
}

Important

After uploading a new packaged server ZIP, /api/prepare_build must be called immediately.

The recommended workflow is:

1. Package the Unreal Engine Dedicated Server
2. Create the server ZIP
3. Upload the new ZIP
4. Call /api/prepare_build
5. The new build is prepared and ready to be used


Example:

POST /api/prepare_build

{
    "authorization": "YOUR_AUTHORIZATION_KEY",
    "buildname": "LinuxServer"
}


The buildname must match the uploaded server build.

API Workflow

A typical server management workflow is:

                    ┌─────────────────────┐
                    │ Upload Server ZIP   │
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

Before using UE5 DSM with your Unreal Engine project, add the following configuration to your project's Config/DefaultGame.ini file.

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

Configuration
Game Session
[/Script/Engine.GameSession]
bRequiresPushToTalk=true
MaxPlayers=100

bRequiresPushToTalk enables the Push-to-Talk requirement.
MaxPlayers defines the maximum number of players allowed in the game session.
Network Configuration
[/Script/Engine.GameNetworkManager]
TotalNetBandwidth=600000
MaxDynamicBandwidth=80000
MinDynamicBandwidth=4000


These values configure the Unreal Engine network bandwidth limits for the dedicated server.

UE5 DSM Configuration
[/Script/DSMProject.DSMGameInstance]
ServerAddress=YOUR_SERVER_IP:1204
AuthKey=YOUR_AUTHORIZATION_KEY

ServerAddress is the address and port of the UE5 DSM API server.
AuthKey is the authorization key used when communicating with the UE5 DSM API.

For example:

[/Script/DSMProject.DSMGameInstance]
ServerAddress=164.92.158.177:1204
AuthKey=YOUR_AUTHORIZATION_KEY


Important: Do not commit your real AuthKey to a public GitHub repository. Use a placeholder in source-controlled configuration files and provide the real key through your deployment configuration or another secure mechanism.

After changing DefaultGame.ini, rebuild/package your Unreal Engine Dedicated Server before uploading the new server ZIP to UE5 DSM.

Recommended Deployment Flow
Unreal Engine Project
        │
        ▼
Config/DefaultGame.ini
        │
        ▼
Package Dedicated Server
        │
        ▼
Create Server ZIP
        │
        ▼
Upload ZIP to UE5 DSM
        │
        ▼
POST /api/prepare_build
        │
        ▼
POST /api/create_server
        │
        ▼
Dedicated Server Running

To check available servers at any time:

POST /api/server_list

USE

start stop -----------------------

nohup ./UE5ServMan > ue5_serv_man.log 2>&1 &

pkill -f UE5ServMan

Dependencies

The project automatically downloads its external dependencies using CMake FetchContent.

cpp-httplib

Version: v0.18.3

nlohmann/json

Version: v3.11.3

No manual installation of these libraries is required.

C++ Standard

The project uses C++17:

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

License

Copyright (c) 2026 Rahman Qadirzade

This project is licensed under the MIT License.

See the LICENSE file for the complete license text.

Author

Rahman Qadirzade