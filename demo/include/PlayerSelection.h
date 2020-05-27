#pragma once

#include <QGraphicsView>
#include <QLabel>
#include <vector>
#include "Player.h"
#include "Scene.h"
#include "Server.h"
#include "Utilities.h"

/*
 * For player selection new window is created in the beginning of the game.
 *  When all skins are selected, this window gets hidden (use it later)
 *  If this works, maybe should do the same to menu(s).
 */

class PlayerSelection : public QObject {
Q_OBJECT;
public:
    PlayerSelection(Scene *scene);
    ~PlayerSelection() override;

    void add_players(const std::vector<Player *> &players);
    void run_player_selection();
    void clear_player_selection();
    void init_window();
    void set_text();
    void set_buttons();
    void set_images();
    void set_buttons_player(int index, int xPos);


public slots:
    void change_image(int player_number);
    void increase_ready_num(int player_number);
    void decrease_ready_num(int player_number);

signals:
    void start_level();

private:
    void change_image_impl(int player_number);
    void increase_ready_num_impl(int player_number);
    void decrease_ready_num_impl(int player_number);

    Scene *scene;
    bool initialized = false;
    static constexpr int MAX_NUM_OF_PLAYERS = 4;

    QLabel *character_selection;
    std::vector<QLabel *> player_num;

    std::vector<QPushButton *> buttons_player[MAX_NUM_OF_PLAYERS];
    QPushButton *customize_player[MAX_NUM_OF_PLAYERS] = { [0 ... MAX_NUM_OF_PLAYERS - 1] = nullptr };
    QPushButton *ready_player[MAX_NUM_OF_PLAYERS] = { [0 ... MAX_NUM_OF_PLAYERS - 1] = nullptr };
    QPushButton *back_player[MAX_NUM_OF_PLAYERS] = { [0 ... MAX_NUM_OF_PLAYERS - 1] = nullptr };

    std::vector<QString> available_skins{"images/demo_player.png", "images/demo_player_2.png"};
    std::vector<QGraphicsPixmapItem *> player_textures;
    std::vector<int> player_textures_index;
    int num_of_players = 0;
    int num_of_ready = 0;
    std::vector<Player *> players;

    std::function<void(Utilities::ButtonPurpose)> sendClick_;
    int id_ = -1;

    friend class PlayerSelectionRemoteClicker;
};

class PlayerSelectionRemoteClicker {
public:
    PlayerSelectionRemoteClicker(PlayerSelection &ps, Inet::InternetConnection *inet);

    void click(int id, Utilities::ButtonPurpose purpose);
    void sendClick(Utilities::ButtonPurpose purpose);

private:
    PlayerSelection &ps_;
    Inet::InternetConnection *inetConnection_;
};