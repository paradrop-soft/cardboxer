#include "../headers/CardAction.hpp"
#include <iostream>
CardAction::CardAction(bool offensive, bool burn, bool bypass, bool heal, std::string chain, int health, int maxHealth, int guard, int animation)
{
    this->offensive = offensive;
    this->burn = burn;
    this->bypass = bypass;
    this->chain = chain;
    this->healthMod = health;
    this->maxHealthMod = maxHealth;
    this->guardMod = guard;
    this->animation = Animation(animation);
    this->heal = heal;
}

CardAction::CardAction(CardAction &copy)
{
    this->offensive = copy.offensive;
    this->burn = copy.burn;
    this->bypass = copy.bypass;
    this->chain = copy.chain;
    this->healthMod = copy.healthMod;
    this->maxHealthMod = copy.maxHealthMod;
    this->guardMod = copy.guardMod;
    this->animation = copy.animation;
    this->heal = copy.heal;
}

// std::pair<bool, bool> getType();
bool CardAction::isOffensive() { return this->offensive; }
bool CardAction::isDefensive() { return !this->offensive; }

bool CardAction::isBurn() { return this->burn; }
bool CardAction::isBypass() { return this->bypass; }
bool CardAction::isHeal() { return this->heal; }

std::string CardAction::getChain() { return this->chain; }

int CardAction::getHealthMod() { return this->healthMod; }
int CardAction::getMaxHealthMod() { return this->maxHealthMod; }
int CardAction::getGuardMod() { return this->guardMod; }

void CardAction::setHealthMod(int health) { this->healthMod = health; }
void CardAction::setMaxHealthMod(int health) { this->maxHealthMod = health; }
void CardAction::setGuardMod(int guard) { this->guardMod = guard; }
Animation CardAction::getAnimation() { return this->animation; }
