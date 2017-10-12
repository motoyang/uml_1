#ifndef GRAPH_H
#define GRAPH_H

#include "ve2.h"

class Link;

class Graph: public VisualEntity
{
    using BaseClass = VisualEntity;

protected:
    QPointF origin() const;

public:
    Graph(const QPointF& p);

    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    virtual int type() const override;

    virtual bool shouldBeenDropped(const QPointF& p) override;
    virtual bool isLocked() const override;

    virtual void setDroppedFlag(bool f);
    virtual void connectTarget(const Link *r);
    virtual void disconnectTarget(const Link* const g);
    virtual void connectSource(const Link *r);
    virtual void disconnectSource(const Link* const g);

    virtual void showDroppedPoint(const QPointF &p);

    virtual bool sourceDroppedPoint(const Link* l, QPointF &p) const = 0;
    virtual bool targetDroppedPoint(const Link* l, QPointF &p) const = 0;
    virtual QPainterPath bodyPath() const = 0;
};


#endif // GRAPH_H
