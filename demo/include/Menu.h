#pragma once

#include <vector>
#include <functional>
#include <QGraphicsPixmapItem>
#include <QLineEdit>
#include <QObject>
#include <QPushButton>
#include <QLabel>
#include "Scene.h"
#include "StateMachine.h"
#include "MenuButton.h"
#include "Utilities.h"

class Menu : public QObject {
Q_OBJECT;

public:
    Menu(Scene *scene, StateMachine *state_machine);

    ~Menu() override;


    void init_buttons();

    void init_client_failure_window();

    void add_name_of_game();

    void set_inet_type_background();

    void set_connection_type_buttons();

    void set_client_window();

    unsigned short get_server_port() { return serverPort = server_port->text().toShort(); }

    //void set_client_window();

public slots:
    void run_menu();

    void init_inet_type_window();

    void clear_menu();

    void hide_num_of_players_menu_buttons();

    void init_client_window();

    void clear_client_window();

    void clear_inet_window();

    void clear_client_failure_window();

    void client_failure_window();

private:
    Scene *scene_;
    StateMachine *state_machine;
    QLabel *name_of_game;
    QPushButton *two_players;
    QPushButton *one_player;
    QPushButton *exit;
    QPushButton *paint_floor_mode;
    std::vector<QPushButton *> buttons_players;
    std::vector<QPushButton *> buttons_mode;
    QGraphicsPixmapItem *name;

    QGraphicsRectItem *inet_type_window_;
    QGraphicsRectItem *client_failure_window_;
    QLabel *insert_port;
    QLineEdit *server_port;
    QPushButton *client_button_;
    QPushButton *server_button_;
    QPushButton *local_button_;
    QPushButton *back_button_;
    QPushButton *connect_button_;
    QPushButton *cancel_button_;
    std::vector<QPushButton *> buttons_inet_;

    QLabel *failed_connection_to_server1;
    QLabel *failed_connection_to_server2;
    QPushButton *back_to_screen;

    unsigned short serverPort;
};

