#ifndef DICEROLL_H
#define DICEROLL_H

#include <QtQuick/QQuickPaintedItem>

class Player;

/// This class controls the display of a group of dice representing a score in the game. It
/// controls such roll for both the left and the right player simultaneously
class DiceRoll final : public QQuickPaintedItem
{
    Q_OBJECT
    
    QVector<int> leftDice_;
    QVector<int> rightDice_;
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
