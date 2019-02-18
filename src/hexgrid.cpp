#include "hexgrid.h"

#include "hex.h"
#include "player.h"
#include "territory.h"
#include "diceroll.h"

#include <QtMath>
#include <QTime>
#include <QElapsedTimer>

const int HexGrid::DIRECTIONS[][2] = {{1,0},{0,1},{-1,1},{-1,0},{0,-1},{+1,-1}};

HexGrid::HexGrid(QQuickItem *parent)
    : QQuickItem(parent)
{
}

HexGrid::~HexGrid()
{
    for (auto hex : map_) delete hex;

    for (auto player : players_) delete player;

    for (auto territory : territories_) delete territory;

    if (timer_.isActive()) timer_.stop();

    delete diceRoll_;
}

void HexGrid::initializeGrid()
{
    QElapsedTimer timer;
    timer.start();
    if (numPlayers_ <= 0) return;

    qsrand(static_cast<uint>(QTime::currentTime().msec()));

    players_.clear();
    for (auto terr : territories_) delete terr;
    territories_.clear();
    for (auto hex : map_) delete hex;
    map_.clear();
    selectedTerritory_ = nullptr;
    otherTerritory_ = nullptr;

    playingAnimation_ = false;
    autoMode_ = false;
    if (timer_.isActive()) timer_.stop();
    // ReSharper disable once CppExpressionWithoutSideEffects
    timer_.disconnect();

    humansLeft_ = 0;

    delete diceRoll_;
    diceRoll_ = new DiceRoll(qobject_cast<QQuickItem*>(parent()));
    diceRoll_->setX(x());
    diceRoll_->setY(y() + height() - 190);
    diceRoll_->setWidth(width());
    diceRoll_->setHeight(120);
    connect(diceRoll_, &DiceRoll::rollFinished, this, &HexGrid::attackFinished);

    for (auto i = 0; i < numPlayers_; i++)
    {
        auto player = new Player();
        player->setPlayerNumber(i);

        if (humanList_.at(i))
        {
            player->setHuman(true);
            humansLeft_++;
        }
        players_.append(player);
    }

    playersLeft_ = numPlayers_;

    const auto height = radius_ * 2;
    const auto vert = height * 3/4;
    const auto width = qSqrt(3)/2 * height;
    const auto horz = width;

    for (auto x = 0; x < gridWidth_; x++)
    {
        for (auto y = 0; y < gridHeight_; y++)
        {
            QPointF pos(horz*x + (y % 2 != 0 ? width/2 : 0), vert*y);
            auto hex = new Hex(this);
            hex->setCenter(pos);
            hex->setRadius(radius_);

            QPair<int, int> pair;
            pair.first = x - (y - (y&1)) / 2;
            pair.second = y;
            hex->setGridPosition(pair);
            map_.insert(pair, hex);
        }
    }

    //Until this point, the players have been regenerated, the hex grid too, but and it only needs to create the tertitoreis and asign hex cells to them

    initMethodGrowth();

    // Remove small territories
    /*for (Hex *hex : *map_)
    {
        if (hex->isIsolated() && hex->territory() != 0) hex->territory()->removeCell(hex);
    }*/
    for (auto terr : territories_)
    {
        if (terr->cellCount() < 6) terr->owner()->removeTerritory(terr);
    }

    //Adding initial dice for the players' territories
    for (auto player : players_) player->addDice(territories_.size() * 15 / 10 / players_.size());

    // Emit signal that initializes the connected territories for each player
    for (auto i = 0; i < players_.size(); i++)
    {
        emit connTerrChanged(i, players_.at(i)->connectedTerritories());
    }

    //Using set function instead of assigning the value to playerTurn_ directly because this will emit the corresponding signal too
    setPlayerTurn(qrand() % players_.size());

    //qDebug() << "Elapsed time for initialization: " << timer.elapsed();

    if (!players_.at(playerTurn_)->human()) startAITurn();
}

int HexGrid::territorySize() const
{
    return territorySize_;
}

void HexGrid::setTerritorySize(int territorySize)
{
    territorySize_ = territorySize;
}

int HexGrid::numTerritories() const
{
    return numTerritories_;
}

void HexGrid::setNumTerritories(int numTerritories)
{
    numTerritories_ = numTerritories;
}


//The territories are uniformly created, however the boundary hexagons may be swapped to the adjacent territories
void HexGrid::initMethodGrid()
{
    const auto size = 5;

    for (auto x = 0; x < gridWidth_; x++)
    {
        for (auto y = 0; y < gridHeight_; y++)
        {
            QPair<int, int> pair;
            pair.first = x - (y - (y&1)) / 2;
            pair.second = y;

            const auto hex = map_.value(pair);

            auto index = ((x / size) * (gridHeight_/size + 1) + y / size);
            if (x % size == size - 1) //At the limit of a terrain
                index += (qrand() % 2) * (gridHeight_/size + 1);
            else if (y % size == size - 1)
                index += (qrand() % 2);

            if (index >= territories_.size()) //Adding territories
            {
                for (auto i = territories_.size(); i <= index; i++)
                {
                    auto terr = new Territory(this);
                    players_.at(qrand() % numPlayers_)->appendTerritory(terr); //The owner will be set internally
                    territories_.append(terr);
                }
            }
            territories_.at(index)->appendCell(hex);
        }
    }
}

void HexGrid::initMethodGrowth()
{
    QPair<int, int> pair;
    const auto x = qrand() % gridWidth_;
    const auto y = qrand() % gridHeight_;
    pair.first = x - (y - (y&1)) / 2;
    pair.second = y;
    auto hex = map_.value(pair);

    //Creating a first territory for that hex cell
    auto terr = new Territory(this);
    players_.at(0)->appendTerritory(terr); //The owner will be set internally
    terr->appendCell(hex);
    terr->grow(territorySize_);
    territories_.append(terr);

    for (auto terrCount = 1; terrCount < numTerritories_; terrCount++)
    {
        // First attempt: selecting and adjacent hex from the previous territory
        hex = terr->findEmptyAdjacent();

        // IF that does not work, then select an adjacent hex from a random territory
        while (!hex)
        {
            hex = territories_.at(qrand() % terrCount)->findEmptyAdjacent();
        }

        terr = new Territory(this);
        players_.at(terrCount % numPlayers_)->appendTerritory(terr); //The owner will be set internally
        terr->appendCell(hex);
        territories_.append(terr);

        //Now: make the territories grow randomly to the number of hex cells desired
        terr->grow(territorySize_);
    }

}

void HexGrid::processClick(qreal x, qreal y)
{
    if (playingAnimation_) return;

    const auto cubeX = (x * sqrt(3)/3 - y / 3) / radius_;
    const auto cubeZ = y * 2 / 3 / radius_;
    const auto cubeY = -cubeX -cubeZ;
    const auto hex = cubeRound(cubeX, cubeY, cubeZ);

    if (!hex || !hex->territory() || !hex->territory()->owner()) return;

    auto terr = hex->territory();

    // The selected territory has been clicked
    if (selectedTerritory_ != nullptr && selectedTerritory_ == terr)
    {
        terr->setSelected(false);
        selectedTerritory_ = nullptr;
        return;
    }

    // The selected territory is from a different player and adjacent to the one already selected -> ATTACK!
    if (selectedTerritory_ != nullptr && selectedTerritory_->owner() != terr->owner() && terr->neighbours().contains(selectedTerritory_))
    {
        otherTerritory_ = terr;
        processAttack();

        //TODO: before deselecting the originally selected territory, there should be a battle animation with both selected, and then they will both be deselected
        selectedTerritory_->setSelected(false);
        selectedTerritory_ = nullptr;

        return;
    }

    if (hex->territory()->owner() != players_.at(playerTurn_) || hex->territory()->numDice() < 2) return; //Only territories with 2 or more dice can attack, and hence can be selected

    if (selectedTerritory_ != nullptr) selectedTerritory_->setSelected(false);

    terr->setSelected(true);
    selectedTerritory_ = terr;
}

void HexGrid::processAttack() const
{
    const auto attacker = selectedTerritory_->owner();
    const auto defender = otherTerritory_->owner();

    auto attack = selectedTerritory_->numDice(), defense = otherTerritory_->numDice();
    if (cheatMode_ && attacker->human()) attack = 1 + static_cast<int>(attack * 1.5); //Cheater!
    if (cheatMode_ && defender->human()) defense = 1 + static_cast<int>(defense * 1.5); //Cheater!

    diceRoll_->startRoll(attacker, attack, defender, defense);
}

void HexGrid::attackFinished(int attack, int defense)
{
    emit showAttackResult(attack, defense);

    auto attacker = selectedTerritory_->owner(), defender = otherTerritory_->owner();

    if (attack > defense)
    {
        attacker->appendTerritory(otherTerritory_);
        otherTerritory_->setNumDice(selectedTerritory_->numDice() - 1);
        emit connTerrChanged(players_.indexOf(attacker), attacker->connectedTerritories());
        emit connTerrChanged(players_.indexOf(defender), defender->connectedTerritories());
        otherTerritory_->updateAll();

        if (defender->connectedTerritories() == 0) playersLeft_--;

        if (defender->human() && defender->territories().empty()) humansLeft_--;
    }

    selectedTerritory_->setNumDice(1);
    selectedTerritory_->setSelected(false);

    if (playersLeft_ == 1)
    {
        emit victory(players_.indexOf(attacker), attacker->human());
        return;
    }

    if (autoMode_ || !attacker->human()) //start next AI turn
    {
        otherTerritory_->setSelected(false);
        timer_.disconnect();
        connect(&timer_, &QTimer::timeout, this, &HexGrid::nextAIStep);
        timer_.start(static_cast<int>(AI_STEP_INTERVAL / gameSpeed_));
    }
}

int HexGrid::playerTurn() const
{
    return playerTurn_;
}

void HexGrid::setPlayerTurn(int playerTurn)
{
    playerTurn_ = playerTurn;
    emit playerTurnChanged();
}

Hex *HexGrid::cubeRound(qreal x, qreal y, qreal z) const
{
    auto rx = qRound(x);
    auto ry = qRound(y);
    auto rz = qRound(z);

    const auto xDiff = qAbs(rx-x);
    const auto yDiff = qAbs(ry-y);
    const auto zDiff = qAbs(rz-z);

    if (xDiff > yDiff && xDiff > zDiff)
        rx = -ry-rz;
    else if (yDiff > zDiff)
        ry = -rx-rz;
    else
        rz = -rx-ry;

    QPair<int, int> pair;
    pair.first = rx;
    pair.second = rz;

    return map_.value(pair);
}

void HexGrid::startAITurn()
{
    if (players_.at(playerTurn_)->human()) autoMode_ = true;
    playingAnimation_ = true;
    timer_.disconnect();
    connect(&timer_, &QTimer::timeout, this, &HexGrid::nextAIStep);
    timer_.setSingleShot(true);
    timer_.start(static_cast<int>(AI_STEP_INTERVAL  / gameSpeed_));
}
QVector<bool> HexGrid::humanList() const
{
    return humanList_;
}

void HexGrid::setHumanList(const QVector<bool> &humanList)
{
    humanList_ = humanList;
}

qreal HexGrid::gameSpeed() const
{
    return gameSpeed_;
}

void HexGrid::setGameSpeed(const qreal &gameSpeed)
{
    if (gameSpeed <= 0) return;
    if (timer_.isActive()) timer_.start(static_cast<int>(timer_.remainingTime() * gameSpeed_ / gameSpeed));
    gameSpeed_ = gameSpeed;
}

bool HexGrid::cheatMode() const
{
    return cheatMode_;
}

void HexGrid::setCheatMode(bool cheatMode)
{
    cheatMode_ = cheatMode;
}


void HexGrid::nextAIStep()
{
    const auto player = players_.at(playerTurn_);

    const auto terrCount = player->territories().size();
    const auto terrBase = qrand() % terrCount;

    for (auto terrOffset = 0; terrOffset < terrCount; terrOffset++)
    {
        const auto terr = player->territories().at((terrBase + terrOffset) % terrCount);
        if (terr->numDice() < 2) continue;

        const auto nTerrCount = terr->neighbours().size();
        const auto nTerrBase = qrand() % nTerrCount;

        for (auto nTerrOffset = 0; nTerrOffset < nTerrCount; nTerrOffset++)
        {
            auto neigh = terr->neighbours().at((nTerrBase + nTerrOffset) % nTerrCount);
            if (neigh->owner() == nullptr || neigh->owner() == player) continue;

            //When no humans are left, the AI players may attack territories with one more dice
            //int maxDice = humansLeft_ == 0 ? terr->numDice() + 1 : neigh->owner()->human() ? terr->numDice() : terr->numDice() - 1;
            auto maxDice = terr->numDice();
            if (maxDice != Territory::MAX_DICE) maxDice--; //Territories with 8 dice can attack other territories with 8 dice

            if (neigh->numDice() <= maxDice)
            {
                selectedTerritory_ = terr;
                otherTerritory_ = neigh;
                timer_.disconnect();
                connect(&timer_, &QTimer::timeout, this, &HexGrid::selectTerritories);
                timer_.start(0); //No delay needed for selecting the attacking territory, as we have already waited for AI_STEP_INTERVAL
                return;
            }
        }

    }

    //If no territory was attacked, finish the turn
    playingAnimation_ = false;
    timer_.disconnect();
    endTurn();
}

void HexGrid::selectTerritories()
{
    if (!selectedTerritory_->selected()) //First step
    {
        selectedTerritory_->setSelected(true);
        timer_.start(static_cast<int>(AI_SELECT_INTERVAL / gameSpeed_));
    }
    else //Second step
    {
        otherTerritory_->setSelected(true);
        timer_.disconnect();
        connect(&timer_, &QTimer::timeout, this, &HexGrid::processAttack);
        timer_.start(static_cast<int>(AI_ATTACK_INTERVAL / gameSpeed_));
    }

}

void HexGrid::endTurn()
{
    if (playingAnimation_) return;

    autoMode_ = false;
    playingAnimation_ = true;

    players_.at(playerTurn_)->addDice(players_.at(playerTurn_)->connectedTerritories(), false);

    connect(&timer_, &QTimer::timeout, this, &HexGrid::growPlayer);
    timer_.setSingleShot(false);
    timer_.start(static_cast<int>(GROWTH_INTERVAL / gameSpeed_));
}

bool HexGrid::isPlayerHuman(int index) const
{
    const auto player = players_.at(index);
    if (!player) return false;
    return player->human();
}

void HexGrid::growPlayer()
{
    auto player = players_.at(playerTurn_);

    //If the dice could be correctly inserted with no problems and there are dice remaining, keep going; otherwise, stop the timer and finish the turn
    if (player->distributeDice(1) && player->remainingDice() > 0) return;

    playingAnimation_ = false;
    if (timer_.isActive()) timer_.stop();
    timer_.disconnect();

    do
    {
        playerTurn_ = (playerTurn_ + 1) % numPlayers_;
    } while (players_.at(playerTurn_)->territories().empty());

    if (selectedTerritory_)
    {
        selectedTerritory_->setSelected(false);
        selectedTerritory_->updateAll();
        selectedTerritory_ = nullptr;
    }

    emit playerTurnChanged();

    if (!players_.at(playerTurn_)->human()) startAITurn();
}

Hex *HexGrid::neighbour(Hex *hex, int direction) const
{
    if (!map_.contains(hex->gridPosition())) return nullptr;
    QPair<int, int> pair;
    pair.first = hex->gridPosition().first + DIRECTIONS[direction % 6][0];
    pair.second = hex->gridPosition().second + DIRECTIONS[direction % 6][1];
    return map_.value(pair, nullptr);
}

int HexGrid::numPlayers() const
{
    return numPlayers_;
}

void HexGrid::setNumPlayers(int numPlayers)
{
    numPlayers_ = numPlayers;
}

int HexGrid::gridWidth() const
{
    return gridWidth_;
}

void HexGrid::setGridWidth(int gridWidth)
{
    gridWidth_ = gridWidth;
}

int HexGrid::gridHeight() const
{
    return gridHeight_;
}
void HexGrid::setGridHeight(int gridHeight)
{
    gridHeight_ = gridHeight;
}

qreal HexGrid::radius() const
{
    return radius_;
}

void HexGrid::setRadius(qreal radius)
{
    radius_ = radius;
}

