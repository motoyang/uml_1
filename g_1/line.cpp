#include <QtWidgets>
#include "defaultsettings.h"
#include "intersected.h"
#include "graph.h"
#include "line.h"

//
// Line
//

void Line::setDroppedGraph(const Graph *g, const QPointF &p)
{
    if (g && (g == m_dropped)) {
        m_dropped->showDroppedPoint(m_dropped->mapFromScene(p));
        return;
    }

    if (m_dropped) {
        m_dropped->setDroppedFlag(false);
    }

    m_dropped = const_cast<Graph*>(g);
    if (m_dropped) {
        m_dropped->setDroppedFlag(true);
    }
}

Graph *Line::droppedGraph() const
{
    return m_dropped;
}

void Line::lockDroppedGraph()
{
    Grip::GripType gp = m_grips.at(m_currentGripIndex)->m_type;
    switch (gp) {
    case Grip::TARGET_GRIP:
        linkTargetGraph(droppedGraph());
        break;
    case Grip::SOURCE_GRIP:
        linkSourceGraph(droppedGraph());
        break;
    default:
        Q_ASSERT(false);
    }
    setDroppedGraph(nullptr, QPointF(0, 0));

    berthGripsAt();
    updatePosition();
}

void Line::lookupDroppedGraph(const QPointF &p)
{
    QList<QGraphicsItem*> l = scene()->items(p);
    qDebug() << "items.count = " << l.size();

    Graph* dropped = nullptr;
    foreach (QGraphicsItem* i, l) {
        VisualEntity* g = qgraphicsitem_cast<VisualEntity*>(i);
        if (g && g->shouldBeenDropped(p)) {
            dropped = dynamic_cast<Graph*>(g);
            break;
        }
    }

    setDroppedGraph(dropped, p);
}
/*
bool Line::linkedPointer(const QPointF &start, const Graph *g, QPointF& intersectedPoint)
{
    // 如果是不是中心的，就使用droppedPointer作为交点
    bool b = false;
    intersectedPoint = g->droppedPoint(b);
    intersectedPoint = g->mapToScene(intersectedPoint);
    if (b) {
        // scene坐标系中的line
        QLineF l(mapToScene(start), g->mapToScene(g->droppedPoint(b)));

        // 计算从p1()到目标graph中心的交点，返回的是scene坐标点
        return IntersectedPoint(l, g->mapToScene(g->shape()), intersectedPoint);
    }

    return true;
}
*/
bool Line::linkedSourcePoint(const QPointF &start, const Graph *g, QPointF &intersectedPoint)
{
    bool bCenter = g->sourceDroppedPoint(this, intersectedPoint);
    if (bCenter) {
        // scene坐标系中的line
        QLineF l(mapToScene(start), g->mapToScene(intersectedPoint));

        // 计算从p1()到目标graph中心的交点，返回的是scene坐标点
        return IntersectedPoint(l, g->mapToScene(g->shape()), intersectedPoint);
    }

    intersectedPoint = g->mapToScene(intersectedPoint);
    return true;
}

bool Line::linkedTargetPoint(const QPointF &start, const Graph *g, QPointF &intersectedPoint)
{
    bool bCenter = g->targetDroppedPoint(this, intersectedPoint);
    if (bCenter) {
        // scene坐标系中的line
        QLineF l(mapToScene(start), g->mapToScene(intersectedPoint));

        // 计算从p1()到目标graph中心的交点，返回的是scene坐标点
        return IntersectedPoint(l, g->mapToScene(g->shape()), intersectedPoint);
    }

    intersectedPoint = g->mapToScene(intersectedPoint);
    return true;
}
/*
bool Line::linkGraphs()
{
    Q_ASSERT(m_target);
    Q_ASSERT(m_source);

    bool b = false;
    QPointF p1, p2;
    p2 = m_target->droppedPoint(b);
    p2 = m_target->mapToScene(p2);
    if (b) {
        QLineF l1(m_source->mapToScene(m_source->droppedPoint(b)), m_target->mapToScene(m_target->droppedPoint(b)));
        // 计算从source到target graph的交点，返回的是scene坐标点
        IntersectedPoint(l1, m_target->mapToScene(m_target->shape()), p2);
    }

    p1 = m_source->droppedPoint(b);
    p1 = m_source->mapToScene(p1);
    if (b) {
        QLineF l2(m_target->mapToScene(m_target->droppedPoint(b)), m_source->mapToScene(m_source->droppedPoint(b)));
        // 计算从target到source graph的交点，返回的是scene坐标点
        IntersectedPoint(l2, m_source->mapToScene(m_source->shape()), p1);
    }

    // 根据计算结果，重新赋值起点和终点
    setStartPoint(mapFromScene(p1));
    setEndPoint(mapFromScene(p2));

    return true;
}
*/
bool Line::linkGraphs2()
{
    Q_ASSERT(m_target);
    Q_ASSERT(m_source);

    QPointF p1, p2;
    bool bCenter1 = m_source->sourceDroppedPoint(this, p1);
    bool bCenter2 = m_target->targetDroppedPoint(this, p2);
    if (bCenter2) {
        QLineF l1(mapToScene(closestPointToTarget()), m_target->mapToScene(p2));
        IntersectedPoint(l1, m_target->mapToScene(m_target->shape()), p2);
    } else {
        p2 = m_target->mapToScene(p2);
    }

    if (bCenter1) {
        QLineF l2(mapToScene(closestPointToSource()), m_source->mapToScene(p1));
        IntersectedPoint(l2, m_source->mapToScene(m_source->shape()), p1);
    } else {
        p1 = m_source->mapToScene(p1);
    }

    // 根据计算结果，重新赋值起点和终点
    setStartPoint(mapFromScene(p1));
    setEndPoint(mapFromScene(p2));

    return true;
}
/*
void Line::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    BaseClass::mousePressEvent(event);
}
*/
void Line::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (isGrippedState()) {
        lookupDroppedGraph(event->scenePos());
    }

    BaseClass::mouseMoveEvent(event);
}

void Line::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    BaseClass::mouseReleaseEvent(event);

    if (isGrippedState()) {
        Grip::GripType gt = m_grips.at(m_currentGripIndex)->m_type;

        // 先断开与targetGraph或sourceGraph的联系
        if (gt == Grip::TARGET_GRIP && m_target) {
            setTargetGraph(nullptr);
        }
        if (gt == Grip::SOURCE_GRIP && m_source) {
            setSourceGraph(nullptr);
        }

        // 如果有新的droppedGraph，就连接到新的graph
        if (droppedGraph()) {
            lockDroppedGraph();
        }
    }
}

bool Line::shouldBeenDropped(const QPointF &p)
{
    Q_UNUSED(p);

    return false;
}

bool Line::isLocked() const
{
    return m_source || m_target;
}

Line::Line(const QPointF &p)
    : BaseClass(p)
{
}

int Line::type() const
{
    return LineType;
}


void Line::linkTargetGraph(const Graph* g)
{
    setTargetGraph(g);

    if (m_source) {
        linkGraphs2();
        return;
    }

    // 计算从p1()到目标graph的交点，返回的是scene坐标点
    QPointF p;
    bool bIntersected = linkedTargetPoint(startPoint(), m_target, p);
    Q_ASSERT_X(bIntersected, "GraphRelation::linkTargetGraph()", "err");

    setEndPoint(mapFromScene(p));
}

void Line::linkSourceGraph(const Graph* g)
{
    setSourceGraph(g);

    if (m_target) {
        linkGraphs2();
        return;
    }

    // 计算从p2()到源graph的交点，返回的是scene坐标点
    QPointF p;
    bool bIntersected = linkedSourcePoint(endPoint(), m_source, p);
    Q_ASSERT_X(bIntersected, "GraphRelation::linkSourceGraph", "err");

    // 根据计算得出的交点，重新设置position
    setStartPoint(mapFromScene(p));
}

void Line::setTargetGraph(const Graph *g)
{
    if (m_target == g) {
        return;
    }

    // 如果原先有连接的graph，先与原graph断开
    if (m_target) {
        m_target->disconnectTarget(this);
        m_target = nullptr;
    }

    if (g) {
        m_target = const_cast<Graph*>(g);
        m_target->connectTarget(this);
    }
}

void Line::setSourceGraph(const Graph *g)
{
    if (m_source == g) {
        return;
    }

    // 如果原先有连接的graph，先与原graph断开
    if (m_source) {
        m_source->disconnectSource(this);
        m_source = nullptr;
    }

    if (g) {
        m_source =  const_cast<Graph*>(g);
        m_source->connectSource(this);
    }
}
