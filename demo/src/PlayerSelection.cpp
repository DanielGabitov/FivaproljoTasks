#include "PlayerSelection.h"

/*
 * Окно в котором можно выбрать текстуры для персонажа. Когда все игроки готовы, запускает уровень.
 */

PlayerSelection::PlayerSelection(Scene *scene) : scene(scene) {}

PlayerSelection::~PlayerSelection() {}

/*
 * Receives amount of players from starting menu (Controller::runGame).
 */
void PlayerSelection::add_players(const std::vector<Player *> &new_players) {
    num_of_players = new_players.size();
    players = new_players;
}

void PlayerSelection::run_player_selection() {
    init_window();
}

void PlayerSelection::init_window() {
 //   scene->add_background("images/black_color.jpg");
    scene->add_background("images/background1.jpg");
    set_text();
    set_images();
    set_buttons();
}

/*
 * Sets text in the window: for each present player -  "Player @num_of_player".
 * Sets "EMPTY SLOT" for remaining players.
 */
void PlayerSelection::set_text() {
    character_selection = new QLabel();
    character_selection->setText("Character Selection");
    character_selection->move(50, 50);
    QFont cs_f("Arial", 30, QFont::Bold);
    character_selection->setFont(cs_f);
    character_selection->setAttribute(Qt::WA_TranslucentBackground);
    scene->add_text(character_selection);

    QFont pn_f("Arial", 25);
    for (int i = 0; i < MAX_NUM_OF_PLAYERS; i++) {
        player_num.push_back(new QLabel());
        if (i < num_of_players) {
            switch (i) {
                case 0:
                    player_num[i]->setText("Player 1");
                    break;
                case 1:
                    player_num[i]->setText("Player 2");
                    break;
                case 2:
                    player_num[i]->setText("Player 3");
                    break;
                case 3:
                    player_num[i]->setText("Player 4");
                    break;
            }
        } else {
            player_num[i]->setText("EMPTY SLOT");
        }
        player_num[i]->move(75 + 300 * i, 210);
        player_num[i]->setFont(pn_f);
        player_num[i]->setAttribute(Qt::WA_TranslucentBackground);
        scene->add_text(player_num[i]);
    }
}

/*
 * Sets textures for players to choose their skin from.
 */
void PlayerSelection::set_images() {
    for (int i = 0; i < num_of_players; i++) {
        player_textures.push_back(new QGraphicsPixmapItem());
        player_textures[i]->setPixmap(QPixmap(available_skins[0]).scaled(200, 200));
        player_textures_index.push_back(0);
        player_textures[i]->setPos(75 + 300 * i, 250);
        scene->add_pixmap(player_textures[i]);
    }
}

/*
 * After all players are ready, hides the player selection window.
 */
void PlayerSelection::clear_player_selection() {
    for (auto i : player_textures) {
        i->hide();
    }
    character_selection->hide();
    for (auto i : player_num) {
        i->hide();
    }
    //players.clear();
    player_textures_index.clear();
    for (int i = 0; i < MAX_NUM_OF_PLAYERS; i++) {
        for (auto button : buttons_player[i]) {
            button->hide();
        }
    }
}

/*
 * Sets buttons according to the amount of players.
 */
void PlayerSelection::set_buttons() {
    for (int i = 0; i < num_of_players; i++) {
        set_buttons_player(i, 75 + 300 * i);
    }
}

/*
 * Changes texture of selected player when button @Customize is clicked.
 */
void PlayerSelection::change_image_impl(int player_number) {
    player_textures_index[player_number - 1] = (player_textures_index[player_number - 1] + 1) % available_skins.size();
    player_textures[player_number - 1]->setPixmap(
            QPixmap(available_skins[
                            player_textures_index[player_number - 1]]).
                    scaled(200, 200));
}


void PlayerSelection::change_image(int player_number) {
    if (sendClick_) {
        sendClick_(Utilities::ButtonPurpose::CUSTOMIZE);
        std::cout << "sendClick_ int change_image, id: " << id_ << std::endl;
    }
    change_image_impl(player_number);
}

/*
 * Increases the number of players that are ready, when one clicks @Ready button.
 * When all players are ready, starts level.
 */
void PlayerSelection::increase_ready_num_impl(int player_number) {
    if (++num_of_ready == num_of_players) {
        for (int i = 0; i < num_of_players; i++) {
            players[i]->set_player_skin(available_skins[player_textures_index[i]]);
        }
        clear_player_selection();
        emit start_level();
    } else {
        customize_player[player_number - 1]->hide();
        ready_player[player_number - 1]->hide();
        back_player[player_number - 1]->show();
    }
}

void PlayerSelection::increase_ready_num(int player_number) {
    if (sendClick_) {
        sendClick_(Utilities::ButtonPurpose::READY);
    }
    increase_ready_num_impl(player_number);
}
/*
 * Decreases the number of players that are ready, when @Back is clicked.
 */
void PlayerSelection::decrease_ready_num_impl(int player_number) {
    --num_of_ready;
    customize_player[player_number - 1]->show();
    ready_player[player_number - 1]->show();
    back_player[player_number - 1]->hide();
}

void PlayerSelection::decrease_ready_num(int player_number) {
    if (sendClick_) {
        sendClick_(Utilities::ButtonPurpose::BACK);
    }
    decrease_ready_num_impl(player_number);
}

/*
 * Sets buttons for one player:
 * @Customize - change the current chosen texture of player
 * @Ready - shows that player is ready to start the level
 * @Back - after @Ready is clicked, if player changes mind, they can go back
 */
void PlayerSelection::set_buttons_player(int index, int xPos) {
    customize_player[index] = new QPushButton("Customize");
    customize_player[index]->setGeometry(QRect(QPoint(xPos, 460),
                                               QSize(200, 50)));
    connect(customize_player[index], &QPushButton::clicked, this, [=] { change_image(index + 1); });

    ready_player[index] = new QPushButton("Ready");
    ready_player[index]->setGeometry(QRect(QPoint(xPos, 520),
                                           QSize(200, 50)));
    connect(ready_player[index], &QPushButton::clicked, this, [=] { increase_ready_num(index + 1); });

    back_player[index] = new QPushButton("Back");
    back_player[index]->setGeometry(QRect(QPoint(xPos, 580),
                                          QSize(200, 50)));
    connect(back_player[index], &QPushButton::clicked, this, [=] { decrease_ready_num(index + 1); });
    back_player[index]->hide();

    buttons_player[index].push_back(customize_player[index]);
    buttons_player[index].push_back(ready_player[index]);
    buttons_player[index].push_back(back_player[index]);

    scene->add_button(customize_player[index]);
    scene->add_button(ready_player[index]);
    scene->add_button(back_player[index]);
}


PlayerSelectionRemoteClicker::PlayerSelectionRemoteClicker(PlayerSelection &ps, Inet::InternetConnection *inet)
        : ps_(ps), inetConnection_(inet) {
    using namespace std::placeholders;
    inetConnection_->setClick(std::bind(&PlayerSelectionRemoteClicker::click, this, _1, _2));
    ps_.sendClick_ = std::bind(&PlayerSelectionRemoteClicker::sendClick, this, _1);
    ps_.id_ = inetConnection_->id();
}

void PlayerSelectionRemoteClicker::click(int id, Utilities::ButtonPurpose purpose) {
    if (purpose == Utilities::ButtonPurpose::CUSTOMIZE) {
        ps_.change_image_impl(id + 1);
    } else if (purpose == Utilities::ButtonPurpose::READY) {
        ps_.increase_ready_num_impl(id + 1);
    } else if (purpose == Utilities::ButtonPurpose::BACK) {
        ps_.decrease_ready_num_impl(id + 1);
    }
}

void PlayerSelectionRemoteClicker::sendClick(Utilities::ButtonPurpose purpose) {
    inetConnection_->send(inetConnection_->buildPacket(purpose).data());
}

