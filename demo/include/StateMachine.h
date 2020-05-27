#pragma once

#include <functional>
#include <QObject>
#include "Utilities.h"
#include "Scene.h"


class StateMachine : public QObject {
Q_OBJECT
public:
    StateMachine();

    Utilities::GameState get_cur_state();

    Utilities::GameState get_prev_state();

public slots:

    void one_player_mode();

    void two_players_mode();

    void connection_type();

    void connect_server();

    void connect_client();

    void connection_result(Utilities::ConnectionResult result);

    void menu_mode();

// will be called after choosing mode (choosing mode after player selection (perhaps(???) can be changed))
    void start_level();

    void end_level();

    void close_window();

    void exit_game();

signals:

    void set_num_of_players(Utilities::GameNumOfPlayers num);

    void set_level(Utilities::GameMode mode);

    void set_server_connection();

    void set_client_connection();

    void server_connection_success();

    void server_connection_failure();

    void client_connection_success();

    void client_connection_failure();

    void set_menu_mode();

    void set_exit_game();

    void set_end_level();

    void begin_connection_upd();

    void set_inet_type();

    void set_close_window(Utilities::GameState state);

private:
    Utilities::GameState current_state;
    Utilities::GameState prev_state;
    Utilities::GameNumOfPlayers current_num;
    Utilities::GameMode mode;
};
