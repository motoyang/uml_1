#include <QtWidgets>
#include "defaultsettings.h"
#include "intersected.h"
#include "graph.h"
#include "link.h"

//
// Link
//

// 在线段l的终点画箭头
QPainterPath Link::arrowPath(const QLineF &l) const
{
    const qreal ArrowAngle = 30.0;
    const qreal ArrowLength = 10.0;

    QPainterPath pp;
    QLineF l2(l.p2(), l.p1());
    QLineF l3(l2);

    // 找出三个点
    l3.setLength(ArrowLength);
    l3.setAngle(l2.angle() - ArrowAngle);
    QPointF p1 = l3.p2();
    l3.setAngle(l2.angle() + ArrowAngle);
    QPointF p2 = l3.p2();

    // 画箭头，注意pp终点的位置
    pp.moveTo(l3.p1());
    pp.lineTo(p2);
    pp.moveTo(p1);
    pp.lineTo(l3.p1());

    return pp;
}

// 在线段l的终点画三角形，pp的终点在三角形底边的中点
QPainterPath Link::trianglePath(const QLineF &l) const
{
    const qreal TriangleAngle = 30.0;
    const qreal TriangleLength = 10.0;

    QLineF l2(l.p2(), l.p1());
    QLineF l3(l2);
    l3.setLength(TriangleLength);

    // 找出三角形的三个顶点
    l3.setAngle(l2.angle() - TriangleAngle);
    QPointF p1 = l3.p2();
    QPointF p2 = l3.p1();
    l3.setAngle(l2.angle() + TriangleAngle);
    QPointF p3 = l3.p2();

    // 找出三角形底边的中点
    QPointF p4 = QLineF(p3, p1).center();

    // 从底边的中点开始画三角形
    QPainterPath pp;
    pp.moveTo(p4);
    pp.lineTo(p1);
    pp.lineTo(p2);
    pp.lineTo(p3);
    pp.lineTo(p4);

    return pp;
}

// 在线段l的起点画菱形，pp的终点在菱形的顶点
QPainterPath Link::diamondPath(const QLineF &l) const
{
    const qreal DiamondAngle = 25.0;
    const qreal DiamondLength = 8.0;

    // 找出菱形的四个顶点
    QLineF l2(l);
    QPointF p3 = l2.p1();
    l2.setLength(DiamondLength);
    l2.setAngle(l.angle() - DiamondAngle);
    QPointF p4 = l2.p2();
    QLineF l3(l2);
    l3.setAngle(l.angle() + DiamondAngle);
    QPointF p2 = l3.p2();
    l3.translate(l2.dx(), l2.dy());
    QPointF p1 = l3.p2();

    // 画菱形，注意pp终点的位置
    QPainterPath pp;
    pp.moveTo(p1);
    pp.lineTo(p2);
    pp.lineTo(p3);
    pp.lineTo(p4);
    pp.lineTo(p1);

    return pp;
}

void Link::setDroppedGraph(const Graph *g, const QPointF &p)
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

Graph *Link::droppedGraph() const
{
    return m_dropped;
}

void Link::lockDroppedGraph()
{
    Grip::GripType gp = (*m_selectedGrip)->m_type;
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
}

void Link::lookupDroppedGraph(const QPointF &p)
{
    Grip::GripType gp = (*m_selectedGrip)->m_type;
    if (!(gp == Grip::TARGET_GRIP || gp == Grip::SOURCE_GRIP)) {
        return;
    }

    QList<QGraphicsItem*> l = scene()->items(p);

    Graph* dropped = nullptr;
    for (QGraphicsItem* i: l) {
        VisualEntity* g = qgraphicsitem_cast<VisualEntity*>(i);
        if (g && g->shouldBeenDropped(p)) {
            dropped = dynamic_cast<Graph*>(g);
            break;
        }
    }

    setDroppedGraph(dropped, p);
}

bool Link::linkedSourcePoint(const QPointF &start, const Graph *g, QPointF &intersectedPoint)
{
    bool bCenter = g->sourceDroppedPoint(this, intersectedPoint);
    if (bCenter) {
        // scene坐标系中的line
        QLineF l(mapToScene(start), g->mapToScene(intersectedPoint));

        // 计算从p1()到目标graph中心的交点，返回的是scene坐标点
        return IntersectedPoint(l, g->mapToScene(g->bodyPath()), intersectedPoint);
    }

    intersectedPoint = g->mapToScene(intersectedPoint);
    return true;
}

bool Link::linkedTargetPoint(const QPointF &start, const Graph *g, QPointF &intersectedPoint)
{
    bool bCenter = g->targetDroppedPoint(this, intersectedPoint);
    if (bCenter) {
        // scene坐标系中的line
        QLineF l(mapToScene(start), g->mapToScene(intersectedPoint));

        // 计算从p1()到目标graph中心的交点，返回的是scene坐标点
        return IntersectedPoint(l, g->mapToScene(g->bodyPath()), intersectedPoint);
    }

    intersectedPoint = g->mapToScene(intersectedPoint);
    return true;
}

bool Link::linkGraphs2()
{
    Q_ASSERT(m_target);
    Q_ASSERT(m_source);

    QPointF p1, p2;
    bool bCenter1 = m_source->sourceDroppedPoint(this, p1);
    bool bCenter2 = m_target->targetDroppedPoint(this, p2);
    if (bCenter2) {
        QLineF l1(mapToScene(closestPointToTarget()), m_target->mapToScene(p2));
        IntersectedPoint(l1, m_target->mapToScene(m_target->bodyPath()), p2);
    } else {
        p2 = m_target->mapToScene(p2);
    }

    if (bCenter1) {
        QLineF l2(mapToScene(closestPointToSource()), m_source->mapToScene(p1));
        IntersectedPoint(l2, m_source->mapToScene(m_source->bodyPath()), p1);
    } else {
        p1 = m_source->mapToScene(p1);
    }

    // 根据计算结果，重新赋值起点和终点
    setStartPoint(mapFromScene(p1));
    setEndPoint(mapFromScene(p2));

    return true;
}

void Link::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (isGrippedState()) {
        lookupDroppedGraph(event->scenePos());
    }

    BaseClass::mouseMoveEvent(event);
}

void Link::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    BaseClass::mouseReleaseEvent(event);

    if (isGrippedState()) {
        Grip::GripType gt = (*m_selectedGrip)->m_type;

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

bool Link::shouldBeenDropped(const QPointF &p)
{
    Q_UNUSED(p);

    return false;
}

bool Link::isLocked() const
{
    return m_source || m_target;
}

Link::Link(const QPointF &p)
    : BaseClass(p)
{
}

int Link::type() const
{
    return LinkType;
}

void Link::linkTargetGraph(const Graph* g)
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

void Link::linkSourceGraph(const Graph* g)
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

void Link::setTargetGraph(const Graph *g)
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

void Link::setSourceGraph(const Graph *g)
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
