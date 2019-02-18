#ifndef TERRITORY_H
#define TERRITORY_H

#include <QtQuick/QQuickPaintedItem>

class Hex;
class Player;

/// This class represents a fixed group of Hex cells. Any player can own it, it has
/// dice (with a minimum of 1) and can attack other territories owned by other players
class Territory : public QQuickPaintedItem
{
    Q_OBJECT
    
    Player *owner_ = nullptr;

    /// Determines whether the territory is selected or not (if selected, it is also drawn differently)
    bool selected_ = false;

    /// The center of the territory, defined as the point where the dice will be placed
    QPointF center_;

    int numDice_ = 1;

    /// The Hex cells this territory is made of. Once initialized at the game start, the list will not change
    QVector<Hex *> cells_;

    /// Other territories adjacent to this one. Once precalculated at the game start, the list will not change
    QVector<Territory *> neighbours_;

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

    // This getter is defined here to ensure auto works
    const auto& neighbours() const { return neighbours_; }

    bool selected() const;
    void setSelected(bool selected);

    // Size of the dice, in pixels
    static const int DICE_SIZE;

    static const int MAX_DICE;
};

#endif // TERRITORY_H
