# Survival_Shooter_Multiplayer

[Setup guide](SETUP.md)



https://github.com/user-attachments/assets/e175283b-68d9-43a4-b466-0d2aaaa7f418


Multiplayer Combat Prototype (Work in Progress)

This project is a multiplayer-focused combat prototype built in Unreal Engine. The core focus is on implementing a solid gameplay loop with properly structured networking architecture.

Currently implemented features include:

-  Weapon Rotation to Aim Direction(06 Mar 26)  

-  Added Crosshair (06 Mar 26)

-  Weapon hit FX (06 Mar 26)

 - Multiplayer session flow (create, join, seamless travel)

 - Replication and RPC-based communication

 - Character movement and animation blueprint integration

 - Weapon system with equip and replication logic

 - Aim offsets, IK (FABRIK), and turn-in-place

 - Crouching, strafing, leaning, and blendspaces

The project emphasizes clean multiplayer structure, correct network roles, and scalable architecture rather than visual polish at this stage.

## Setup

### Requirements

- Unreal Engine **5.6** (the version declared in `Survival_Multiplayer.uproject`).
- On Windows, Visual Studio 2022 with **Game development with C++** and a Windows SDK. The repository's `.vsconfig` lists the project components.
- Git to clone the repository. The `Content`, `Config`, `Source`, and `Plugins/MultiplayerSessions` folders are part of the project.

### Open and build

1. Clone this repository and open `Survival_Multiplayer.uproject` with Unreal Engine 5.6.
2. If Unreal asks to rebuild missing modules, allow it. If compilation fails, generate Visual Studio project files from the `.uproject`, open the solution, select **Development Editor** and **Win64**, and build.
3. Open the project in the Unreal Editor. Its configured startup map is `/Game/Maps/Lobby_level`.

### Try multiplayer

In the Editor, set the Play options to multiple players and test the lobby and session flow. The project enables `OnlineSubsystemSteam` and `SteamSockets`; testing Steam sessions requires a working Steam setup and separate test accounts. Editor local play can be used to inspect the gameplay without distributing a packaged build.

Generated folders such as `Binaries`, `Intermediate`, `Saved`, and `DerivedDataCache` are created by Unreal and do not need to be copied from another machine.
