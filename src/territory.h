#ifndef TERRITORY_H
#define TERRITORY_H

#include <QtQuick/QQuickPaintedItem>

#include "hex.h"

class Player;

class Territory : public QQuickPaintedItem
{
    Q_OBJECT
    
    Player *owner_ = nullptr;

    /// Determines whether the territory is selected or not (if selected, it is also drawn differently)
    bool selected_ = false;

    QPointF center_;

    int numDice_ = 1;

    QList<Hex *> cells_;

    /// Neighbours of this territory
    QList<Territory *> neighbours_;

    /// To regenerate neighbours. The appendCell function is usually capable of generating the neighbours without using this
    void regenerateNeighbours();

public:
    explicit Territory(QQuickItem* parent = nullptr);

    void paint(QPainter *painter) override;

    Player *owner() const;
    void setOwner(Player *value);

    int cellCount() const;
    void appendCell(Hex *cell);
    void removeCell(Hex *cell);

    void updateAll();

    /// Finds an empty hex cell adjacent to this territory. If there are no empty hex cells, it will return 0
    Hex *findEmptyAdjacent() const;

    /// Attempts to increase the hex cells of the territory by the number specified; returns the actual number of cells that could be added
    int grow(int numCells);

    /// Calculates the center of the territory based on the attached hex cells. If no hex cells are attached, mCenter will be (-1,-1)
    void calculateCenter();

    QPointF center() const;

    int numDice() const;
    void setNumDice(int numDice);

    /// Quick function to add a number of dice to the territory
    int addDice(int numDice);

    const QList<Territory *> & neighbours() const;

    bool selected() const;
    void setSelected(bool selected);

    // Size of the dice, in pixels
    static const int DICE_SIZE;

    static const int MAX_DICE;
};

#endif // TERRITORY_H
