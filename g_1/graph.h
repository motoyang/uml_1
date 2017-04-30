#ifndef GRAPH_H
#define GRAPH_H

#include "visualentity.h"

class Line;
class GraphRelation;

class Graph: public VisualEntity
{
    using BaseClass = VisualEntity;

protected:
    QPointF origin() const;

    // from QGraphicsItem
//    virtual void focusInEvent(QFocusEvent *event) override;
//    virtual void focusOutEvent(QFocusEvent *event) override;
//    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
//    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
//    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
//    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
//    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
//    virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;

public:
    Graph(const QPointF& p);

    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    virtual int type() const override;

    virtual bool shouldBeenDropped(const QPointF& p) override;
    virtual bool isLocked() const override;
    virtual void setDroppedFlag(bool f);
    virtual void connectTarget(const Line *r);
    virtual void disconnectTarget(const Line* const g);
    virtual void connectSource(const Line *r);
    virtual void disconnectSource(const Line* const g);

    virtual void connectTarget(const GraphRelation *r);
    virtual void disconnectTarget(const GraphRelation* const g);
    virtual void connectSource(const GraphRelation *r);
    virtual void disconnectSource(const GraphRelation* const g);

    virtual void showDroppedPoint(const QPointF &p);

    virtual bool sourceDroppedPoint(const Line* l, QPointF &p) const = 0;
    virtual bool targetDroppedPoint(const Line* l, QPointF &p) const = 0;
};


#endif // GRAPH_H
