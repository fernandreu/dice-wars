#include "territory.h"
#include "player.h"
#include "hexgrid.h"

#include <QDebug>
#include <QPainter>
#include <QtMath>

const int Territory::DICE_SIZE = 28;
const int Territory::MAX_DICE = 8;

Territory::Territory(QQuickItem* parent): QQuickPaintedItem(parent)
{
}

void Territory::paint(QPainter *painter)
{
    if (owner_ == nullptr) return;
    if (center_.x() < 0 && center_.y() < 0) return;

    const auto pixmap = owner_->dicePixmap();

    const auto heightFactor = 0.54;   // When the dice are piled above each other, they don't need the full height to be shown
    const auto widthFactor = 0.55;    // The same with the width when there are two piles

    //Note: if there is only one pile of dice, it will not be centered. However, this is preferable for when the animation of adding dice is played in the future,
    //so that it does not suddenly swap the pile position when it switched form 1 to 2 piles
    auto startX = width()*0.5 - DICE_SIZE * widthFactor;
    auto startY = height() - DICE_SIZE;
    for (auto i = MAX_DICE / 2; i < numDice_; i++) //Drawing the ones at the left pile. This column is behind and aligned with the dice edge
    {
        painter->drawPixmap(
                    static_cast<int>(startX),
                    startY - DICE_SIZE * 0.3 - (i - MAX_DICE / 2.0) * DICE_SIZE * heightFactor,
                    DICE_SIZE,
                    DICE_SIZE,
                    *pixmap
        );
    }
    startX += DICE_SIZE * widthFactor;
    for (auto i = 0; i < MAX_DICE / 2 && i < numDice_; i++) //Drawing the ones at the right pile
    {
        painter->drawPixmap(
                    static_cast<int>(startX),
                    startY - i * DICE_SIZE * heightFactor,
                    DICE_SIZE,
                    DICE_SIZE,
                    *pixmap
        );
    }
}

Player *Territory::owner() const
{
    return owner_;
}

void Territory::setOwner(Player *value)
{
    owner_ = value;
    if (!value) regenerateNeighbours(); //As not owned territories do not appear in the neighbours list, this needs a recalculation
}

int Territory::cellCount() const
{
    return cells_.size();
}

void Territory::appendCell(Hex *cell)
{
    if (cell == nullptr) return;

    const auto grid = qobject_cast<HexGrid *>(parent());

    auto cTerritory = cell->territory();
    if (cTerritory != nullptr)
    {
        cTerritory->cells_.removeAll(cell);
        regenerateNeighbours();
    }
    else if (grid)
    {
        for (auto i = 0; i < 6; i++)
        {
            const auto neighbour = grid->neighbour(cell, i);
            if (!neighbour) continue;
            auto terr = neighbour->territory();
            if (terr && terr != this && terr->owner())
            {
                if (!neighbours_.contains(terr)) neighbours_.append(terr);
                if (!terr->neighbours_.contains(this)) terr->neighbours_.append(this);
            }
        }
    }
    cells_.append(cell);
    cell->setTerritory(this);
    calculateCenter();
}

void Territory::removeCell(Hex *cell)
{
    if (cell == nullptr) return;
    cells_.removeAll(cell);
    cell->setTerritory(nullptr);
    calculateCenter();
    regenerateNeighbours();
}

void Territory::updateAll() //The cells are not automatically updated because they are not their children actually: they are the grid's children
{
    update();
    for (auto hex : cells_) hex->update();
}

Hex *Territory::findEmptyAdjacent() const
{
    //Note: the function assumes the qsrand() function has been previously used

    if (cells_.empty()) return nullptr;
    const auto grid = qobject_cast<HexGrid *>(parent());
    if (!grid) return nullptr;

    const auto size = cells_.size();
    const auto indexOffset = qrand() % size;
    for (auto indexBase = 0; indexBase < size; indexBase++)
    {
        const auto hex = cells_.at((indexBase + indexOffset) % size);

        const auto dirOffset = qrand() % 6;    //Putting this outside this for loop should be random enough, but this looks cleaner
        for (auto dirBase = 0; dirBase < 6; dirBase++)
        {
            const auto neighbour = grid->neighbour(hex, (dirBase + dirOffset) % 6);

            if (neighbour != nullptr && neighbour->territory() == nullptr) return neighbour;
        }
    }

    return nullptr;
}

int Territory::grow(int numCells)
{
    // Note: this function assumes qsrand() has been previously used

    if (cells_.empty()) return 0;
    const auto grid = qobject_cast<HexGrid *>(parent());
    if (!grid) return 0;

    auto size = cells_.size();
    for (auto cellCount = 0; cellCount < numCells; cellCount++)
    {
        auto cellAdded = false;

        const auto indexOffset = qrand() % size;
        for (auto indexBase = 0; indexBase < size; indexBase++)
        {
            const auto hex = cells_.at((indexBase + indexOffset) % size);

            // Putting this outside the indexBase for loop should be random enough, but this looks cleaner
            const auto dirOffset = qrand() % 6;

            for (auto dirBase = 0; dirBase < 6; dirBase++)
            {
                const auto neighbour = grid->neighbour(hex, (dirBase + dirOffset) % 6);

                if (neighbour != nullptr && neighbour->territory() == nullptr)
                {
                    cellAdded = true;
                    appendCell(neighbour);
                    size++;
                    break;
                }
            }

            if (cellAdded) break;
        }

        if (!cellAdded) return cellCount;
    }

    return numCells;
}

void Territory::calculateCenter()
{
    const auto size = cells_.size();

    if (size == 0)
    {
        center_ = QPointF(-1, -1);
        return;
    }

    qreal centerX = 0, centerY = 0;
    for (auto hex : cells_)
    {
        auto hexCenter = hex->center();
        centerX += hexCenter.x()/size;
        centerY += hexCenter.y()/size;
    }

    setX(centerX - DICE_SIZE * 0.9);
    setY(centerY - DICE_SIZE * 2.5);

    // This ensures a gradual Z order where the territories at the bottom show their dice above the territories at the top
    setZ(centerY / 10);

    setWidth(DICE_SIZE * 1.8);
    setHeight(static_cast<qreal>(DICE_SIZE) * 3);

    center_ = QPointF(centerX, centerY);
}

QPointF Territory::center() const
{
    return center_;
}
int Territory::numDice() const
{
    return numDice_;
}

void Territory::setNumDice(int numDice)
{
    numDice_ = qMax(1, qMin(numDice, MAX_DICE));
}

int Territory::addDice(int numDice)
{
    setNumDice(numDice_ + numDice);
    return numDice_;
}

const QList<Territory *> & Territory::neighbours() const
{
    return neighbours_;
}

bool Territory::selected() const
{
    return selected_;
}

void Territory::setSelected(bool selected)
{
    selected_ = selected;
    for (auto hex : cells_)
    {
        if (selected)
            hex->setZ(0);
        else
            hex->setZ(-1);
    }
    updateAll();
}


void Territory::regenerateNeighbours() //Territories with no owner do not appear in the neighbours list
{
    const auto grid = qobject_cast<HexGrid *>(parent());
    if (!grid) return;

    neighbours_.clear();

    for (auto cell : cells_)
    {
        for (auto i = 0; i < 6; i++)
        {
            const auto neighbour = grid->neighbour(cell, i);
            if (!neighbour) continue;
            auto terr = neighbour->territory();
            if (terr && terr != this && terr->owner() && !neighbours_.contains(terr)) neighbours_.append(terr);
        }
    }
}
