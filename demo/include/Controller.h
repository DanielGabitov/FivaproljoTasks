#pragma once

#include <vector>
#include <functional>
#include <QWidget>
#include <QtGui>
#include <QObject>
#include <QGraphicsScene>
#include <QTimer>
#include <QGraphicsItem>
#include <iostream>
#include <unistd.h>
#include "Utilities.h"
#include "Player.h"
#include "Scene.h"
#include "Model.h"
#include "KeyPresser.h"
#include "Menu.h"
#include "StateMachine.h"
#include "Application.h"
#include "PlayerSelection.h"
#include "Server.h"


class ConnectionUpdater : public QObject {
    Q_OBJECT;
public:
    ConnectionUpdater(Inet::InternetConnection *&ic);

public slots:
    void commit();
    void advance();

private:
    QTimer *timer = new QTimer(this);
    Inet::InternetConnection *&inetConnection_;
};


class Controller : public QObject {
Q_OBJECT
public:
    Controller(int argc, char *argv[]);

    ~Controller() override;

    int run_game();

    void run_player_selection();

    void connect_server();

    void connect_client(unsigned short serverPort);

    void update_model_positions(const std::vector<float> &positions);

private slots:
//the name suggests that amount of players can be changed - for now this feature is not supported
    void set_num_of_players_for_lvl(Utilities::GameNumOfPlayers num);

    void exit_game();

    void end_level();

    void run_level(Utilities::GameMode mode);

    void set_inet_type();

    void update_clients_positions();


signals:

    void set_run_level();

    //void set_player_selection();

private:
    Application app;
    QTimer *level_durance = nullptr;
    StateMachine *state_machine_ = nullptr;
    Scene *scene_ = nullptr;
    Model *model_ = nullptr;
    Inet::InternetConnection *internetConnection = nullptr;
    KeyPresser *key_presser_ = nullptr;
    Menu *menu_ = nullptr;
    // KeyPresserHelper *key_presser_helper_ = nullptr;
    std::vector<Player *> players_;
    PlayerSelection *player_selection = nullptr;
    ConnectionUpdater *connUpd_ = nullptr;
    PlayerSelectionRemoteClicker *remoteClicker_ = nullptr;
    int localId = 0;

    Utilities::ConnectionType connection_type_ = Utilities::ConnectionType::OFFLINE;
    QTimer *server_pos_updater;
};
