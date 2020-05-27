#include "Model.h"
#include "Player.h"
#include <QTimer>

Model::Model(Scene *scene, StateMachine *state_machine) : game_scene(scene), state_machine(state_machine) {
    game_just_ended = false;
    game_on = true;
    engine = new QTimer(this);
    QObject::connect(engine, SIGNAL(timeout()), this, SLOT(advance_scene()));
    engine->start(10);
    engine->setInterval(10);
}


void Model::make_new_level() { //TODO - find other way to load game
    level_made = true, game_on = true;
    game_scene->add_background("images/background.jpg");

    Utilities::LevelType level_type = game_scene->get_random_level_type();

    game_scene->print_level(level_type);
    game_scene->add_players(players_);
    start_timer();
    game_scene->show();
}

void Model::advance_scene() {
    if (!game_on) {
        return;
    }
    advance_players();
}

void Model::stop_advance_scene() {
    engine->stop();
    game_on = false;
    qDebug() << "STOP";
}

void Model::advance_players() {
    for (auto player : players_) {
        player->previous_position = player->pos();
        if (player->moving) {
            if (player->direction == Utilities::Direction::LEFT) {
                player->setX(player->x() - player->hor_speed);
                if (!player->jumping and !player->falling) {
                    player->check_floor();
                }
            } else if (player->direction == Utilities::Direction::RIGHT) {
                player->setX(player->x() + player->hor_speed);
                if (!player->jumping and !player->falling) {
                    player->check_floor();
                }
            }
            solve_collisions(player);
        }

        if (player->jumping) {
            player->current_timer--;
            if (player->current_timer == 0){
                player->start_falling();
            }
            player->setY(player->y() - player->vert_speed);
            solve_collisions(player);
        }

        if (player->falling) {
            player->setY(player->y() + player->hor_speed);
            solve_collisions(player);
        }
    }
}

void Model::solve_collisions(Player *player) {
    bool revert = false;

    for (QGraphicsItem *item: player->collidingItems()) {

        if (Block *platform = dynamic_cast<Block *>(item)) {

            lvl_statistic->change_block_color(platform, player);
            platform->change_color(player->color);

            Utilities::Direction collision_dir = player->collision_direction(platform);
            if (collision_dir == Utilities::Direction::UNKNOWN) continue;
            if (collision_dir == Utilities::Direction::DOWN && player->falling) {
                player->object_on_which_moving = platform;
                player->stop_falling();
            }
            // case 2: touching an object while jumping
            if (collision_dir == Utilities::Direction::UP && player->jumping) {
                player->stop_jumping();
            }

            revert = true; // if we got here we need to go back

        }
    }

    if (revert) {
        player->setPos(player->previous_position);
    }
}

void Model::add_players(std::vector<Player *> &players) {
    if (level_made) return;
    players_ = players;
    for (auto player : players_) {
        qDebug() << game_scene->get_width() << game_scene->get_width() / 25;
        player->set_size(game_scene->get_width() / 25, game_scene->get_width() / 25 + 10);
    }
}

void Model::set_statistics() {
    if (!lvl_statistic)
        lvl_statistic = new LevelStatistics(players_, game_scene, state_machine);
    else {
        lvl_statistic->set_players(players_);
    }
}

void Model::clear_level() {
    game_scene->clear_blocks();
    game_scene->clear_players(players_);
}

void Model::start_timer() {
    engine->start(10);
    engine->setInterval(10);
}

void Model::show_statistics() {
    lvl_statistic->run_level_statistic();
}