#include "player.h"

#include <functional>

QColor Player::color() const
{
    return color_;
}

void Player::appendTerritory(Territory *territory)
{
    if (territory == nullptr) return;
    auto tOwner = territory->owner();
    territory->setOwner(this);
    if (tOwner != nullptr)
    {
        tOwner->territories_.removeAll(territory);
        tOwner->calculateConnTerr();
    }
    territories_.append(territory);
    calculateConnTerr();
}

void Player::removeTerritory(Territory *territory)
{
    if (territory == nullptr) return;
    territories_.removeAll(territory);
    territory->setOwner(nullptr);
    calculateConnTerr();
}

QSharedPointer<QPixmap> Player::dicePixmap(int diceValue) const
{
    return pixmaps_.at(diceValue - 1);
}

QSharedPointer<QPixmap> Player::dicePixmap() const
{
    return pixmaps_.at(playerNumber_ % 6);
}

int Player::playerNumber() const
{
    return playerNumber_;
}

void Player::setPlayerNumber(int playerNumber)
{
    if (playerNumber > 7) playerNumber = 7;
    if (playerNumber < 0) playerNumber = 0;
    playerNumber_ = playerNumber;

    switch (playerNumber)  //Pre-defined player colors for players 0 to 7
    {
        case 0:
            color_ = QColor::fromRgb(213, 2, 2);   //red
            break;
        case 1:
            color_ = QColor::fromRgb(2, 117, 2);   //dark green
            break;
        case 2:
            color_ = QColor::fromRgb(245, 245, 15);   //yellow
            break;
        case 3:
            color_ = QColor::fromRgb(2, 230, 230);   //cyan
            break;
        case 4:
            color_ = QColor::fromRgb(117, 2, 230);   //purple
            break;
        case 5:
            color_ = QColor::fromRgb(255, 35, 150);   //pink
            break;
        case 6:
            color_ = QColor::fromRgb(117, 230, 2);   //light green
            break;
        case 7:
            color_ = QColor::fromRgb(230, 127, 2);   //orange
            break;
        default:
            color_ = Qt::black;
    }

    pixmaps_.clear();

    for (auto i = 1; i <= 6; i++)
    {
        pixmaps_.append(QSharedPointer<QPixmap>::create(QString(":/pixmaps/Player%1_Dice%2.png").arg(playerNumber).arg(i)));
    }
}

void Player::calculateConnTerr()
{
    //This is a list that contains the territories that have already been scanned when detecting the max contiguous number
    QList<Territory *> scanned;

    std::function<int (Territory *)> scanTerr = [&](Territory *tScan)
    {
        auto result = 1; //There will be at least one contiguous territory: itself
        for (auto neighbour : tScan->neighbours())
        {
            if (neighbour->owner() == this && !scanned.contains(neighbour))
            {
                scanned.append(neighbour);
                result += scanTerr(neighbour);
            }
        }
        return result;
    };

    connectedTerritories_ = 0;

    for (auto terr : territories_)
    {
        if (scanned.contains(terr)) continue;
        scanned.append(terr);

        const auto numConn = scanTerr(terr);
        if (numConn > connectedTerritories_) connectedTerritories_ = numConn;
    }
}
bool Player::human() const
{
    return human_;
}

void Player::setHuman(bool human)
{
    human_ = human;
}
const QList<Territory *>& Player::territories() const
{
    return territories_;
}

int Player::remainingDice() const
{
    return remainingDice_;
}

void Player::addDice(int numDice, bool distributeThem)
{
    remainingDice_ += numDice;
    if (remainingDice_ > MAX_REMAINING_DICE) remainingDice_ = MAX_REMAINING_DICE;

    if (distributeThem) distributeDice(remainingDice_);
}

bool Player::distributeDice(int numDice)
{
    if (numDice > remainingDice_) numDice = remainingDice_;

    //For each remaining dice, a random territory is chosen. If that territory has already 8 dice, it is skipped, otherwise a dice is added
    //If all the territories have been skipped because they are all full, the process stops
    const auto size = territories_.size();

    for (auto diceCount = 0; diceCount < numDice; diceCount++)
    {
        auto added = false;
        const auto startIndex = qrand() % size;
        for (auto offsetIndex = 0; offsetIndex < size; offsetIndex++)
        {
            auto terr = territories_.at((startIndex + offsetIndex) % size);
            if (terr->numDice() < Territory::MAX_DICE)
            {
                terr->addDice(1);
                remainingDice_--;
                terr->update();
                added = true;
                break;
            }
        }
        if (!added) return false;
    }

    return true;
}

int Player::connectedTerritories() const
{
    return connectedTerritories_;
}



