#ifndef HEX_H
#define HEX_H

#include <QtQuick/QQuickPaintedItem>

class Territory;

/// This class represents a single hexagon in the grid
class Hex : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QPointF center READ center WRITE setCenter)
    Q_PROPERTY(qreal radius READ radius WRITE setRadius)

    /// The center of the Hex, in pixel coordinates
    QPointF center_;

    /// The radius of the Hex, in pixel coordinates
    qreal radius_ = 10;

    /// The Territory this Hex belongs to
    Territory *territory_;

    /// The posiiton inside the HexGrid, in axial coordinates
    QPair<int, int> gridPosition_;

    void resize();

public:
    explicit Hex(QQuickItem *parent = nullptr);

    void paint(QPainter *painter) override;

    QPointF center() const;
    void setCenter(const QPointF &center);

    qreal radius() const;
    void setRadius(const qreal &radius);

    Territory *territory() const;
    void setTerritory(Territory *territory);

    const QPair<int, int>& gridPosition() const;
    void setGridPosition(const QPair<int, int> &gridPosition);

    /// Returns whether this Hex is not part of a Territory which contains other Hex instances
    bool isIsolated();

signals:
    void sizeChanged();
};

#endif // HEX_H
