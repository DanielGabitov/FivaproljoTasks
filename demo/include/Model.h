#ifndef DEMO_MODEL_H
#define DEMO_MODEL_H

#include <QGraphicsPixmapItem>
#include <QGraphicsView>
#include <QWidget>
#include "Scene.h"
#include "Player.h"
#include <vector>


class Model : public QObject {
Q_OBJECT
public:
    Model(QWidget *parent = 0);

  //  ~Model();

    //explicit Model(std::vector<Player *> &players);
    void add_players(std::vector<Player *> &players);

    void make_new_level(Scene *gs);

    void advance_players();

private slots:

    void advance_scene();

private:
    bool game_on;
    Scene *game_scene;
    std::vector<Player *> players_;
    QTimer *engine;
};

#endif //DEMO_MODEL_H