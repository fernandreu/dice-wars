#ifndef PLAYER_H
#define PLAYER_H

#include <QColor>
#include <QPixmap>

#include "territory.h"

/// This class controls all statistics and actions that a player sees / can perform
class Player
{
    /// A unique index given to each player. It will determine in which order he plays
    int playerNumber_ = 0;

    /// The color associated to this player. It will determine the dice pictures to use
    QColor color_;
    
    /// The number of territories that this player controls
    QList<Territory *> territories_;

    /// The 6 images of the dice used by this player with each possible side up
    QList<QSharedPointer<QPixmap>> pixmaps_;

    /// Calculates the maximum number of contiguous territories that this player has. This will be used
    /// to determine the amount of dice that will be given to him after each turn
    void calculateConnTerr();

    /// The maximum number of territories that are both controlled by this player and
    /// contiguous respect to each other. This will determine how many dice the player
    /// receives at the end of his turn
    int connectedTerritories_ = 1;

    /// After assigning the corresponding dice when a turn ends, the ones that could not be allocated to
    /// any territory because they were already full
    int remainingDice_ = 0;

    /// The maximum number of remaining dice
    static const int MAX_REMAINING_DICE = 100;

    /// Determines whether the player is AI-controlled or not
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

    /// Adds the specified number of dice to the stack
    void addDice(int numDice, bool distributeThem = true);

    /// Distributes the dice in the stack randomly to the territories. If all the dice
    /// could not be distributed, it will return false
    bool distributeDice(int numDice);

    bool human() const;
    void setHuman(bool human);

    const QList<Territory *>& territories() const;
};

#endif // PLAYER_H
