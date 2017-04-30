#include <QtWidgets>
#include "intersected.h"
#include "defaultsettings.h"
#include "graphrelation.h"

//
// GraphRelation
//
QPointF GraphRelation::startPoint() const
{
    // 返回scene坐标系的p1
    return QPointF(x(), y());
}

QPointF GraphRelation::endPoint() const
{
    // 返回scene坐标系的p2
    return mapToScene(m_p2);
}

// p是scene坐标
void GraphRelation::setStartPoint(const QPointF &p)
{
    // 记录p2()的位置，等会要转回item坐标赋值给m_p2;
    QPointF old_p2 = endPoint();

    // 设置位置
    setX(p.x());
    setY(p.y());

    // 由于pos已经改变，要重新根据记录的p2赋值给m_p2
    setEndPoint(old_p2);
}

void GraphRelation::setEndPoint(const QPointF &p)
{
    m_p2 = mapFromScene(p);
    updatePosition();
}

void GraphRelation::setDroppedGraph(const Graph *g, const QPointF &p)
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

Graph *GraphRelation::droppedGraph() const
{
    return m_dropped;
}

void GraphRelation::lockDroppedGraph()
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

void GraphRelation::linkTargetGraph(const Graph* g)
{
    setTargetGraph(g);

    if (m_source) {
        linkGraphs();
        return;
    }

    // 计算从p1()到目标graph的交点，返回的是scene坐标点
    QPointF p;
    bool bIntersected = linkedPointer(startPoint(), m_target, p);
    Q_ASSERT_X(bIntersected, "GraphRelation::linkTargetGraph()", "err");

    setEndPoint(p);
}

void GraphRelation::linkSourceGraph(const Graph* g)
{
    setSourceGraph(g);

    if (m_target) {
        linkGraphs();
        return;
    }

    // 计算从p2()到源graph的交点，返回的是scene坐标点
    QPointF p;
    bool bIntersected = linkedPointer(endPoint(), m_source, p);
    Q_ASSERT_X(bIntersected, "GraphRelation::linkSourceGraph", "err");

    // 根据计算得出的交点，重新设置position
    setStartPoint(p);
}

void GraphRelation::setTargetGraph(const Graph *g)
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

void GraphRelation::setSourceGraph(const Graph *g)
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

bool GraphRelation::linkedPointer(const QPointF &start, const Graph *g, QPointF& intersectedPoint)
{
/*    // scene坐标系中的line
    bool bCenter = false;
    QLineF l(start, g->mapToScene(g->droppedPoint(bCenter)));

    // 计算从p1()到目标graph的交点，返回的是scene坐标点
    bool bIntersected = IntersectedPoint(l, g->mapToScene(g->shape()), intersectedPoint);
    return bIntersected;
    */
    return true;
}
bool GraphRelation::linkGraphs()
{
    /*
    Q_ASSERT(m_target);
    Q_ASSERT(m_source);

    QPointF p1, p2;
    QLineF l1(m_source->mapToScene(m_source->droppedPoint()), m_target->mapToScene(m_target->droppedPoint()));
    // 计算从source到target graph的交点，返回的是scene坐标点
    bool b1 = IntersectedPoint(l1, m_target->mapToScene(m_target->shape()), p2);

    QLineF l2(m_target->mapToScene(m_target->droppedPoint()), m_source->mapToScene(m_source->droppedPoint()));
    // 计算从target到source graph的交点，返回的是scene坐标点
    bool b2 = IntersectedPoint(l2, m_source->mapToScene(m_source->shape()), p1);

    // 根据计算结果，重新赋值起点和终点
    setStartPoint(p1);
    setEndPoint(p2);
//    m_p2 = mapFromScene(p2);

    return b1 && b2;
*/
    return true;
}
void GraphRelation::lookupDroppedGraph(const QPointF &p)
{
    QList<QGraphicsItem*> l = scene()->items(p);
    qDebug() << "items.count = " << l.size();

    Graph* dropped = nullptr;
    foreach (QGraphicsItem* i, l) {
        Graph* g = qgraphicsitem_cast<Graph*>(i);
        if (g && g->shouldBeenDropped(p)) {
            dropped = g;
            break;
        }
    }

    setDroppedGraph(dropped, p);
}

void GraphRelation::createGrips()
{
    if (!m_grips.isEmpty()) {
        Q_ASSERT(false);
        return;
    }

    m_grips << QSharedPointer<Grip>::create(Grip::SOURCE_GRIP)
            << QSharedPointer<Grip>::create(Grip::TARGET_GRIP);
}

void GraphRelation::berthGripsAt()
{
    foreach (const auto& g, m_grips) {
        switch (g->m_type) {
        case Grip::SOURCE_GRIP:
            g->m_rect.moveCenter(origin());
            break;
        case Grip::TARGET_GRIP:
            g->m_rect.moveCenter(m_p2);
            break;
        default:
            Q_ASSERT(false);
        }
    }
}

void GraphRelation::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Graph::mousePressEvent(event);
}

void GraphRelation::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
/*
    if (QLineF(event->screenPos(), event->buttonDownScreenPos(Qt::LeftButton))
        .length() < QApplication::startDragDistance()) {
        Graph::mouseMoveEvent(event);
        return;
    }

    QDrag *drag = new QDrag(event->widget());
    QMimeData *mime = new QMimeData;
    drag->setMimeData(mime);
    drag->exec();
*/

    if (isGrippedState()) {
        lookupDroppedGraph(event->scenePos());
    }

    Graph::mouseMoveEvent(event);
}

void GraphRelation::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Graph::mouseReleaseEvent(event);

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

void GraphRelation::resize(const QSizeF &diff)
{
    Q_ASSERT(m_currentGripIndex < m_grips.size());
    auto g = m_grips.at(m_currentGripIndex);

    switch ( g->m_type) {
    case Grip::SOURCE_GRIP:
        setStartPoint(QPointF(startPoint().x() + diff.width(), startPoint().y() + diff.height()));
        break;
    case Grip::TARGET_GRIP:
        m_p2.setX(m_p2.x() + diff.width());
        m_p2.setY(m_p2.y() + diff.height());
        break;
    default:
        Q_ASSERT(false);
    }

    berthGripsAt();
    updatePosition();
}

bool GraphRelation::shouldBeenDropped(const QPointF &p)
{
    Q_UNUSED(p);

    return false;
}

int GraphRelation::type() const
{
    return GraphRelationType;
}

QRectF GraphRelation::boundingRect() const
{
    QRectF r(origin(), m_p2);
    return r.normalized()
            .adjusted(0 - 6,
                      0 - 6,
                      6,
                      6
                      );
}

void GraphRelation::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(pen());
    painter->setBrush(brush());
    painter->setFont(font());

    painter->drawLine(origin(), m_p2);

//    QPainterPath pp = shape();
//    painter->drawPath(pp);

    drawGrips(painter);
}

QPainterPath GraphRelation::shape() const
{
/*    qreal halfGrip = Singleton<Settings>::instance().sizeOfGrip().width() / 2;

    QList<QPointF> points;
    QLineF line(origin(), m_p2);
    line.setLength(line.length() + halfGrip);
    QPointF endP = line.p2();
    QLineF line2(m_p2, origin());
    line2.setLength(line2.length() + halfGrip);
    QPointF startP = line2.p2();
    line.setLine(startP.x(), startP.y(), endP.x(), endP.y());

    QLineF startEdge = line.normalVector();
    startEdge.setLength(halfGrip);
    points << startEdge.p2();
    startEdge.setLength(0 - halfGrip);
    points << startEdge.p2();

    QLineF endEdge = QLineF(line.p2(), line.p1()).normalVector();
    endEdge.setLength(halfGrip);
    points << endEdge.p2();
    endEdge.setLength(0 - halfGrip);
    points << endEdge.p2();

    points << points.first();

    QPainterPath pp;
    for(auto p = points.cbegin(); p < points.cend(); ++p) {
        if (p == points.cbegin()) {
            pp.moveTo(*p);
        } else {
            pp.lineTo(*p);
        }
    }
*/
    QPainterPath pp2;
    pp2.moveTo(origin());
    pp2.lineTo(m_p2);

    QPainterPathStroker pps;
    pps.setWidth(Singleton<Settings>::instance().sizeOfGrip().width());
    return pps.createStroke(pp2);
}


