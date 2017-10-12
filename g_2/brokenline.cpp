#include <QtWidgets>
#include "defaultsettings.h"
#include "intersected.h"
#include "brokenline.h"

//
// BrokenLine
//
QPainterPath BrokenLine::path() const
{
    QPainterPath pp;
    QLineF l(startPoint(), closestPointToSource());
    pp = diamondPath(l);

    for(auto p = m_points.cbegin(); p < m_points.cend(); ++p) {
        if (p == m_points.cbegin()) {
//            pp.moveTo(**p);
        } else if (p == m_points.cend() - 1) {

        } else {
            pp.lineTo(**p);
        }
    }

    l.setPoints(closestPointToTarget(), endPoint());
    QPainterPath pp2 = trianglePath(l);
    pp.connectPath(pp2);

    return pp;
}

bool BrokenLine::simplify()
{
    Q_ASSERT(isGrippedState());

    auto g = *m_selectedGrip;
    int index  = indexOfGrip();
    if (g->m_type != Grip::INTERSECTED_GRIP
     && g->m_type != Grip::BROKEN_GRIP) {
        return false;
    }

    QSharedPointer<QPointF> p1 = m_points.at(index - 1);
    QSharedPointer<QPointF> p = m_points.at(index);
    QSharedPointer<QPointF> p2 = m_points.at(index + 1);

    QLineF l(*p1, *p2);
    QLineF l1(*p1, *p);
    QLineF l2(*p, *p2);

    bool b = false;
    if ((l1.length() + l2.length()) < (l.length() * 1.01)) {
        m_points.removeAt(index);
        b = true;

        removeGrips();
        createGrips();
        berthGripsAt();
    }

    return b;
}

void BrokenLine::brokenAt(const QPointF &pos)
{
    prepareGeometryChange();

    QPointF p1, p2;
    for (int i = 0; i < m_points.size(); ++i) {
        if (i == 0) {
            p2 = *m_points.at(i);
            continue;
        }
        p1 = p2;
        p2 = *m_points.at(i);

        QPainterPath pp;
        pp.moveTo(p1);
        pp.lineTo(p2);

        QPainterPathStroker pps;
        pps.setWidth(Singleton<Settings>::instance().widthOfShape());

        QPainterPath pp2 = pps.createStroke(pp);
        if (pp2.contains(pos)) {
            m_points.insert(i, QSharedPointer<QPointF>::create(pos));
            removeGrips();
            createGrips();
            berthGripsAt();
            hoverOnGrip(pos);

            break;
        }
    }
}

void BrokenLine::createGrips()
{
    if (!m_grips.isEmpty()) {
        Q_ASSERT(false);
        return;
    }

    Grip::GripType gt;
    for (int i = 0; i < m_points.size(); ++i) {
        if (i == 0) {
            gt = Grip::SOURCE_GRIP;
        } else if (i == m_points.size() - 1) {
            gt = Grip::TARGET_GRIP;
        } else {
            gt = Grip::INTERSECTED_GRIP;
        }
        auto g = QSharedPointer<Grip>::create(gt);
        m_grips << g;
    }

    m_selectedGrip = m_grips.end();
}

void BrokenLine::berthGripsAt()
{
    auto p = m_points.cbegin();
    foreach (const auto& g, m_grips) {
        g->m_rect.moveCenter(**p);
        ++p;
    }
}

void BrokenLine::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (isLocked() && !isGrippedState()) {
        QPointF pos = event->pos();
        brokenAt(pos);
    }

    BaseClass::mousePressEvent(event);
}
/*
void BrokenLine::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    BaseClass::mouseMoveEvent(event);
}
*/
void BrokenLine::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (isGrippedState()) {
        simplify();
    }

    BaseClass::mouseReleaseEvent(event);
}

void BrokenLine::resizeByGrip(const QSizeF &diff)
{
    prepareGeometryChange();

    int index = indexOfGrip();
    QSharedPointer<QPointF> p = m_points.at(index);
    p->rx() += diff.width();
    p->ry() += diff.height();

    berthGripsAt();
}

QPointF BrokenLine::startPoint() const
{
    Q_ASSERT(m_points.size() > 0);

    return *m_points.first();
}

QPointF BrokenLine::endPoint() const
{
    Q_ASSERT(m_points.size() > 0);

    return *m_points.last();
}

void BrokenLine::setStartPoint(const QPointF &p)
{
    Q_ASSERT(m_points.size() > 0);

    prepareGeometryChange();
    QSharedPointer<QPointF> pF = m_points.first();
    pF->setX(p.x());
    pF->setY(p.y());
}

void BrokenLine::setEndPoint(const QPointF &p)
{
    Q_ASSERT(m_points.size() > 0);

    prepareGeometryChange();
    QSharedPointer<QPointF> pL = m_points.last();
    pL->setX(p.x());
    pL->setY(p.y());
}

QPointF BrokenLine::closestPointToTarget() const
{
    Q_ASSERT(m_points.size() >= 2);

    auto p = m_points.at(m_points.size() - 2);
    return *p;
}

QPointF BrokenLine::closestPointToSource() const
{
    Q_ASSERT(m_points.size() >= 2);

    auto p = m_points.at(1);
    return *p;
}

BrokenLine::BrokenLine(const QPointF &p)
    : BaseClass(p)
{
    m_points << QSharedPointer<QPointF>::create(0.0, 0.0)
             << QSharedPointer<QPointF>::create(30.0, 0.0)
             << QSharedPointer<QPointF>::create(100, 80);
}

int BrokenLine::type() const
{
    return BrokenLineType;
}

QRectF BrokenLine::boundingRect() const
{
    QSizeF s = Singleton<Settings>::instance().sizeOfExtended();
    return path().simplified().boundingRect()
            .adjusted(0 - s.width() / 2 , 0 - s.height() / 2,
                      s.width() / 2, s.height() / 2);

    QPointF topLeft, bottomRight;
    foreach (auto p, m_points) {
        topLeft.setX(std::min(topLeft.x(), p->x()));
        topLeft.setY(std::min(topLeft.y(), p->y()));
        bottomRight.setX(std::max(bottomRight.x(), p->x()));
        bottomRight.setY(std::max(bottomRight.y(), p->y()));
    }

    return QRectF(topLeft, bottomRight)
            .normalized()
            .adjusted(0 - s.width(), 0 - s.height(), s.width(), s.height());
}

void BrokenLine::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

//    painter->setPen(pen());
//    painter->setBrush(brush());
//    painter->setFont(font());

    painter->drawPath(path());
//    painter->drawPath(shape());

    drawGrips(painter);
}

QPainterPath BrokenLine::shape() const
{
    QPainterPathStroker pps;
    pps.setWidth(Singleton<Settings>::instance().widthOfShape());

    return pps.createStroke(path()).simplified();
}
