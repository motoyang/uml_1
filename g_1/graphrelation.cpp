#include <QtWidgets>
#include "intersected.h"
#include "defaultsettings.h"
#include "graphrelation.h"

//
// GraphRelation
//
QPointF GraphRelation::p1() const
{
    // 返回scene坐标系的p1
    return QPointF(x(), y());
}

QPointF GraphRelation::p2() const
{
    // 返回scene坐标系的p2
    return mapToScene(m_p2);
}

// p是scene坐标
void GraphRelation::setStartPoint(const QPointF &p)
{
    // 记录p2()的位置，等会要转回item坐标赋值给m_p2;
    QPointF old_p2 = p2();

    // 设置位置
    setX(p.x());
    setY(p.y());

    // 由于pos已经改变，要重新根据记录的p2赋值给m_p2
    m_p2 = mapFromScene(old_p2);
}

void GraphRelation::setDroppedGraph(const Graph *g)
{
    if (g != m_dropped) {
        if (m_dropped) {
            m_dropped->setDroppedFlag(false);
        }
        m_dropped = const_cast<Graph*>(g);
        if (m_dropped) {
            m_dropped->setDroppedFlag(true);
        }
    }
}

void GraphRelation::lockDroppedGraph()
{
    if (!isGrippedState() || !m_dropped) {
        return;
    }

    Grip::GripType gp = m_grips.at(m_currentGripIndex)->m_type;
    switch (gp) {
    case Grip::TARGET_GRIP:
        m_target = m_dropped;
        linkTargetGraph();
        break;
    case Grip::SOURCE_GRIP:
        m_source = m_dropped;
        linkSourceGraph();
        break;
    default:
        Q_ASSERT(false);
    }
    setDroppedGraph(nullptr);

    berthGripsAt();
    setX(p1().x() + 0.00001);
}

void GraphRelation::linkTargetGraph()
{
    if (m_source) {
        linkGraphs();
        return;
    }

    // 计算从p1()到目标graph的交点，返回的是scene坐标点
    QPointF p;
    bool bIntersected = linkedPointer(p1(), m_target, p);
    Q_ASSERT_X(bIntersected, "GraphRelation::linkTargetGraph()", "err");

    m_p2 = mapFromScene(p);
}

void GraphRelation::linkSourceGraph()
{
    if (m_target) {
        linkGraphs();
        return;
    }

    // 计算从p2()到源graph的交点，返回的是scene坐标点
    QPointF p;
    bool bIntersected = linkedPointer(p2(), m_source, p);
    Q_ASSERT_X(bIntersected, "GraphRelation::linkSourceGraph", "err");

    // 根据计算得出的交点，重新设置position
    setStartPoint(p);
}

bool GraphRelation::linkedPointer(const QPointF &start, const Graph *g, QPointF& intersectedPoint)
{
    // scene坐标系中的line
    QLineF l(start, g->mapToScene(g->centerPoint()));

    // 计算从p1()到目标graph的交点，返回的是scene坐标点
    bool bIntersected = IntersectedPoint(l, g->mapToScene(g->shape()), intersectedPoint);
    return bIntersected;
}

bool GraphRelation::linkGraphs()
{
    Q_ASSERT(m_target);
    Q_ASSERT(m_source);

    QPointF p1, p2;
    QLineF l1(m_source->mapToScene(m_source->centerPoint()), m_target->mapToScene(m_target->centerPoint()));
    // 计算从source到target graph的交点，返回的是scene坐标点
    bool b1 = IntersectedPoint(l1, m_target->mapToScene(m_target->shape()), p2);

    QLineF l2(m_target->mapToScene(m_target->centerPoint()), m_source->mapToScene(m_source->centerPoint()));
    // 计算从target到source graph的交点，返回的是scene坐标点
    bool b2 = IntersectedPoint(l2, m_source->mapToScene(m_source->shape()), p1);

    // 根据计算结果，重新赋值起点和终点
    setStartPoint(p1);
    m_p2 = mapFromScene(p2);

    return b1 && b2;
}

void GraphRelation::lookupDroppedGraph(const QPointF &p)
{
    QList<QGraphicsItem*> l = scene()->items(p);
    qDebug() << "items.count = " << l.size();

    Graph* dropped = nullptr;
    foreach (QGraphicsItem* i, l) {
        Graph* g = qgraphicsitem_cast<Graph*>(i);
        if (g->shouldBeenDropped(p)) {
            dropped = g;
            break;
        }
    }

    setDroppedGraph(dropped);
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

    lookupDroppedGraph(event->scenePos());
    Graph::mouseMoveEvent(event);
}

void GraphRelation::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Graph::mouseReleaseEvent(event);
    lockDroppedGraph();
}

void GraphRelation::resize(const QSizeF &diff)
{
    Q_ASSERT(m_currentGripIndex < m_grips.size());
    auto g = m_grips.at(m_currentGripIndex);

    switch ( g->m_type) {
    case Grip::SOURCE_GRIP:
        setStartPoint(QPointF(p1().x() + diff.width(), p1().y() + diff.height()));
        break;
    case Grip::TARGET_GRIP:
        m_p2.setX(m_p2.x() + diff.width());
        m_p2.setY(m_p2.y() + diff.height());
        break;
    default:
        Q_ASSERT(false);
    }

    berthGripsAt();

    setX(x() + 0.00001);
    setY(y() + 0.00001);
    //    update();
}

bool GraphRelation::shouldBeenDropped(const QPointF &p)
{
    Q_UNUSED(p);

    return false;
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
    qreal halfGrip = Singleton<Settings>::instance().sizeOfGrip().width() / 2;

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

    return pp;
}

int GraphRelation::type() const
{
    return GraphRelationType;
}
