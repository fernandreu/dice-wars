#ifndef HEX_H
#define HEX_H

#include <QtQuick/QQuickPaintedItem>

class Territory;

class Hex : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QPointF center READ center WRITE setCenter)
    Q_PROPERTY(qreal radius READ radius WRITE setRadius)

    QPointF center_;
    qreal radius_ = 10;
    Territory *territory_;

    //In axial coordinates
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

    QPair<int, int> gridPosition() const;
    void setGridPosition(const QPair<int, int> &gridPosition);

    bool isIsolated();

signals:
    void sizeChanged();
};

#endif // HEX_H
