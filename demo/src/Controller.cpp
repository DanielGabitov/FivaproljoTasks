#include <vector>
#include <QApplication>
#include <QTimer>
#include <iostream>
#include <unistd.h>
#include "Controller.h"

/*
 * Из контроллера осуществляется управление игровым процессом.
 * Изначально запускается окно с выбором количества игроков (runGame). При выборе посылается сигнал StateMachine::set_num_of_players,
 * после чего запускается окно с выбором скинов игроков Controller::set_num_of_players_for_lvl.
 * Когда все игроки готовы, (PlayerSelection::start_level -> StateMachine::start_level ->
 * StateMachine::set_level -> Controller::run_level) начинается уровень, который пока длится 10 секунд.
 * По окончании level_durance выводится статистика по уровню : Controller::end_level.
 * В появившемся окне статистики можно выбрать следующий режим игры.
 */

ConnectionUpdater::ConnectionUpdater(Inet::InternetConnection *&ic) : inetConnection_(ic) {
    std::cout << ic << std::endl;;
}

Controller::~Controller() {

}


void ConnectionUpdater::commit() {
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(advance()));
    timer->start(10);
    timer->setInterval(16);
}

void ConnectionUpdater::advance() {
    if (inetConnection_) {
        inetConnection_->receive();
    } else {
        // std::cout << "NOOO" << std::endl;
    }
}

Controller::Controller(int argc, char *argv[])
        : app(argc, argv), scene_(new Scene()), key_presser_(new KeyPresser(internetConnection)),
          state_machine_(new StateMachine()), model_(new Model(scene_, state_machine_)),
          menu_(new Menu(scene_, state_machine_)), player_selection(new PlayerSelection(scene_)),
          connUpd_(new ConnectionUpdater(internetConnection)) {
    key_presser_->setFixedSize(QSize(scene_->get_width(), scene_->get_height()));
    scene_->addWidget(key_presser_);
    connect(state_machine_, &StateMachine::set_num_of_players, this, &Controller::set_num_of_players_for_lvl);
    connect(state_machine_, &StateMachine::set_exit_game, this, &Controller::exit_game);
    connect(player_selection, &PlayerSelection::start_level, state_machine_, &StateMachine::start_level);
    connect(state_machine_, &StateMachine::set_level, this, &Controller::run_level);
    connect(state_machine_, &StateMachine::set_end_level, this, &Controller::end_level);
    connect(state_machine_, &StateMachine::set_client_connection, this,
            [=] { connect_client(menu_->get_server_port()); });
    connect(state_machine_, &StateMachine::server_connection_success, state_machine_, &StateMachine::two_players_mode);
    connect(state_machine_, &StateMachine::client_connection_success, state_machine_, &StateMachine::two_players_mode);
    connect(state_machine_, &StateMachine::set_server_connection, this, &Controller::connect_server);
    connect(state_machine_, &StateMachine::begin_connection_upd, connUpd_, &ConnectionUpdater::commit);
}

static int connect(Inet::Client *client) { // ждет ответного пакета
    int attempts = 100;
    while (!client->id()) {
        if (attempts-- == 0) {
            return 0;
        }
        client->receive();
        usleep(160);
    }
    return client->id();
}

void Controller::connect_server() {
    internetConnection = new Inet::Server();
    connection_type_ = Utilities::ConnectionType::SERVER;
    remoteClicker_ = new PlayerSelectionRemoteClicker(*player_selection, internetConnection);
    //if(smtn went wrong) return false;
    state_machine_->connection_result(Utilities::ConnectionResult::SERVER_SUCCESS);
}

void Controller::connect_client(unsigned short serverPort) {
    internetConnection = new Inet::Client();
    connection_type_ = Utilities::ConnectionType::CLIENT;
    reinterpret_cast<Inet::Client *>(internetConnection)->setUpdatePositions(std::bind(&Controller::update_model_positions,
                                                                                       this, std::placeholders::_1));

    while (!localId) {
        internetConnection->connect({127, 0, 0, 1, serverPort});
        localId = ::connect(static_cast<Inet::Client *>(internetConnection));
        if (!localId) {
//            delete internetConnection;
            state_machine_->connection_result(Utilities::ConnectionResult::CLIENT_FAILURE);
            return;
            // std::cout << "Could not connect. ";
        }
        remoteClicker_ = new PlayerSelectionRemoteClicker(*player_selection, internetConnection);
    }
    state_machine_->connection_result(Utilities::ConnectionResult::CLIENT_SUCCESS);
}

/*
 * Запускает стартововый экран с выбором количества персонажей; get_cur_state = MENU_NUM_OF_PLAYERS
 */
int Controller::run_game() {
    scene_->show();
    menu_->run_menu();
    return app.exec();
}

/*
 * Добавляет нужное количество персонажей и запускает окно с выбором текстур для персонажей
 */
void Controller::set_num_of_players_for_lvl(Utilities::GameNumOfPlayers num) {
    switch (num) {
        case Utilities::GameNumOfPlayers::SINGLE_PLAYER:
            players_.push_back(new Player(Utilities::Color::GREEN));
            key_presser_->add_players(players_[0]);
            break;
        case Utilities::GameNumOfPlayers::TWO_PLAYERS:
            players_.push_back(new Player(Utilities::Color::GREEN));
            players_.push_back(new Player(Utilities::Color::YELLOW));
            key_presser_->add_players(players_[localId], players_[1 - localId]); // потом тупо случаи как-то разбирать
            break;
    }
    run_player_selection();
}

/*
 * Прячет предыдущее окно меню. Открывается новое окно с выбором текстур.
 */
void Controller::run_player_selection() {
    menu_->clear_menu();
    player_selection->add_players(players_);
    //only for 2 players; DOES NOT WORK
    if (internetConnection && internetConnection->id() == 1) {
        std::swap(players_[1], players_[0]);
    }
    player_selection->add_players(players_);
    if (internetConnection && internetConnection->id() == 1) {
        std::swap(players_[1], players_[0]);
    }
    player_selection->run_player_selection();
}

/*
 * Когда все игроки готовы, начинает уровень.
 */
void Controller::run_level(Utilities::GameMode mode) {
    if (state_machine_->get_prev_state() == Utilities::GameState::PLAYER_SELECTION) {
        player_selection->clear_player_selection();
       // menu_->clear_menu();
    } else if (state_machine_->get_prev_state() == Utilities::GameState::LEVEL_STATISTICS) {
        model_->clear_level();
    }
    menu_->clear_menu();
    model_->add_players(players_);
    model_->set_statistics();
    model_->make_new_level();
    level_durance = new QTimer(this);

    if (connection_type_ == Utilities::ConnectionType::SERVER){
        server_pos_updater = new QTimer(this);
        connect(server_pos_updater, SIGNAL(timeout()), this, SLOT(update_clients_positions()));
        server_pos_updater->start(1000 / 6);
    }

    connect(level_durance, SIGNAL(timeout()), state_machine_, SLOT(end_level()));
    connect(level_durance, SIGNAL(timeout()), model_, SLOT(stop_advance_scene()));
    level_durance->start(7000 * 5);
}

/*
 * По истечении времени останавливает таймер, останавливает игроков, выводит статистику.
 */
void Controller::end_level() {
    level_durance->stop();
    if (server_pos_updater)
        server_pos_updater->stop();
    model_->show_statistics();
}

/*
/*
 * При выборе выхода из игры заканчивает процессы, выходит из приложения.
 */
void Controller::exit_game() {
    app.quit();
}

void Controller::update_clients_positions() {
    std::vector<float> position_data;
    for (auto &player : players_){
        QPointF pos = player->pos();

        position_data.push_back(pos.x());
        position_data.push_back(pos.y());
    }
    qDebug() << "HERE!";
    reinterpret_cast<Inet::Server*>(internetConnection)->update_positions(position_data);
    server_pos_updater->start(1000 / 3);
}

void Controller::update_model_positions(const std::vector<float> &positions) {
    int j = 0;
    for (auto &player : players_){
        float x = positions[j++];
        float y = positions[j++];

        player->setPos(QPointF(x, y));
    }
}
