#ifndef GRAPHRELATION_H
#define GRAPHRELATION_H

#include "graph.h"

class GraphRelation: public Graph
{
    // m_p2是本item坐标系中的点
    QPointF m_p2;

    // 返回的都是scene坐标
    QPointF p1() const;
    QPointF p2() const;

    // p是scene坐标
    void setStartPoint(const QPointF& p);

    Graph *m_source = nullptr, *m_target = nullptr, *m_dropped = nullptr;
    void setDroppedGraph(const Graph* g);
    void lockDroppedGraph();
    bool linkedPointer(const QPointF &start, const Graph* g, QPointF& intersectedPoint);
    bool linkGraphs();

protected:
    void lookupDroppedGraph(const QPointF& p);

    // grip相关的操作，子类可以override或实现这些操作。
    virtual void createGrips() override;
    virtual void berthGripsAt() override;

    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    // 子类要实现
    virtual void resize(const QSizeF &diff) override;
    virtual bool shouldBeenDropped(const QPointF &p) override;

public:

    // p1和p2都是scene坐标中的点
    GraphRelation(const QPointF& p1, const QPoint& p2)
        : Graph(p1)
    {
        m_p2 = mapFromScene(p2);
    }

    virtual QRectF boundingRect() const override;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    virtual QPainterPath shape() const override;

    virtual int type() const override;

    void linkTargetGraph();
    void linkSourceGraph();
};

#endif // GRAPHRELATION_H
