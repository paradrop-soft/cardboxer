#include "../headers/Enemy.hpp"
#include "../headers/utils.hpp"
#include <iostream>

Enemy::Enemy(int charakter) : Boxer()
{
    this->charakter = charakter;
    this->setHealth(10);
    this->setMaxHealth(10);
    this->setGuard(0);

    this->healthbar = new Healthbar(1);
    this->appendChild(this->healthbar);

    this->healthbar->setHealth(100);

    this->shield = new Shield(1);
    this->appendChild(this->shield);

    this->loadSprites();
    this->lastPlayed = this->deck.end();

}

Enemy::Enemy(Json::Value enemy, std::list<Card *>& library, int charakter)
    : Boxer()
{
    this->lastPlayed = this->deck.end();
    this->healthbar = new Healthbar(1);
    this->appendChild(this->healthbar);

    this->healthbar->setHealth(100);

    this->loadSprites();
    this->construct(charakter);

    this->charakter = (enemy["character"].isInt()) ? enemy["character"].asInt() : 0;

    if (enemy["name"].isNull()) { throw 10; }
    else
    {
        this->name.setString(enemy["name"].asString());
    }

    if (enemy["hp"].isNull()) { throw 20; }
    else 
    {
        this->setHealth(enemy["hp"].asInt());
        this->setMaxHealth(enemy["hp"].asInt());
    }

    if (enemy["texture"].isNull()) { throw 30; }
    /* load texture or throw error if not exists */
    else
    {
        this->texture = new sf::Texture();
        std::string path = "./res/textures/" + enemy["texture"]["file"].asString();
        if(!this->texture->loadFromFile(path))
            throw 31;
        this->setTexture(*this->texture);

        this->frames = enemy["texture"]["frames"].asInt();
        this->frameWidth = enemy["texture"]["frameWidth"].asInt();
        this->frameHeight = enemy["texture"]["frameHeight"].asInt();
        this->framePos = { enemy["texture"]["x"].asFloat(), enemy["texture"]["y"].asFloat() };
    }

    if (!enemy["actions"].isArray()) { throw 40; }
    else
    {
        int size = enemy["actions"].size();
        for (int i = 0; i < size; i++)
        {
            std::string name = enemy["actions"][i]["cardName"].asString();
            auto card = std::find_if(library.begin(), library.end(), [name](Card* _card){ return (*_card == name); });
            if (card != library.end())
            {
                this->deck.emplace_back(new Card(**card));
            }
            else
                throw 41;
            
            /* search through window->library for the card here */
            this->probabilities.emplace_back(enemy["actions"][i]["probability"].asFloat());
        }  
    } 
    
    auto it_d = this->deck.begin();
    auto it_p = this->probabilities.begin();
}    

Enemy::Enemy(Enemy *copy)
{
    this->probabilities = copy->probabilities;
    this->health = copy->health;
    this->maxHealth = copy->maxHealth;
    this->name = copy->name;
    this->charakter = copy->charakter;
    this->deck = copy->deck;
    
    this->setGuard(0);
    this->healthbar = new Healthbar(1);
    this->appendChild(this->healthbar);

    this->healthbar->setHealth(100);

    this->shield = new Shield(1);
    this->appendChild(this->shield);
    this->charakter = copy->charakter;
    this->loadSprites();
    this->setIdle();

    this->lastPlayed = this->deck.end();
}
  
void Enemy::construct(int charakter)
{
    this->healthbar = new Healthbar(1);
    this->appendChild(this->healthbar);
  
    this->charakter = charakter;

    this->loadSprites();

    this->setIdle();
}


void Enemy::loadSprites()
{
    this->sam_idleTexture.loadFromFile("./res/textures/Samurai/Idle.png");
    this->sam_attack1Texture.loadFromFile("./res/textures/Samurai/Attack_1.png");
    this->sam_attack2Texture.loadFromFile("./res/textures/Samurai/Attack_2.png");
    this->sam_attack3Texture.loadFromFile("./res/textures/Samurai/Attack_3.png");
    this->sam_deadTexture.loadFromFile("./res/textures/Samurai/Dead.png");
    this->sam_blockTexture.loadFromFile("./res/textures/Samurai/Shield.png");
    this->sam_hurtTexture.loadFromFile("./res/textures/Samurai/Hurt.png");

    this->shi_idleTexture.loadFromFile("./res/textures/Shinobi/Idle.png");
    this->shi_attack1Texture.loadFromFile("./res/textures/Shinobi/Attack_1.png");
    this->shi_attack2Texture.loadFromFile("./res/textures/Shinobi/Attack_2.png");
    this->shi_attack3Texture.loadFromFile("./res/textures/Shinobi/Attack_3.png");
    this->shi_deadTexture.loadFromFile("./res/textures/Shinobi/Dead.png");
    this->shi_blockTexture.loadFromFile("./res/textures/Shinobi/Shield.png");
    this->shi_hurtTexture.loadFromFile("./res/textures/Shinobi/Hurt.png");

    this->vam_idleTexture.loadFromFile("./res/textures/Vampire/Idle.png");
    this->vam_attack1Texture.loadFromFile("./res/textures/Vampire/Attack_1.png");
    this->vam_attack2Texture.loadFromFile("./res/textures/Vampire/Attack_2.png");
    this->vam_attack3Texture.loadFromFile("./res/textures/Vampire/Attack_3.png");
    this->vam_deadTexture.loadFromFile("./res/textures/Vampire/Dead.png");
    this->vam_blockTexture.loadFromFile("./res/textures/Vampire/Protect.png");
    this->vam_hurtTexture.loadFromFile("./res/textures/Vampire/Hurt.png");

    this->setScale(sf::Vector2f(-1.5, 1.5));
    this->setPosition(sf::Vector2f(600, 320));
}

void Enemy::onDraw(sf::RenderTarget &target, sf::Transform& transform) 
{
    target.draw(*this);
}



std::list<Card *>::iterator Enemy::chooseCard()
{
    int random = randomUniform(1, 100);
    int chance = 0;
    auto it_d = this->deck.begin();
    auto it_p = this->probabilities.begin();
    for (; it_d != this->deck.end() && it_p != this->probabilities.end(); ++it_d, ++it_p)
    {
        chance += (*it_p) * 100;
        if (it_d == this->lastPlayed)
        {
            it_d = this->deck.begin();
            it_p = this->probabilities.begin();
            random = randomUniform(1, 100);
        }
        if (chance >= random)
            return it_d;
    }
    return it_d;
}
  
Card Enemy::playCard()
{
    auto it = this->chooseCard();
    if (it == this->deck.end() || it == this->lastPlayed)
        it = this->chooseCard();
    this->lastPlayed = it;
    return **it;
}
  
std::string Enemy::getName() { return this->name.getString(); }

    

void Enemy::onUpdate(sf::Vector2f mousePos)
{
    this->updateHealthbar();
    this->shield->setPoints(this->guard);
  
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
        this->attack1();
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
        this->attack2();
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num3))
        this->attack3();
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num4))
        this->dead();
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num5))
        this->block();
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num6))
        this->hurt();

    this->animate();
}

void Enemy::animate()
{
    if(this->animationTimer > 5)
    {
        if(this->frame < this->maxFrame)
        {
            this->frame += 1;
            this->animationTimer = 0;
        }
        else
        {
            this->setIdle();
        }
    }
    else
    {
        this->animationTimer += 1;
    }
    this->setAnimationFrame();
}

void Enemy::setAnimationFrame()
{
    if(this->animationType == 0)
    {
        if(this->charakter == 0)
        {
            switch(this->frame)
            {
                case 1:
                    this->setTextureRect(sf::IntRect(41, 47, 60, 81));
                    break;
                case 2:
                    this->setTextureRect(sf::IntRect(169, 47, 60, 81));
                    break;
                case 3:
                    this->setTextureRect(sf::IntRect(297, 47, 60, 81));
                    break;
                case 4:
                    this->setTextureRect(sf::IntRect(425, 47, 60, 81));
                    break;
                case 5:
                    this->setTextureRect(sf::IntRect(553, 47, 60, 81));
                    break;
                case 6:
                    this->setTextureRect(sf::IntRect(681, 47, 60, 81));
                    break;
            }
        }
        else if(this->charakter == 1)
        {
            switch(this->frame)
            {
                case 1:
                    this->setTextureRect(sf::IntRect(50, 47, 60, 81));
                    break;
                case 2:
                    this->setTextureRect(sf::IntRect(178, 47, 60, 81));
                    break;
                case 3:
                    this->setTextureRect(sf::IntRect(306, 47, 60, 81));
                    break;
                case 4:
                    this->setTextureRect(sf::IntRect(434, 47, 60, 81));
                    break;
                case 5:
                    this->setTextureRect(sf::IntRect(562, 47, 60, 81));
                    break;
                case 6:
                    this->setTextureRect(sf::IntRect(690, 47, 60, 81));
                    break;
            }
        }
        else if(this->charakter == 2)
        {
            switch(this->frame)
            {
                case 1:
                    this->setTextureRect(sf::IntRect(30, 47, 80, 81));
                    break;
                case 2:
                    this->setTextureRect(sf::IntRect(158, 47, 80, 81));
                    break;
                case 3:
                    this->setTextureRect(sf::IntRect(286, 47, 80, 81));
                    break;
                case 4:
                    this->setTextureRect(sf::IntRect(414, 47, 80, 81));
                    break;
                case 5:
                    this->setTextureRect(sf::IntRect(542, 47, 80, 81));
                    break;
            }
        }
    }
    else if(this->animationType == 1)
    {
        if(this->charakter == 0)
        {
            switch(this->frame)
            {
                case 1:
                    this->setTextureRect(sf::IntRect(23, 47, 60, 81));
                    break;
                case 2:
                    this->setTextureRect(sf::IntRect(152, 47, 60, 81));
                    break;
                case 3:
                    this->setTextureRect(sf::IntRect(280, 47, 60, 81));
                    break;
                case 4:
                    this->setTextureRect(sf::IntRect(408, 47, 60, 81));
                    break;
                case 5:
                    this->setTextureRect(sf::IntRect(536, 47, 101, 81));
                    break;
                case 6:
                    this->setTextureRect(sf::IntRect(664, 47, 60, 81));
                    break;
            }
        }
        else if(this->charakter == 1)
        {
            switch(this->frame)
            {
                case 1:
                    this->setTextureRect(sf::IntRect(46, 47, 60, 81));
                    break;
                case 2:
                    this->setTextureRect(sf::IntRect(163, 47, 60, 81));
                    break;
                case 3:
                    this->setTextureRect(sf::IntRect(285, 47, 60, 81));
                    break;
                case 4:
                    this->setTextureRect(sf::IntRect(428, 47, 101, 81));
                    break;
                case 5:
                    this->setTextureRect(sf::IntRect(556, 47, 60, 81));
                    break;
            }
        }
        else if(this->charakter == 2)
        {
            switch(this->frame)
            {
                case 1:
                    this->setTextureRect(sf::IntRect(15, 47, 80, 81));
                    break;
                case 2:
                    this->setTextureRect(sf::IntRect(149, 47, 80, 81));
                    break;
                case 3:
                    this->setTextureRect(sf::IntRect(295, 47, 80, 81));
                    break;
                case 4:
                    this->setTextureRect(sf::IntRect(429, 47, 80, 81));
                    break;
                case 5:
                    this->setTextureRect(sf::IntRect(557, 47, 80, 81));
                    break;
            }
        }
    }
    else if(this->animationType == 2)
    {
        if(this->charakter == 0)
        {
            switch(this->frame)
            {
                case 1:
                    this->setTextureRect(sf::IntRect(30, 47, 60, 81));
                    break;
                case 2:
                    this->setTextureRect(sf::IntRect(155, 47, 60, 81));
                    break;
                case 3:
                    this->setTextureRect(sf::IntRect(286, 47, 101, 81));
                    break;
                case 4:
                    this->setTextureRect(sf::IntRect(414, 47, 60, 81));
                    break;
                case 5:
                    this->setTextureRect(sf::IntRect(414, 47, 60, 81));
                    break;
            }
        }
        else if(this->charakter == 1)
        {
            switch(this->frame)
            {
                case 1:
                    this->setTextureRect(sf::IntRect(45, 47, 60, 81));
                    break;
                case 2:
                    this->setTextureRect(sf::IntRect(45, 47, 60, 81));
                    break;
                case 3:
                    this->setTextureRect(sf::IntRect(151, 47, 101, 81));
                    break;
                case 4:
                    this->setTextureRect(sf::IntRect(275, 47, 60, 81));
                    break;
                case 5:
                    this->setTextureRect(sf::IntRect(275, 47, 60, 81));
                    break;
            }
        }
        else if(this->charakter == 2)
        {
            switch(this->frame)
            {
                case 1:
                    this->setTextureRect(sf::IntRect(21, 47, 80, 81));
                    break;
                case 2:
                    this->setTextureRect(sf::IntRect(21, 47, 80, 81));
                    break;
                case 3:
                    this->setTextureRect(sf::IntRect(157, 47, 80, 81));
                    break;
                case 4:
                    this->setTextureRect(sf::IntRect(286, 47, 80, 81));
                    break;
                case 5:
                    this->setTextureRect(sf::IntRect(286, 47, 80, 81));
                    break;
            }
        }
    }
    else if(this->animationType == 3)
    {
        if(this->charakter == 0)
        {
            switch(this->frame)
            {
                case 1:
                    this->setTextureRect(sf::IntRect(22, 47, 60, 81));
                    break;
                case 2:
                    this->setTextureRect(sf::IntRect(22, 47, 60, 81));
                    break;
                case 3:
                    this->setTextureRect(sf::IntRect(152, 47, 60, 81));
                    break;
                case 4:
                    this->setTextureRect(sf::IntRect(280, 47, 101, 81));
                    break;
                case 5:
                    this->setTextureRect(sf::IntRect(280, 47, 101, 81));
                    break;
            }
        }
        else if(this->charakter == 1)
        {
            switch(this->frame)
            {
                case 1:
                    this->setTextureRect(sf::IntRect(22, 47, 60, 81));
                    break;
                case 2:
                    this->setTextureRect(sf::IntRect(22, 47, 60, 81));
                    break;
                case 3:
                    this->setTextureRect(sf::IntRect(159, 47, 60, 81));
                    break;
                case 4:
                    this->setTextureRect(sf::IntRect(296, 47, 101, 81));
                    break;
                case 5:
                    this->setTextureRect(sf::IntRect(424, 47, 60, 81));
                    break;
            }
        }
        else if(this->charakter == 2)
        {
            switch(this->frame)
            {
                case 1:
                    this->setTextureRect(sf::IntRect(44, 47, 80, 81));
                    break;
                case 2:
                    this->setTextureRect(sf::IntRect(173, 47, 80, 81));
                    break;
                case 3:
                    this->setTextureRect(sf::IntRect(284, 47, 101, 81));
                    break;
                case 4:
                    this->setTextureRect(sf::IntRect(413, 47, 101, 81));
                    break;
                case 5:
                    this->setTextureRect(sf::IntRect(413, 47, 101, 81));
                    break;
            }
        }
    }
    else if(this->animationType == 4)
    {
        if(this->charakter == 0)
        {
            switch(this->frame)
            {
                case 1:
                    this->setTextureRect(sf::IntRect(25, 47, 60, 81));
                    break;
                case 2:
                    this->setTextureRect(sf::IntRect(25, 47, 60, 81));
                    break;
                case 3:
                    this->setTextureRect(sf::IntRect(152, 47, 60, 81));
                    break;
                case 4:
                    this->setTextureRect(sf::IntRect(285, 47, 101, 81));
                    break;
                case 5:
                    this->setTextureRect(sf::IntRect(285, 47, 101, 81));
                    if (this->animationEnd != nullptr) 
                    { *this->animationEnd = true; }
                    break;
            }
        }
        else if(this->charakter == 1)
        {
            switch(this->frame)
            {
                case 1:
                    this->setTextureRect(sf::IntRect(39, 47, 60, 81));
                    break;
                case 2:
                    this->setTextureRect(sf::IntRect(166, 47, 60, 81));
                    break;
                case 3:
                    this->setTextureRect(sf::IntRect(295, 47, 60, 81));
                    break;
                case 4:
                    this->setTextureRect(sf::IntRect(422, 47, 101, 81));
                    break;
                case 5:
                    this->setTextureRect(sf::IntRect(422, 47, 101, 81));
                    if (this->animationEnd != nullptr) 
                    { *this->animationEnd = true; }
                    break;
            }
        }
        else if(this->charakter == 2)
        {
            switch(this->frame)
            {
                case 1:
                    this->setTextureRect(sf::IntRect(33, 47, 80, 81));
                    break;
                case 2:
                    this->setTextureRect(sf::IntRect(161, 47, 80, 81));
                    break;
                case 3:
                    this->setTextureRect(sf::IntRect(283, 47, 80, 81));
                    break;
                case 4:
                    this->setTextureRect(sf::IntRect(405, 47, 80, 81));
                    break;
                case 5:
                    this->setTextureRect(sf::IntRect(533, 47, 80, 81));
                    break;
                case 6:
                    this->setTextureRect(sf::IntRect(661, 47, 80, 81));
                    break;
                case 7:
                    this->setTextureRect(sf::IntRect(789, 47, 80, 81));
                    break;
                case 8:
                    this->setTextureRect(sf::IntRect(917, 47, 80, 81));
                    if (this->animationEnd != nullptr) 
                    { *this->animationEnd = true; }
                    break;
            }
        }
    }
    else if(this->animationType == 5)
    {
        if(this->charakter == 0)
        {
            switch(this->frame)
            {
                case 1:
                    this->setTextureRect(sf::IntRect(36, 47, 60, 81));
                    break;
                case 2:
                    this->setTextureRect(sf::IntRect(36, 47, 60, 81));
                    break;
                case 3:
                    this->setTextureRect(sf::IntRect(36, 47, 60, 81));
                    break;
                case 4:
                    this->setTextureRect(sf::IntRect(164, 47, 60, 81));
                    break;
                case 5:
                    this->setTextureRect(sf::IntRect(164, 47, 60, 81));
                    break;
            }
        }
        else if(this->charakter == 1)
        {
            switch(this->frame)
            {
                case 1:
                    this->setTextureRect(sf::IntRect(38, 47, 80, 81));
                    break;
                case 2:
                    this->setTextureRect(sf::IntRect(166, 47, 80, 81));
                    break;
                case 3:
                    this->setTextureRect(sf::IntRect(294, 47, 80, 81));
                    break;
                case 4:
                    this->setTextureRect(sf::IntRect(422, 47, 80, 81));
                    break;
                case 5:
                    this->setTextureRect(sf::IntRect(422, 47, 80, 81));
                    break;
            }
        }
        else if(this->charakter == 2)
        {
            switch(this->frame)
            {
                case 1:
                    this->setTextureRect(sf::IntRect(40, 47, 80, 81));
                    break;
                case 2:
                    this->setTextureRect(sf::IntRect(40, 47, 80, 81));
                    break;
                case 3:
                    this->setTextureRect(sf::IntRect(40, 47, 80, 81));
                    break;
                case 4:
                    this->setTextureRect(sf::IntRect(167, 47, 80, 81));
                    break;
                case 5:
                    this->setTextureRect(sf::IntRect(167, 47, 80, 81));
                    break;
            }
        }
    }
    else if(this->animationType == 6)
    {
        if(this->charakter == 0)
        {
            switch(this->frame)
            {
                case 1:
                    this->setTextureRect(sf::IntRect(40, 47, 60, 81));
                    break;
                case 2:
                    this->setTextureRect(sf::IntRect(40, 47, 60, 81));
                    break;
                case 3:
                    this->setTextureRect(sf::IntRect(40, 47, 60, 81));
                    break;
                case 4:
                    this->setTextureRect(sf::IntRect(168, 47, 60, 81));
                    break;
                case 5:
                    this->setTextureRect(sf::IntRect(168, 47, 60, 81));
                    break;
            }
        }
        else if(this->charakter == 1)
        {
            switch(this->frame)
            {
                case 1:
                    this->setTextureRect(sf::IntRect(45, 47, 80, 81));
                    break;
                case 2:
                    this->setTextureRect(sf::IntRect(45, 47, 80, 81));
                    break;
                case 3:
                    this->setTextureRect(sf::IntRect(45, 47, 80, 81));
                    break;
                case 4:
                    this->setTextureRect(sf::IntRect(173, 47, 80, 81));
                    break;
                case 5:
                    this->setTextureRect(sf::IntRect(173, 47, 80, 81));
                    break;
            }
        }
        else if(this->charakter == 2)
        {
            switch(this->frame)
            {
                case 1:
                    this->setTextureRect(sf::IntRect(30, 47, 80, 81));
                    break;
                case 2:
                    this->setTextureRect(sf::IntRect(30, 47, 80, 81));
                    break;
                case 3:
                    this->setTextureRect(sf::IntRect(30, 47, 80, 81));
                    break;
                case 4:
                    this->setTextureRect(sf::IntRect(30, 47, 80, 81));
                    break;
                case 5:
                    this->setTextureRect(sf::IntRect(30, 47, 80, 81));
                    break;
            }
        }
    }
}

void Enemy::setIdle()
{
    if(this->charakter == 0)
    {
        this->setTexture(this->sam_idleTexture);
        this->maxFrame = 6;
    }
    else if(this->charakter == 1)
    {
        this->setTexture(this->shi_idleTexture);
        this->maxFrame = 6;
    }
    else if(this->charakter == 2)
    {
        this->setTexture(this->vam_idleTexture);
        this->maxFrame = 5;
    }
    this->frame = 1;
    this->animationTimer = 0;
    this->animationType = 0;
}

void Enemy::setAttack1()
{
    if(this->charakter == 0)
    {
        this->setTexture(this->sam_attack1Texture);
        this->maxFrame = 6;
    }
    else if(this->charakter == 1)
    {
        this->setTexture(this->shi_attack1Texture);
        this->maxFrame = 5;
    }
    else if(this->charakter == 2)
    {
        this->setTexture(this->vam_attack1Texture);
        this->maxFrame = 5;
    }
    this->frame = 1;
    this->animationTimer = 0;
    this->animationType = 1;
}

void Enemy::setAttack2()
{
    if(this->charakter == 0)
    {
        this->setTexture(this->sam_attack2Texture);
        this->maxFrame = 5;
    }
    else if(this->charakter == 1)
    {
        this->setTexture(this->shi_attack2Texture);
        this->maxFrame = 5;
    }
    else if(this->charakter == 2)
    {
        this->setTexture(this->vam_attack2Texture);
        this->maxFrame = 5;
    }
    this->frame = 1;
    this->animationTimer = 0;
    this->animationType = 2;
}

void Enemy::setAttack3()
{
    if(this->charakter == 0)
    {
        this->setTexture(this->sam_attack3Texture);
        this->maxFrame = 5;
    }
    else if(this->charakter == 1)
    {
        this->setTexture(this->shi_attack3Texture);
        this->maxFrame = 5;
    }
    else if(this->charakter == 2)
    {
        this->setTexture(this->vam_attack3Texture);
        this->maxFrame = 5;
    }
    this->frame = 1;
    this->animationTimer = 0;
    this->animationType = 3;
}

void Enemy::setDead()
{
    if(this->charakter == 0)
    {
        this->setTexture(this->sam_deadTexture);
        this->maxFrame = 5;
    }
    else if(this->charakter == 1)
    {
        this->setTexture(this->shi_deadTexture);
        this->maxFrame = 5;
    }
    else if(this->charakter == 2)
    {
        this->setTexture(this->vam_deadTexture);
        this->maxFrame = 8;
    }
    this->frame = 1;
    this->animationTimer = 0;
    this->animationType = 4;
}

void Enemy::setBlock()
{
    if(this->charakter == 0)
    {
        this->setTexture(this->sam_blockTexture);
        this->maxFrame = 5;
    }
    else if(this->charakter == 1)
    {
        this->setTexture(this->shi_blockTexture);
        this->maxFrame = 5;
    }
    else if(this->charakter == 2)
    {
        this->setTexture(this->vam_blockTexture);
        this->maxFrame = 5;
    }
    this->frame = 1;
    this->animationTimer = 0;
    this->animationType = 5;
}

void Enemy::setHurt()
{
    if(this->charakter == 0)
    {
        this->setTexture(this->sam_hurtTexture);
        this->maxFrame = 5;
    }
    else if(this->charakter == 1)
    {
        this->setTexture(this->shi_hurtTexture);
        this->maxFrame = 5;
    }
    else if(this->charakter == 2)
    {
        this->setTexture(this->vam_hurtTexture);
        this->maxFrame = 5;
    }
    this->frame = 1;
    this->animationTimer = 0;
    this->animationType = 6;
}

void Enemy::attack1()
{
    this->setAttack1();
}

void Enemy::attack2()
{
    this->setAttack2();
}

void Enemy::attack3()
{
    this->setAttack3();
}

void Enemy::dead()
{
    this->setDead();
}

void Enemy::dead(bool *animationEnd)
{
    this->setDead();
    this->animationEnd = animationEnd;
}


void Enemy::block()
{
    this->setBlock();
}

void Enemy::hurt()
{
    this->setHurt();
}
