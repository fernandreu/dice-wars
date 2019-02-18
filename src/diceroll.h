#ifndef DICEROLL_H
#define DICEROLL_H

#include <QtQuick/QQuickPaintedItem>

#include "player.h"

/// This class controls the display of a group of dice representing a score in the game. It
/// controls such roll for both the left and the right player simultaneously
class DiceRoll final : public QQuickPaintedItem
{
    Q_OBJECT
    
    QList<int> leftDice_;
    QList<int> rightDice_;
    Player *leftOwner_ = nullptr;
    Player *rightOwner_ = nullptr;

public:
    explicit DiceRoll(QQuickItem* parent = nullptr);

    void paint(QPainter *painter) override;

signals:
    void rollFinished(int leftScore, int rightScore);

public slots:
    void startRoll(Player *leftPlayer, int leftDiceCount, Player *rightPlayer, int rightDiceCount);
};

#endif // DICEROLL_H
