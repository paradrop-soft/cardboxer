#pragma once
#include <list>
#include <SFML/Graphics.hpp>

class Node
/* Abstract base class for scene management */
{
private:
    sf::Transform transform;
    bool hidden = false;

    void hide();
    void reveal();

    virtual void onDraw(sf::RenderTarget& target, sf::Transform& transform) {}
    virtual int onButtonClick(sf::Vector2f mousePosition) {return 0;}
    virtual void onUpdate(sf::Vector2f mousePos) {}

public:
    std::list<Node*> children;

    Node();
    ~Node() {}
    void appendChild(Node *child);
    void removeChild(Node *child);
    // void moveChild(int i);
    // void updateChildren();
    
    
    void draw(sf::RenderTarget& target, sf::Transform& parentTransform);
    int buttonClick(sf::Vector2f mousePosition);
    void update(sf::Vector2f mousePos);
};