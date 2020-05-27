#include "Menu.h"

Menu::Menu(Scene *scene, StateMachine *state_machine)
        : scene_(scene), state_machine(state_machine) {
    add_name_of_game();
    init_buttons();
    init_client_failure_window();

    connect(state_machine, &StateMachine::server_connection_success, this, &Menu::clear_inet_window);
    connect(state_machine, &StateMachine::client_connection_success, this, &Menu::clear_client_window);
    connect(state_machine, &StateMachine::client_connection_failure, this, &Menu::clear_client_window);
    connect(state_machine, &StateMachine::client_connection_failure, this, &Menu::client_failure_window);
}

Menu::~Menu() {
    scene_ = nullptr;
    state_machine = nullptr;
}

void Menu::add_name_of_game() {
    name = new QGraphicsPixmapItem;
    name->setPixmap(QPixmap("images/name.png").scaled(600, 80));
    name->setPos(340, 60);
    name->hide();
    scene_->add_pixmap(name);
}

void Menu::run_menu() {
    //for now skip as there is only one mode and also because i'm not sure how it should look like
    scene_->add_background("images/background1.jpg");
    name->show();
    for (auto button : buttons_players) {
        button->show();
    }
}

void Menu::init_buttons() {
    one_player = new QPushButton("One Player");
    one_player->setGeometry(QRect(QPoint(340, 400),
                                  QSize(600, 50)));
    buttons_players.push_back(one_player);
    connect(one_player, SIGNAL(clicked()), state_machine, SLOT(one_player_mode()));

    two_players = new QPushButton("Two Players");
    two_players->setGeometry(QRect(QPoint(340, 470),
                                   QSize(600, 50)));
    buttons_players.push_back(two_players);
    connect(two_players, SIGNAL(released()), state_machine, SLOT(connection_type()));

    exit = new QPushButton("Exit");
    exit->setGeometry(QRect(QPoint(340, 540),
                            QSize(600, 50)));
    buttons_players.push_back(exit);
    connect(exit, SIGNAL(clicked()), state_machine, SLOT(exit_game()));

    scene_->add_button(one_player);
    scene_->add_button(two_players);
    scene_->add_button(exit);
}

void Menu::run_inet_type_window() {
    hide_num_of_players_menu_buttons();
    scene_->add_background("images/background1.jpg");
    server_button_->show();
    client_button_->show();
    back_button_->show();
    name->show();
}

void Menu::run_client_window() {
    clear_inet_window();
    scene_->add_background("images/client_background.jpg");
    insert_port->show();
    server_port->show();
    connect_button_->show();
    cancel_button_->show();
}

void Menu::init_client_failure_window() {
    failed_connection_to_server1 = new QLabel("Failed to connect to server");
    QFont f("Arial", 15);
    failed_connection_to_server1->setFont(f);
    failed_connection_to_server1->setAttribute(Qt::WA_TranslucentBackground);
    failed_connection_to_server1->move(QPoint(340, 370));
    scene_->add_text(failed_connection_to_server1);
    failed_connection_to_server1->hide();

    failed_connection_to_server2 = new QLabel("Connection timed out");
    failed_connection_to_server2->setFont(f);
    failed_connection_to_server2->setFixedSize(500, 20);
    failed_connection_to_server2->setAttribute(Qt::WA_TranslucentBackground);
    failed_connection_to_server2->move(QPoint(360, 390));
    scene_->add_text(failed_connection_to_server2);
    failed_connection_to_server2->hide();

    back_to_screen = new QPushButton("Back to title screen");
    back_to_screen->setGeometry(QRect(QPoint(340, 450),
                                      QSize(600, 35)));
    connect(back_to_screen, SIGNAL(clicked()), this, SLOT(clear_client_failure_window()));
    connect(back_to_screen, SIGNAL(clicked()), this, SLOT(run_inet_type_window()));
    scene_->add_button(back_to_screen);
    back_to_screen->hide();
}


void Menu::init_client_window() {
    insert_port = new QLabel;
    insert_port->setText("Server name");
    QFont f("Arial", 15);
    insert_port->setFont(f);
    insert_port->setAttribute(Qt::WA_TranslucentBackground);
    insert_port->move(QPoint(340, 370));
    scene_->add_text(insert_port);
    insert_port->hide();

    server_port = new QLineEdit;
    server_port->setGeometry(340, 400,
                             600, 30);
    server_port->setFocus();
    scene_->add_qlineedit(server_port);

    server_port->hide();
    
    connect_button_ = new QPushButton("Done");
    connect_button_->setGeometry(QRect(QPoint(340, 450),
                                       QSize(600, 35)));
    connect(connect_button_, SIGNAL(clicked()), state_machine, SLOT(connect_client()));
    scene_->add_button(connect_button_);

    connect_button_->hide();
    cancel_button_ = new QPushButton("Cancel");
    cancel_button_->setGeometry(QRect(QPoint(340, 490),
                                      QSize(600, 35)));
    connect(cancel_button_, SIGNAL(clicked()), this, SLOT(clear_client_window()));
    connect(cancel_button_, SIGNAL(clicked()), this, SLOT(run_inet_type_window()));
    scene_->add_button(cancel_button_);
    cancel_button_->hide();
}

void Menu::client_failure_window() {
    failed_connection_to_server1->show();
    failed_connection_to_server2->show();
    back_to_screen->show();
}


void Menu::clear_inet_window() {
    name->hide();
    for (auto button : buttons_inet_) {
        button->hide();
    }
}

void Menu::clear_menu() {
    name->hide();
    hide_num_of_players_menu_buttons();
}

void Menu::hide_num_of_players_menu_buttons() {
    for (auto button : buttons_players) {
        button->hide();
    }
}

void Menu::clear_client_failure_window() {
    failed_connection_to_server1->hide();
    failed_connection_to_server2->hide();
    back_to_screen->hide();
}

void Menu::clear_client_window() {
    name->hide();
    insert_port->hide();
    server_port->hide();
    connect_button_->hide();
    cancel_button_->hide();
}


void Menu::init_connection_type_buttons() {
    server_button_ = new QPushButton("Server");
    server_button_->setGeometry(QRect(QPoint(340, 400),
                                      QSize(600, 50)));
    buttons_inet_.push_back(server_button_);

    server_button_->hide();

    connect(server_button_, SIGNAL(clicked()), state_machine, SLOT(connect_server()));

    client_button_ = new QPushButton("Client");
    client_button_->setGeometry(QRect(QPoint(340, 470),
                                      QSize(600, 50)));
    buttons_inet_.push_back(client_button_);

    client_button_->hide();
    connect(client_button_, SIGNAL(clicked()), this, SLOT(run_client_window()));

    back_button_ = new QPushButton("Back");
    back_button_->setGeometry(QRect(QPoint(340, 540),
                                    QSize(600, 50)));
    buttons_inet_.push_back(back_button_);
    connect(back_button_, SIGNAL(clicked()), this, SLOT(clear_inet_window()));
    connect(back_button_, SIGNAL(clicked()), this, SLOT(run_menu()));
    back_button_->hide();
    scene_->add_button(server_button_);
    scene_->add_button(client_button_);
    scene_->add_button(back_button_);
}
