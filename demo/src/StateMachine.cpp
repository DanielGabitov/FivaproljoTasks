#include "StateMachine.h"

StateMachine::StateMachine() : current_state(
        Utilities::GameState::MENU_NUM_OF_PLAYERS) {}//Game starts from choosing number of players (for now at least)

Utilities::GameState StateMachine::get_cur_state() {
    return current_state;
}

Utilities::GameState StateMachine::get_prev_state() {
    return prev_state;
}

void StateMachine::one_player_mode() {
    prev_state = current_state;
    current_state = Utilities::GameState::PLAYER_SELECTION;
    current_num = Utilities::GameNumOfPlayers::SINGLE_PLAYER;
    emit set_num_of_players(Utilities::GameNumOfPlayers::SINGLE_PLAYER);
}

void StateMachine::two_players_mode() {
    prev_state = current_state;
    current_state = Utilities::GameState::PLAYER_SELECTION;
    current_num = Utilities::GameNumOfPlayers::TWO_PLAYERS;
    emit set_inet_type();
    emit begin_connection_upd();
    emit set_num_of_players(Utilities::GameNumOfPlayers::TWO_PLAYERS);
}

void StateMachine::exit_game() {
    current_state = Utilities::GameState::EXIT;
    emit set_exit_game();
}

void StateMachine::start_level() {
    prev_state = current_state;
    current_state = Utilities::GameState::LEVEL_ON;
    mode = Utilities::GameMode::COLOR_CRAZE;
    emit set_level(mode);
}

void StateMachine::end_level() {
    prev_state = current_state;
    current_state = Utilities::GameState::LEVEL_STATISTICS;
    emit set_end_level();
}


void StateMachine::close_window() {
    emit set_close_window(prev_state);
}

void StateMachine::menu_mode() {
    prev_state = current_state;
    current_state = Utilities::GameState::MENU_MODE;
    emit set_menu_mode();
}
