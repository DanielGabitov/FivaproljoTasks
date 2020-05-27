#include <QFile>
#include <random>
#include <ctime>
#include <fstream>
#include "Scene.h"

Scene::Scene(QWidget *parent)
        : scene(new QGraphicsScene()), level_blocks() {

    scene->setSceneRect(0, 0, 1250, 700);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setScene(scene);
    srand(std::time(nullptr));
    resize(1250, 700);
}

Scene::~Scene() {
    setScene(nullptr);
    scene->clear();
    delete (scene);
}

namespace {
    QString get_level_file_name(Utilities::LevelType level_type) {
        switch (level_type) {
            case Utilities::LevelType::DEMO:
                return QString("levels/demo.json");
            case Utilities::LevelType::LEVEL1:
                return QString("levels/Level1.json");
            case Utilities::LevelType::LEVEL2:
                return QString("levels/Level2.json");
            case Utilities::LevelType::HSE:
                return QString("levels/Level_HSE.json");
            default:
                qDebug() << "LevelType couldn't be matched to file_name. Aborting.";
                assert(false);
        }
    }


    QString get_block_filepath(Utilities::BlockType block_type) {
        switch (block_type) {
            case Utilities::BlockType::DEFAULT:
                return QString("images/1block.jpg");
            default:
                qDebug() << "Couldn't match BLOCK_TYPE to FILE_PATH. Aborting.";
                assert(false);
        }
    }

    Utilities::BlockType convert_json_id(int id) {
        switch (id) {
            case 1:
                return Utilities::BlockType::DEFAULT;
            case 0:
                return Utilities::BlockType::EMPTY;
            default:
                qDebug() << "Block type couldn't be matched to JSON id. Aborting.";
                assert(false);
        }
    }
} // Anonymous namespace ENDS.


void Scene::print_block(Utilities::BlockType block_type, QPoint position, int block_size) {

    if (block_type == Utilities::BlockType::EMPTY) {
        return;
    }

    level_blocks.push_back(new Block(position, get_block_filepath(block_type), block_size));
    scene->addItem(level_blocks.back());
}


void Scene::print_level(Utilities::LevelType level_type) {

    QString file_name = get_level_file_name(level_type);
    QFile inFile(file_name);
    inFile.open(QIODevice::ReadOnly | QIODevice::Text);

    if (!inFile.isOpen()) {
        assert(inFile.isOpen());
    }

    QByteArray data = inFile.readAll();
    inFile.close();

    QJsonParseError errorPtr;
    QJsonDocument doc = QJsonDocument::fromJson(data, &errorPtr);
    if (doc.isNull()) {
        qDebug() << "Parse failed";
    }

    QJsonObject level_data = doc.object();

    int block_width = level_data.value("block_width").toInt();
    int block_height = level_data.value("block_height").toInt();

    assert(block_width == block_height); // Supported only.

    int block_size = block_width;

    int level_width = level_data.value("level_width").toInt();
    int level_height = level_data.value("level_height").toInt();

    int screen_width = level_data.value("screen_width").toInt();
    int screen_height = level_data.value("screen_height").toInt();

    if (screen_width != 1250 or screen_height != 700) {
        qDebug() << "We do not support format with given screen_width and screen_height.\n Aborting";
        assert(false);
    }

    QJsonArray blocks_ids = level_data.value("blocks").toArray();

    for (int i = 0; i < level_height; i++) {
        for (int j = 0; j < level_width; j++) {
            Utilities::BlockType block_type = convert_json_id(blocks_ids[i * level_width + j].toInt());
            print_block(block_type, QPoint(block_size * j, block_size * i), block_size);
        }
    }
}

void Scene::add_background(const QString &image) {
    scene->setBackgroundBrush(QBrush(QImage(image)));
}

void Scene::add_players(const std::vector<Player *> &players) {
    for (int i = 0; i < players.size(); i++) {

        if (players.size() == 2) {
            players[0]->setPos(50, scene->height() - 50 - players[0]->boundingRect().height());
            players[1]->setPos(1150, scene->height() - 50 - players[0]->boundingRect().height());
            scene->addItem(players[0]);
            scene->addItem(players[1]);
            return;
        }

        players[i]->setPos(50, scene->height() - 50 - players[i]->boundingRect().height());
        players[i]->previous_position = players[i]->pos();
        scene->addItem(players[i]);
    }
}

void Scene::addWidget(KeyPresser *pPresser) {
    scene->addWidget((QWidget *) pPresser);
}

void Scene::add_text(QLabel *text) {
    scene->addWidget(text);
}

void Scene::add_button(QPushButton *button) {
    QGraphicsProxyWidget *widgetItem = scene->addWidget(button);
}

void Scene::add_pixmap(QGraphicsPixmapItem *item) {
    scene->addItem(item);
}

//changes size of window
void Scene::resizeEvent(QResizeEvent *event) {
    QGraphicsView::resizeEvent(event);
    fitInView(this->sceneRect(), Qt::KeepAspectRatio);
}

void Scene::add_qgrectitem(QGraphicsRectItem *item) {
    scene->addItem(item);
}


Utilities::LevelType Scene::get_random_level_type() {

    int number = rand() % AVAILABLE_LEVELS_AMOUNT + 1;

    qDebug() << number;

    switch (number) {
        case 1:
            return Utilities::LevelType::DEMO;
        case 2:
            return Utilities::LevelType::LEVEL1;
        case 3:
            return Utilities::LevelType::LEVEL2;
        case 4:
            return Utilities::LevelType::HSE;
        default:
            qDebug() << "default";
            return Utilities::LevelType::DEMO;
    }
}

void Scene::clear_blocks() {
    for (auto &i : level_blocks) {
        remove_item(i);
    }
    level_blocks.clear();
}

void Scene::clear_players(std::vector<Player *> players) {
    for (auto &player : players) {
        remove_item(player);
    }
    players.clear();
}

void Scene::remove_item(QGraphicsItem *item) {
    scene->removeItem(item);
}

void Scene::add_qlineedit(QLineEdit *box) {
    scene->addWidget(box);
}

