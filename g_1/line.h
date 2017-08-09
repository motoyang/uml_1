#ifndef LINE_H
#define LINE_H

#include "visualentity.h"

class Graph;

class Line: public VisualEntity
{
    using BaseClass = VisualEntity;

    Graph *m_source = nullptr, *m_target = nullptr, *m_dropped = nullptr;

    void setDroppedGraph(const Graph *g, const QPointF &p);
    Graph *droppedGraph() const;
    void lockDroppedGraph();
//    bool linkedPointer(const QPointF &start, const Graph* g, QPointF& intersectedPoint);
    bool linkedSourcePoint(const QPointF &start, const Graph* g, QPointF& intersectedPoint);
    bool linkedTargetPoint(const QPointF &start, const Graph* g, QPointF& intersectedPoint);
//    bool linkGraphs();
    bool linkGraphs2();

protected:
    void lookupDroppedGraph(const QPointF& p);

//    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    virtual QPointF startPoint() const = 0;
    virtual QPointF endPoint() const = 0;
    virtual void setStartPoint(const QPointF& p) = 0;
    virtual void setEndPoint(const QPointF& p) = 0;
    virtual QPointF closestPointToTarget() const = 0;
    virtual QPointF closestPointToSource() const = 0;


public:
    Line(const QPointF &p);

    virtual int type() const override;

    void linkTargetGraph(const Graph* g);
    void linkSourceGraph(const Graph* g);
    void setTargetGraph(const Graph* g);
    void setSourceGraph(const Graph* g);

    virtual bool shouldBeenDropped(const QPointF &p) override;
    virtual bool isLocked() const override;
};


#endif // LINE_H
