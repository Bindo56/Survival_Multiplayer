# Survival Multiplayer setup

## Requirements

- Unreal Engine 5.6 (the version in `Survival_Multiplayer.uproject`).
- On Windows, Visual Studio 2022 with the C++ game development components listed in `.vsconfig`.
- Git and enough free disk space for Unreal to generate build and shader caches.

## Open the project

1. Clone this repository: `git clone https://github.com/Bindo56/Survival_Multiplayer.git`.
2. Open `Survival_Multiplayer.uproject` in Unreal Engine 5.6. If Unreal asks to rebuild missing C++ modules, allow it. The first open can take time while shaders and derived data are generated.
3. If the C++ project does not build from the editor, right-click the `.uproject` file, generate Visual Studio project files, open the generated solution, and build the `Survival_MultiplayerEditor` target for `Development Editor` / `Win64`.
4. The project starts in `/Game/Maps/Lobby_level`, as configured in `Config/DefaultEngine.ini`.

The repository includes `Config`, `Content`, `Plugins/MultiplayerSessions`, and `Source`. Unreal-generated `Binaries`, `DerivedDataCache`, `Intermediate`, and `Saved` folders are intentionally not required from Git.

## Test multiplayer

1. In the Unreal editor, open the lobby map and set Play In Editor to two players, using a listen server for a local gameplay check.
2. Use the project's session UI to create and join a session. For online Steam session tests, run Steam on each test machine with a separate account and launch standalone game instances.
3. `Config/DefaultEngine.ini` enables `OnlineSubsystemSteam` and `SteamSockets` and currently uses Steam's test App ID `480`. Replace that with your own App ID before production release.

Session discovery and connectivity depend on the online subsystem and network environment. A local Play In Editor test is useful for replication checks; use separate machines/accounts for a full Steam session test.

## Keep local files private

Do not commit local credentials, packaging keys, or generated Unreal build folders. If Unreal changes project settings while opening the project, review the diff before committing.
