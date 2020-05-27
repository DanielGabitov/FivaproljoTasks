#include <vector>
#include <QApplication>
#include <QKeyEvent>
#include <QDebug>
#include <QGraphicsScene>
#include <QTimer>
#include "Controller.h"

using namespace Inet;

KeyPresser::KeyPresser(InternetConnection *& inetConnection)
        : inetConnection_(inetConnection) {
    setWindowOpacity(0.0);
    setFocus();
}

void KeyPresser::keyPressEvent(QKeyEvent *event) {
    for (auto manip : manipulators_) {
        if (manip->active()) {
            manip->press(static_cast<Qt::Key>(event->key()));
        }
    }
    if (inetConnection_) {
        if (static_cast<Qt::Key>(event->key()) == Qt::Key_W) {
            inetConnection_->send(InternetConnection::buildPacket(true, Utilities::Direction::UP).data());
        } else if (static_cast<Qt::Key>(event->key()) == Qt::Key_A) {
            inetConnection_->send(InternetConnection::buildPacket(true, Utilities::Direction::LEFT).data());
        } else if (static_cast<Qt::Key>(event->key()) == Qt::Key_S) {
            inetConnection_->send(InternetConnection::buildPacket(true, Utilities::Direction::DOWN).data());
        } else if (static_cast<Qt::Key>(event->key()) == Qt::Key_D) {
            inetConnection_->send(InternetConnection::buildPacket(true, Utilities::Direction::RIGHT).data());
        }
    }
}

void KeyPresser::keyReleaseEvent(QKeyEvent *event) {
    for (auto manip : manipulators_) {
        if (manip->active()) {
            manip->release(static_cast<Qt::Key>(event->key()));
        }
    }
    if (inetConnection_) {
        if (static_cast<Qt::Key>(event->key()) == Qt::Key_W) {
            inetConnection_->send(InternetConnection::buildPacket(false, Utilities::Direction::UP).data());
        } else if (static_cast<Qt::Key>(event->key()) == Qt::Key_A) {
            inetConnection_->send(InternetConnection::buildPacket(false, Utilities::Direction::LEFT).data());
        } else if (static_cast<Qt::Key>(event->key()) == Qt::Key_S) {
            inetConnection_->send(InternetConnection::buildPacket(false, Utilities::Direction::DOWN).data());
        } else if (static_cast<Qt::Key>(event->key()) == Qt::Key_D) {
            inetConnection_->send(InternetConnection::buildPacket(false, Utilities::Direction::RIGHT).data());
        }
    }
}

void KeyPresser::add_players(Player *player1, Player *player2) {
    manipulators_.push_back(new PlayerManipulator(player1));
    if (player2) {
        if (inetConnection_) {
            inetManipulators_.push_back(new InetPlayerManipulator(player2, inetConnection_));
        } else {
            manipulators_.push_back(new PlayerManipulator(player2, Qt::Key_T, Qt::Key_F, Qt::Key_G, Qt::Key_H));
        }
    }
}

void KeyPresser::remove_players() {
    for (size_t i = 0; i < manipulators_.size(); i++) {
        if (manipulators_[i]->type() == KeyPresserUtility::ManipulatorType::PLAYER) {
            delete manipulators_[i];
            std::swap(manipulators_[i], manipulators_[manipulators_.size() - 1]);
            manipulators_.pop_back();
        }
    }
}

void KeyPresser::activate(KeyPresserUtility::ManipulatorType type) {
    for (auto manip : manipulators_) {
        if (manip->type() == type) {
            manip->activate();
        }
    }
}

void KeyPresser::deactivate(KeyPresserUtility::ManipulatorType type) {
    for (auto manip : manipulators_) {
        if (manip->type() == type) {
            manip->deactivate();
        }
    }
}

KeyPresser::Key::Key(Qt::Key qt_name)
        : qt_name_(qt_name)
{}

KeyPresser::Key::Key(Utilities::Direction dir)
        : dir_(dir)
{}

KeyPresser::Key::operator Qt::Key() const {
    return qt_name_;
}

KeyPresser::Key::operator Utilities::Direction() const {
    return dir_;
}

bool KeyPresser::Key::is_pressed() const {
    return is_pressed_;
}

void KeyPresser::Key::press() {
    is_pressed_ = true;
}

void KeyPresser::Key::release() {
    is_pressed_ = false;
}

KeyPresser::Manipulator::~Manipulator() {}

KeyPresser::Manipulator::Manipulator(bool is_active, KeyPresserUtility::ManipulatorType manip_type)
        : is_active_(is_active)
        , manip_type_(manip_type)
{}

bool KeyPresser::Manipulator::active() const {
    return is_active_;
}

void KeyPresser::Manipulator::activate() {
    is_active_ = true;
}

void KeyPresser::Manipulator::deactivate() {
    is_active_ = false;
}

KeyPresserUtility::ManipulatorType KeyPresser::Manipulator::type() const {
    return manip_type_;
}

/*KeyPresser::MenuManipulator::MenuManipulator(Menu *menu)
    : Manipulator(true, KeyPresserUtility::ManipulatorType::MENU)
    , menu_(menu)
    , UP(Qt::Key_W)
    , DOWN(Qt::Key_S)
    , RETURN(Qt::Key_Return)
{}

KeyPresser::MenuManipulator::~MenuManipulator() {}

void KeyPresser::MenuManipulator::press(Qt::Key k) {
    if (k == UP) {
    menu_->go_up();
    } else if (k == DOWN) {
    menu_->go_down();
    } else if (k == RETURN) {
    menu_->press();
    }
}

void KeyPresser::MenuManipulator::release(Qt::Key k) {}
*/
KeyPresser::PlayerManipulator::PlayerManipulator(Player *player, Qt::Key up_key, Qt::Key left_key,
                                                 Qt::Key down_key, Qt::Key right_key)
        : Manipulator(true, KeyPresserUtility::ManipulatorType::PLAYER)
        , player_(player)
        , UP(up_key)
        , LEFT(left_key)
        , DOWN(down_key)
        , RIGHT(right_key)
{}

KeyPresser::PlayerManipulator::~PlayerManipulator() {}

void KeyPresser::PlayerManipulator::press(Qt::Key k) {
    if (k == UP) {
        UP.press();
        player_->start_jumping();
    } else if (k == LEFT) {
        LEFT.press();
        player_->moving = true;
        player_->direction = Utilities::Direction::LEFT;
        player_->change_skin_direction();
    } else if (k == RIGHT) {
        RIGHT.press();
        player_->moving = true;
        player_->direction = Utilities::Direction::RIGHT;
        player_->change_skin_direction();
    }
}

void KeyPresser::PlayerManipulator::release(Qt::Key k) {
    if (k == UP) {
        UP.release();
    } else if (k == LEFT) {
        LEFT.release();
        if (RIGHT.is_pressed()) {
            player_->moving = true;
            player_->direction = Utilities::Direction::RIGHT;
            player_->change_skin_direction();
        } else {
            player_->moving = false;
            player_->direction = Utilities::Direction::UNKNOWN;
        }
    } else if (k == RIGHT) {
        RIGHT.release();
        if (LEFT.is_pressed()) {
            player_->moving = true;
            player_->direction = Utilities::Direction::LEFT;
            player_->change_skin_direction();
        } else {
            player_->moving = false;
            player_->direction = Utilities::Direction::UNKNOWN;
        }
    }
}

KeyPresser::InetPlayerManipulator::InetPlayerManipulator(Player *player, InternetConnection *inetConnection)
        : player_(player)
        , inetConnection_(inetConnection) {
    inetConnection_->setPress(std::bind(&KeyPresser::InetPlayerManipulator::press,
                                        this, std::placeholders::_1));
    inetConnection_->setRelease(std::bind(&KeyPresser::InetPlayerManipulator::release,
                                          this, std::placeholders::_1));
}

void KeyPresser::InetPlayerManipulator::press(Utilities::Direction dir) {
    if (dir == UP) {
        UP.press();
        player_->start_jumping();
    } else if (dir == LEFT) {
        LEFT.press();
        player_->moving = true;
        player_->direction = Utilities::Direction::LEFT;
        player_->change_skin_direction();
    } else if (dir == RIGHT) {
        RIGHT.press();
        player_->moving = true;
        player_->direction = Utilities::Direction::RIGHT;
        player_->change_skin_direction();
    }
}

void KeyPresser::InetPlayerManipulator::release(Utilities::Direction dir) {
    if (dir == UP) {
        UP.release();
    } else if (dir == LEFT) {
        LEFT.release();
        if (RIGHT.is_pressed()) {
            player_->moving = true;
            player_->direction = Utilities::Direction::RIGHT;
            player_->change_skin_direction();
        } else {
            player_->moving = false;
            player_->direction = Utilities::Direction::UNKNOWN;
        }
    } else if (dir == RIGHT) {
        RIGHT.release();
        if (LEFT.is_pressed()) {
            player_->moving = true;
            player_->direction = Utilities::Direction::LEFT;
            player_->change_skin_direction();
        } else {
            player_->moving = false;
            player_->direction = Utilities::Direction::UNKNOWN;
        }
    }
}

//========

// KeyPresserHelper::KeyPresserHelper(KeyPresser *key_presser)
//     : key_presser_(key_presser)
// {}

// void KeyPresserHelper::call_activate(KeyPresserUtility::ManipulatorType type) {
//     key_presser_->activate(type);
// }

// void KeyPresserHelper::call_deactivate(KeyPresserUtility::ManipulatorType type) {
//     key_presser_->deactivate(type);
// }

// std::function<void(KeyPresserUtility::ManipulatorType)> KeyPresserHelper::get_activate() {
//     return std::bind(&KeyPresserHelper::call_activate, *this, std::placeholders::_1);
// }

// std::function<void(KeyPresserUtility::ManipulatorType)> KeyPresserHelper::get_deactivate() {
//     return std::bind(&KeyPresserHelper::call_deactivate, *this, std::placeholders::_1);
// }