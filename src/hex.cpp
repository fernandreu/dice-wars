#include "hex.h"

#include "hexgrid.h"
#include "territory.h"
#include "player.h"

#include <QPen>
#include <QPainter>
#include <QtMath>

Hex::Hex(QQuickItem *parent) : QQuickPaintedItem(parent)
{
    territory_ = nullptr;
}

QPointF Hex::center() const
{
    return center_;
}

void Hex::setCenter(const QPointF &center)
{
    center_ = center;
    resize();
    emit sizeChanged();
}
qreal Hex::radius() const
{
    return radius_;
}

void Hex::setRadius(const qreal &radius)
{
    radius_ = radius;
    resize();
    emit sizeChanged();
}

Territory *Hex::territory() const
{
    return territory_;
}

void Hex::setTerritory(Territory *territory)
{
    territory_ = territory;
}

const QPair<int, int>& Hex::gridPosition() const
{
    return gridPosition_;
}

void Hex::setGridPosition(const QPair<int, int> &gridPosition)
{
    gridPosition_ = gridPosition;
}

bool Hex::isIsolated()
{
    auto grid = qobject_cast<HexGrid *>(parent());
    if (!grid) return true;

    for (auto i = 0; i < 6; i++)
    {
        const auto hex = grid->neighbour(this, i);
        if (hex != nullptr && hex->territory() == territory_) return false;
    }

    return true;
}

void Hex::resize()
{
    // The 1.2 and 2.4 factors come from simple hexagon trigonometry
    setX(center_.x() - radius_*1.2);
    setY(center_.y() - radius_*1.2);
    setZ(-1); //Selected territories will overwrite this to 0
    setWidth(radius_*2.4);
    setHeight(radius_*2.4);
}

void Hex::paint(QPainter *painter)
{
    if (!territory_ || !territory_->owner()) return;

    const auto color = territory_->selected() ? Qt::black : territory_->owner()->color().lighter();
    painter->setPen(QPen(color, 2)); // With this trick, no gaps are shown; otherwise, set color as Qt::transparent
    painter->setRenderHints(QPainter::Antialiasing, true);
    painter->setBrush(color);

    QPointF center(width()*0.5, height()*0.5);

    QPointF vertices[7];  // 7 because the hexagon must be closed
    for (auto i = 0; i < 7; i++)
    {
        const auto angle = (60.0*i - 30)*M_PI/180;
        vertices[i] = QPointF(center.x() + radius_ * qCos(angle), center.y() + radius_ * qSin(angle));
    }
    painter->drawPolygon(vertices, 7);

    const auto grid = qobject_cast<HexGrid *>(parent());
    if (!grid) return;

    painter->setPen(QPen(territory_->selected() ? Qt::red : Qt::black, 2));
    painter->setBrush(Qt::transparent);

    // TODO: possibly duplicated edges for two adjacent cells from different territories
    for (auto i = 0; i < 6; i++)
    {
        const auto neighbour = grid->neighbour(this, i);
        if (neighbour == nullptr || neighbour->territory() != territory_)
        {
            painter->drawLine(vertices[i], vertices[i+1]);
        }
    }
}
