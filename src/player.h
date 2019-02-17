#ifndef PLAYER_H
#define PLAYER_H

#include <QColor>
#include <QPixmap>

#include "territory.h"

class Player
{
    int playerNumber_ = 0;
    QColor color_;
    
    QList<Territory *> territories_;
    QList<QSharedPointer<QPixmap>> pixmaps_;

    //Calculates the maximum number of contiguous territories that this player has. This will be used
    //to determine the amount of dice that will be given to him after each turn
    void calculateConnTerr();

    //The maximum number of contiguous territories that this player has
    int connectedTerritories_ = 1;

    //After assigning the corresponding dice when a turn ends, the ones that could not be allocated to any territory because they were already full
    int remainingDice_ = 0;

    //The maximum number of remaining dice
    static const int MAX_REMAINING_DICE = 100;

    //Determines whether the player is AI-controlled or not
    bool human_ = false;

public:
    QColor color() const;

    void appendTerritory(Territory *territory);
    void removeTerritory(Territory *territory);

    QSharedPointer<QPixmap> dicePixmap(int diceValue) const;
    QSharedPointer<QPixmap> dicePixmap() const;

    int playerNumber() const;
    void setPlayerNumber(int playerNumber);

    int connectedTerritories() const;

    int remainingDice() const;

    //Adds the specified number of dice to the stack
    void addDice(int numDice, bool distributeThem = true);

    //Distributes the dice in the stack randomly to the territories. If not all the dice could be distributed, it will return false
    bool distributeDice(int numDice);

    bool human() const;
    void setHuman(bool human);

    const QList<Territory *>& territories() const;
};

#endif // PLAYER_H
