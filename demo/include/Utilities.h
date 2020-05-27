#pragma once

namespace Utilities {

    enum class Direction {
        RIGHT,
        LEFT,
        UP,
        DOWN,
        UNKNOWN
    };

    enum class Color {
        NONE,
        GREEN,
        YELLOW
    };

    enum class GameState {
        INITIALIZING,
        MENU_NUM_OF_PLAYERS,
        MENU_MODE,
        PLAYER_SELECTION,
        MENU_INET_TYPE,
        LEVEL_ON,
        LEVEL_STATISTICS,
        EXIT
    };

    enum class GameNumOfPlayers {
        SINGLE_PLAYER,
        TWO_PLAYERS,
    };

    enum class GameMode {
        COLOR_CRAZE // that's how this mode is originally called in move or die
    };

    enum class PacketType {
        INIT
    };

    enum class ButtonPurpose {
        CUSTOMIZE,
        READY,
        BACK
    };


    enum class ConnectionResult {
        SERVER_SUCCESS,
        SERVER_FAILURE,
        CLIENT_SUCCESS,
        CLIENT_FAILURE
    };

    enum class LevelType {
        DEMO,
        LEVEL1,
        LEVEL2,
        HSE
    };

    enum class BlockType {
        EMPTY, DEFAULT
    };

    enum class ConnectionType{
        SERVER, CLIENT, OFFLINE
    };

} // end namespace Utilities

namespace KeyPresserUtility {

    enum class ManipulatorType {
        PLAYER,
        MENU
    };

} // end namespace KeyPresserUtilitie

#define unused(p) (void)p