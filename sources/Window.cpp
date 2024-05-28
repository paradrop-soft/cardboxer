#include "../headers/Window.hpp"
#include <fstream>
#include <iostream>
#include <jsoncpp/json/json.h>

Window::Window(sf::Vector2f resolution, std::string title)
    : sf::RenderWindow(sf::VideoMode(resolution.x, resolution.y), title)
{
    // set resolution
    this->resolution = resolution;

    
    // set backGround
    this->backGround = new sf::RectangleShape(resolution);
    this->backGround->setFillColor(sf::Color::Black);
    this->backGround->setPosition(0, 0);
    
    this->loadLibrary();
    
    // create scene and view
    this->menu = new Menu();
    this->setCurrentScene(this->menu);

    // this->createMenu();
    // this->view  = new QGraphicsView();
    // // set view options
    // this->view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // this->view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // this->changeScene(this->menu); // set current scene, by default menu
    // this->view->setMinimumSize(this->resolution);
    // this->view->setSceneRect(0, 0, this->resolution.width() - 5,  this->resolution.height() - 5);
    // this->view->setBackgroundBrush(Qt::black);
    // this->view->show();
    // this->setWindowTitle("");
}

void Window::loop()
{
    this->drawScene();
    this->display();
}

void Window::drawScene()
{
    this->draw(*this->backGround);
    // this->menu->draw(*this);
    this->Scene->draw(*this);
    /* draw scene (Node) here*/
}

void Window::setCurrentScene(Node *node)
{
    this->Scene = node;
}
/*
 * UTILITY
 */

// void Window::changeResolution(QSize resolution)
// /* set new resolution */
// {
//     this->resolution = resolution;
//     this->view->setMinimumSize(this->resolution);
// }

// void Window::changeScene(QGraphicsScene *scene)
// /* change currently shown scene */
// {
//     this->view->setScene(scene);
// }

/*
 * INIT
 */

bool Window::loadLibrary()
/* load global library of cards for use during game */
{
    std::ifstream file("./cards/load_list.json", std::ifstream::binary);
    Json::Value list;
    if(file.is_open())
    {   
        file >> list;
        file.close();
    }
    else 
    {
        std::cerr<<"\nPanic! Couldn't open load list for cards! Can't procede\n!";
        return false;
    }
    for (auto f : list)
    {
        std::string path = "./cards/"+f.asString();
        // std::cerr<<path;
        file.open(path, std::ifstream::binary);
        Json::Value card;
        if(file.is_open())
        {
            file >> card;
            file.close();
            this->library.emplace_back(new Card(card));

        }
        else 
        {
            std::cerr<<"Can't open card file: "<<path<<std::endl;
        }
    }
    
    // TODO: Load cards from jsons as json objects into this->library (list of cards)
    return true;
}

// /*
//  * SIGNALS
//  */

// void Window::newGame()
// /* create new game and set it as current scene */
// {
//     this->game = new Game();
//     // TODO: Connect game's backToMenu signal to Window's slot
//     this->changeScene(this->game);
// }

// void Window::backToMenu()
// {
//     // TODO: Remove game's connection to Window's slots
//     this->changeScene(this->menu);
//     delete this->game;
// }
