#include <vector>
#include <QApplication>
#include <QKeyEvent>
#include <QDebug>
#include <QGraphicsScene>
#include <QTimer>
#include "Controller.h"

Controller::Controller()
    : scene_(nullptr)
    , model_(nullptr)
{}

void Controller::runGame() { // later - add loop
    players_.push_back(new Player());
    model_ = new Model();
    model_->add_players(players_);
    scene_ = new Scene();
    key_presser_ = new KeyPresser(players_[0]);
    key_presser_->setFixedSize(QSize(scene_->scene()->width(), scene_->scene()->height()));
    scene_->addWidget(key_presser_);
    model_->make_new_level(scene_);
}

KeyPresser::KeyPresser(Player *player, QWidget *parent)
    : player_manipulator_(player) {
    setWindowOpacity(0.0);
    setFocus();
}

void KeyPresser::keyPressEvent(QKeyEvent *event) {
// <<<<<<< HEAD
    if (!event->isAutoRepeat()) {
	player_manipulator_.press((Qt::Key)event->key()); //TODO cast
        qDebug() << "Pressed!";
// =======
//     if (event->key() == Qt::Key_D and !event->isAutoRepeat()) {
//         player_->moving = true;
//         player_->direction = Direction::RIGHT; // Right
//         qDebug() << "D pressed!";
//     } else if (event->key() == Qt::Key_A and !event->isAutoRepeat()) {
//         player_->moving = true;
//         player_->direction = Direction::LEFT; // Left
//         qDebug() << "A pressed!";
//     } else if (event->key() == Qt::Key_W and !event->isAutoRepeat()) {
//         player_->start_jumping();
//         qDebug() << "W pressed!";
// >>>>>>> upstream/demo
    }
}

void KeyPresser::keyReleaseEvent(QKeyEvent *event) {
// <<<<<<< HEAD
    player_manipulator_.release((Qt::Key)event->key()); //TODO cast
}

KeyPresser::PlayerManipulator_::PlayerManipulator_(Player* player)
    : player_(player)
    , W(Qt::Key_W)
    , D(Qt::Key_A)
    , A(Qt::Key_D)
    , S(Qt::Key_S)
{}

KeyPresser::PlayerManipulator_::Key_::Key_(Qt::Key qt_name)
    : qt_name_(qt_name)
    , is_pressed_(false)
{}

KeyPresser::PlayerManipulator_::Key_::operator Qt::Key() const {
    return qt_name_;
}

bool KeyPresser::PlayerManipulator_::Key_::is_pressed() const {
    return is_pressed_;
}

void KeyPresser::PlayerManipulator_::Key_::press() {
    is_pressed_ = true;
}

void KeyPresser::PlayerManipulator_::Key_::release() {
    is_pressed_ = false;
}

void KeyPresser::PlayerManipulator_::press(Qt::Key k) {
    if (k == W) {
	W.press();
        player_->start_jumping();
    } else if (k == A) {
	A.press();
	player_->moving = true;
	player_->direction = Direction::LEFT; // Left
    } else if (k == D) {
	D.press();
	player_->moving = true;
        player_->direction = Direction::RIGHT; // Right
    }
}

void KeyPresser::PlayerManipulator_::release(Qt::Key k) {
    if (k == W) {
	W.release();
    } else if (k == A) {
	A.release();
	if (D.is_pressed()) {
	    player_->moving = true;
	    player_->direction = Direction::RIGHT; // Right
	} else {
            player_->moving = false;
            player_->direction = Direction::UNKNOWN;
	}
    } else if (k == D) {
	D.release();
	if (A.is_pressed()) {
            player_->moving = true;
	    player_->direction = Direction::LEFT; // Left
	} else {
            player_->moving = false;
            player_->direction = Direction::UNKNOWN;
	}
// =======
//     if (event->key() == Qt::Key_A) {
//         if (player_->direction == Direction::LEFT){
//             player_->moving = true;
//             player_->direction = Direction::UNKNOWN;
//         }
//     } else if (event->key() == Qt::Key_D) {
//         if (player_->direction == Direction::RIGHT){
//             player_->moving = false;
//             player_->direction = Direction::UNKNOWN;
//         }
//     } else if (event->key() == Qt::Key_W) {
//         return;
// >>>>>>> upstream/demo
    }
}
