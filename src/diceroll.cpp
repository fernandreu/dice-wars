#include "diceroll.h"

#include <QPainter>

DiceRoll::DiceRoll(QQuickItem* parent) : QQuickPaintedItem(parent)
{
}

void DiceRoll::paint(QPainter *painter)
{
    const auto centerX = 0.5 * width();
    const auto centerY = 0.5 * height();

    static const auto DICE_SIZE = 30; //Size of each dice in pixels
    static const auto DICE_SPACING = 5;
    static const auto FIRST_SPACING = 10; //Spacing to add in the separation of the rolls for each player; total amount = 2*(FIRST_SPACING + DICE_SPACING)

    painter->setPen(QPen(Qt::black, 2));
    painter->setFont(QFont("Bavaria", 30, 5));

    //Left part
    if (!leftDice_.empty() && leftOwner_ != nullptr)
    {
        //Draw each dice
        for (auto i = 0; i < leftDice_.size(); i++)
            painter->drawPixmap(
                        static_cast<int>(centerX) - FIRST_SPACING - (DICE_SIZE + DICE_SPACING)*(i+1),
                        static_cast<int>(centerY - 0.5 * DICE_SIZE),
                        DICE_SIZE,
                        DICE_SIZE,
                        *leftOwner_->dicePixmap(leftDice_.at(i))
            );

        //Draw final score
        auto leftScore = 0;
        for (auto n : leftDice_) leftScore += n;
        painter->drawText(
                    static_cast<int>(centerX) - FIRST_SPACING - (DICE_SIZE + DICE_SPACING) * (leftDice_.size() + 1) - (leftScore > 9 ? 20 : 0),
                    static_cast<int>(centerY + 0.5 * DICE_SIZE),
                    QString("%1").arg(leftScore)
        );
    }

    //Right part
    if (!rightDice_.empty() && rightOwner_ != nullptr)
    {
        //Draw each dice
        for (auto i = 0; i < rightDice_.size(); i++)
            painter->drawPixmap(
                        static_cast<int>(centerX) + FIRST_SPACING + (DICE_SIZE + DICE_SPACING)*i,
                        static_cast<int>(centerY - 0.5 * DICE_SIZE),
                        DICE_SIZE,
                        DICE_SIZE,
                        *rightOwner_->dicePixmap(rightDice_.at(i))
            );

        //Draw final score
        auto rightScore = 0;
        for (auto n : rightDice_) rightScore += n;
        painter->drawText(
                    static_cast<int>(centerX) + FIRST_SPACING + (DICE_SIZE + DICE_SPACING) * rightDice_.size() + 4,
                    static_cast<int>(centerY + 0.5 * DICE_SIZE),
                    QString("%1").arg(rightScore)
        );
    }
}

void DiceRoll::startRoll(Player *leftPlayer, int leftDiceCount, Player *rightPlayer, int rightDiceCount)
{
    leftOwner_ = leftPlayer;
    rightOwner_ = rightPlayer;
    leftDice_.clear();
    rightDice_.clear();

    if (leftDiceCount < 1) leftDiceCount = 1;
    if (rightDiceCount < 1) rightDiceCount = 1;

    for (auto i = 0; i < leftDiceCount; i++) leftDice_.append(qrand() % 6 + 1);
    for (auto i = 0; i < rightDiceCount; i++) rightDice_.append(qrand() % 6 + 1);

    //TODO: This should be shown after an animation
    auto leftScore = 0, rightScore = 0;
    for (auto n : leftDice_) leftScore += n;
    for (auto n : rightDice_) rightScore += n;
    update();
    emit rollFinished(leftScore, rightScore);
}
