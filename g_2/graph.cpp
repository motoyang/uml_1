//#include <QtWidgets>
//#include "defaultsettings.h"
//#include "graphrelation.h"
#include "graph.h"

//
// Graph
//

Graph::Graph(const QPointF &p)
    : BaseClass(p)
{
}

QPointF Graph::origin() const
{
    return QPointF(0.0, 0.0);
}

int Graph::type() const
{
    return GraphType;
}

QVariant Graph::itemChange(GraphicsItemChange change, const QVariant &value)
{
/*
    if (change == ItemPositionChange && scene()) {
        // value is the new position.
        QPointF newPos = value.toPointF();
        QRectF rect = scene()->sceneRect();
        if (!rect.contains(newPos)) {
            // Keep the item inside the scene rect.
            newPos.setX(qMin(rect.right(), qMax(newPos.x(), rect.left())));
            newPos.setY(qMin(rect.bottom(), qMax(newPos.y(), rect.top())));
            return newPos;
        }
    }
*/
    return BaseClass::itemChange(change, value);
}


bool Graph::shouldBeenDropped(const QPointF &p)
{
    return bodyPath().contains(mapFromScene(p));
}

bool Graph::isLocked() const
{
    return false;
}

void Graph::setDroppedFlag(bool f)
{
    Q_UNUSED(f);
}

void Graph::connectTarget(const Link *r)
{
    Q_UNUSED(r);
    Q_ASSERT(false);
}

void Graph::disconnectTarget(const Link * const g)
{
    Q_UNUSED(g);
    Q_ASSERT(false);
}

void Graph::connectSource(const Link *r)
{
    Q_UNUSED(r);
    Q_ASSERT(false);
}

void Graph::disconnectSource(const Link * const g)
{
    Q_UNUSED(g);
    Q_ASSERT(false);
}

void Graph::showDroppedPoint(const QPointF &p)
{
    Q_UNUSED(p);
    Q_ASSERT(false);
}

