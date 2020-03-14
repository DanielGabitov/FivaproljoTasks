#pragma once

#include <vector>
#include <QWidget>
#include <QtGui>
#include <QGraphicsScene>
#include <QTimer>
#include <QGraphicsItem>
#include "Player.h"
#include "Scene.h"
#include "Model.h"

class KeyPresser : public QWidget {
public:
    explicit KeyPresser(Player *player, QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
private:
    class PlayerManipulator_ {
    public:
	PlayerManipulator_(Player* player);
    	void press(Qt::Key k);
    	void release(Qt::Key k);
    private:
    	class Key_ {
    	public:
	    Key_() = delete;
	    explicit Key_(Qt::Key name);
    	    operator Qt::Key() const;
    	    bool is_pressed() const;
    	    void press();
    	    void release();
	    
    	private:
    	    Qt::Key qt_name_;
    	    bool is_pressed_;
    	} W, A, S, D;
        Player *player_;
    } player_manipulator_;
};

class Controller {
public:
    Controller();
    void runGame();
private:
    std::vector<Player *> players_;
    Scene *scene_ = nullptr;
    Model *model_ = nullptr;
    KeyPresser *key_presser_ = nullptr;
};
