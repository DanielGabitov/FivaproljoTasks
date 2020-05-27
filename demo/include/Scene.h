#pragma once

#include <QGraphicsScene>
#include <QTimer>
#include <QLabel>
#include <QPushButton>
#include <QAbstractAnimation>
#include <QLineEdit>
#include <QDebug>
#include <QGraphicsView>
#include <QWidget>

#include <QJsonValue>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

#include <QGraphicsScene>
#include "Block.h"
#include "Player.h"
#include "Background.h"
#include "MenuButton.h"

class KeyPresser;

class Scene : public QGraphicsView {
public:
    Scene(QWidget *parent = 0);

    ~Scene() override;

    void add_background(const QString &image);

    /* void View::resizeEvent(QResizeEvent *event) {
         QGraphicsView::resizeEvent(event);
         fitInView(this->sceneRect());
     }*/

    void add_platform(int x, int y, int amount, const QString &image, int block_width);

    void add_players(const std::vector<Player *> &players);

    // void add_button(Button *button);

    void add_pixmap(QGraphicsPixmapItem *item);

    void add_button(QPushButton *button);

    void addWidget(KeyPresser *pPresser);

    void add_text(QLabel *text);

    void add_qlineedit(QLineEdit *box);

    void add_qgrectitem(QGraphicsRectItem *item);

    int get_width() { return scene->width(); }

    int get_height() { return scene->height(); }

    //void add_item(QGraphicsItem *item);

    void resizeEvent(QResizeEvent *event) override;

    QGraphicsScene *scene;

    void print_level(Utilities::LevelType level_type);

    void print_block(Utilities::BlockType block_type, QPoint position, int block_size);

    Utilities::LevelType get_random_level_type();

    void remove_item(QGraphicsItem *item);

    void clear_blocks();

    void clear_players(std::vector<Player *> players);
private:
    const size_t AVAILABLE_LEVELS_AMOUNT = 4;
    Background *background = nullptr;
    std::vector<Block *> level_blocks;
};
