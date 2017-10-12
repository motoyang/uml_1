#include <QtWidgets>
#include "defaultsettings.h"
#include "fun.h"
//#include "graphrelation.h"
#include "link.h"
#include "graphclass.h"

//
// GraphClass
//
QSizeF GraphClass::getSize(const QString &s) const
{
    QFontMetrics fm(font());
    return QSizeF(fm.width(s), fm.height());
}

QSizeF GraphClass::getSize(const QList<QString> &l) const
{
    QFontMetrics fm(font());
    qreal width = 0, height = 0;

    for (const QString& s: l) {
        width = std::max<qreal>(fm.width(s), width);
        height += fm.lineSpacing();
    }

    // 减去最后一行的leading
    if (l.size() > 0) {
        height -= fm.leading();
    }

    return QSizeF(width, height);
}

void GraphClass::init()
{
    const TextPadding &tp = Singleton<Settings>::instance().textPadding();

    QSizeF szName = getSize(m_name);
    QSizeF szAttributes = getSize(m_attributes);
    qreal w = std::max<qreal>(szName.width(), szAttributes.width());
    QSizeF szOperations = getSize(m_operations);
    w = std::max(w, szOperations.width());

    qreal penWidth = pen().widthF();
    w = w + tp.left + tp.right + penWidth;
    qreal h = szName.height() + szAttributes.height() + szOperations.height()
            + penWidth * 3 + (tp.top + tp.bottom) * 3;
    m_minSize.setWidth(w);
    m_minSize.setHeight(h);

    if (w > m_size.width()) {
        m_size.setWidth(w);
    }
    if (h > m_size.height()) {
        m_size.setHeight(h);
    }

    placeName();
}

void GraphClass::placeName()
{
    m_nameXPos = (m_size.width() - getSize(m_name).width()) / 2;
}

void GraphClass::drawDropped(QPainter *painter)
{
    if (!m_bDroppedVisible) {
        return;
    }

    qreal x = m_droppedPoint.x() * m_size.width();
    qreal y = m_droppedPoint.y() * m_size.height();

    const QSizeF &droppedSize = Singleton<Settings>::instance().sizeOfDropped();
    QRectF r(x - droppedSize.width() / 2, y - droppedSize.height() / 2,
             droppedSize.width(), droppedSize.height());
    painter->drawEllipse(r);
}

void GraphClass::relationsUpdate() const
{
    foreach (auto r, m_sources.keys()) {
        const_cast<Link*>(r)->linkSourceGraph(this);
    }
    foreach (auto r, m_targets.keys()) {
        const_cast<Link*>(r)->linkTargetGraph(this);
    }
}

void GraphClass::resizeByGrip(const QSizeF &diff)
{
    QSizeF szNew = m_size;
    auto g = *m_selectedGrip;
    g->pretreateSize(diff, szNew);

    prepareGeometryChange();
    if (szNew.width() >= m_minSize.width()) {
        m_size.setWidth(szNew.width());
        if (g->shouldChangeXPos()) {
            setX(x() + diff.width());
        }
    }
    if (szNew.height() > m_minSize.height()) {
        m_size.setHeight(szNew.height());
        if (g->shouldChangeYPos()) {
            setY(y() + diff.height());
        }
    }

    berthGripsAt();
}


void GraphClass::setDroppedFlag(bool f)
{
//    prepareGeometryChange();

    m_bDroppedVisible = f;
//    update();
}

void GraphClass::showDroppedPoint(const QPointF &p)
{
    prepareGeometryChange();

    qreal x = p.x() / m_size.width();
    qreal y = p.y() / m_size.height();

    if ((x > 0.33 && x < 0.66)
     && (y > 0.33 && y < 0.66)) {
        // 中心点
        m_droppedPoint.setX(0.5);
        m_droppedPoint.setY(0.5);
    } else {
        // 四个边
        if (x > y) {
            // 上边和右边
            if (x + y > 1.0) {
                // 右边
                m_droppedPoint.setX(1.0);
                m_droppedPoint.setY(y);
            } else {
                // 上边
                m_droppedPoint.setX(x);
                m_droppedPoint.setY(0.0);
            }
        } else {
            // 下边和左边
            if (x + y < 1.0) {
                // 左边
                m_droppedPoint.setX(0.0);
                m_droppedPoint.setY(y);
            } else {
                // 下边
                m_droppedPoint.setX(x);
                m_droppedPoint.setY(1.0);
            }
        }
    }
//    update();
}

bool GraphClass::sourceDroppedPoint(const Link *l, QPointF &p) const
{
    Q_ASSERT(m_sources.find(l) != m_sources.end());

    bool b = false;
    p = m_sources.value(l);
    if (isEqual(p.x(), 0.5) && isEqual(p.y(), 0.5)) {
        b = true;
    }

    p.rx() *= m_size.width();
    p.ry() *= m_size.height();
    return b;
}

bool GraphClass::targetDroppedPoint(const Link *l, QPointF &p) const
{
    Q_ASSERT(m_targets.find(l) != m_targets.end());

    bool b = false;
    p = m_targets.value(l);
    if (isEqual(p.x(), 0.5) && isEqual(p.y(), 0.5)) {
        b = true;
    }

    p.rx() *= m_size.width();
    p.ry() *= m_size.height();
    return b;
}

QPainterPath GraphClass::bodyPath() const
{
    QPainterPath pp;
    pp.addRect(QRectF(0, 0, m_size.width(), m_size.height()));
    return pp;
}
/*
void GraphClass::connectTarget(const GraphRelation *r)
{
    m_t.append(const_cast<GraphRelation*>(r));
}

void GraphClass::disconnectTarget(const GraphRelation* const g)
{
    bool b = m_t.removeOne(const_cast<GraphRelation* const>(g));
    Q_ASSERT_X(b, __FUNCTION__, "err");
}

void GraphClass::connectSource(const GraphRelation *r)
{
    m_s.append(const_cast<GraphRelation*>(r));
}

void GraphClass::disconnectSource(const GraphRelation* const g)
{
    bool b = m_s.removeOne(const_cast<GraphRelation* const>(g));
    Q_ASSERT_X(b, __FUNCTION__, "err");
}
*/


void GraphClass::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    BaseClass::mouseMoveEvent(event);
    relationsUpdate();
}

void GraphClass::connectTarget(const Link *r)
{
    Q_ASSERT(m_targets.find(r) == m_targets.end());

    m_targets[r] = m_droppedPoint;
//    m_targets.append(const_cast<Line*>(r));
}

void GraphClass::disconnectTarget(const Link* const g)
{
    bool b = m_targets.remove(g);
//    bool b = m_targets.removeOne(const_cast<Line* const>(g));
    Q_ASSERT_X(b, __FUNCTION__, "err");
}

void GraphClass::connectSource(const Link *r)
{
    Q_ASSERT(m_sources.find(r) == m_sources.end());

    m_sources[r] = m_droppedPoint;
//    m_sources.append(const_cast<Line*>(r));
}

void GraphClass::disconnectSource(const Link* const g)
{
    bool b = m_sources.remove(g);
//    bool b = m_sources.removeOne(const_cast<Line* const>(g));
    Q_ASSERT_X(b, __FUNCTION__, "err");
}

GraphClass::GraphClass(const QPointF &p, const QSizeF s, const QString& name)
    : Graph(p), m_c(name), m_size(s)
{
    m_name = m_c.name;
    m_attributes << "-attribute1: float" << "+att2" << "a3: string = abc" << "a4";
    m_operations << "#operation1(p1: int=3, p2: string)" << "+operation2()" << "-op3(p4: double)";

    setAcceptDrops(true);

    init();
}

QPainterPath GraphClass::shape() const
{
    QPainterPath pp;
    pp.addRect(boundingRect());
    return pp;
}

QRectF GraphClass::boundingRect() const
{
    qreal halfW = Singleton<Settings>::instance().sizeOfGrip().width() / 2 + pen().width();
    qreal halfH = Singleton<Settings>::instance().sizeOfGrip().height() / 2 + pen().width();
    QRectF r(0, 0, m_size.width(), m_size.height());
    return r.adjusted(0 - halfW, 0 - halfH, halfW, halfH);
}

void GraphClass::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    const TextPadding &tp = Singleton<Settings>::instance().textPadding();

    painter->setPen(pen());
    painter->setBrush(brush());
    painter->setFont(font());

    QPen p = painter->pen();
    qreal halfWidthPen = p.widthF() / 2;
    QFontMetrics fm(painter->fontMetrics());

    QRectF r(0.0, 0.0, m_size.width(), m_size.height());
    painter->fillRect(r, brush());
    painter->drawRect(r);

    qreal x = m_nameXPos;
    qreal y = halfWidthPen + tp.top + fm.ascent();
    painter->drawText(x, y, m_name);

    qreal yLine = tp.top + fm.height()+ tp.bottom + p.widthF();
    painter->drawLine(0, yLine, m_size.width(), yLine);

    x = halfWidthPen + tp.left;
    y += fm.height() + tp.bottom + p.widthF() + tp.top;
    for (const QString& s: m_attributes) {
        painter->drawText(x, y, s);
        y+= fm.lineSpacing();
        yLine += fm.lineSpacing();
    }

    yLine += p.widthF() + tp.top + tp.bottom - fm.leading();
    painter->drawLine(0, yLine, m_size.width(), yLine);

    y += p.widthF() + tp.bottom + tp.top;
    for (const QString& s: m_operations) {
        painter->drawText(x, y, s);
        y+= fm.lineSpacing();
    }

//    painter->drawPath(shape());
    drawGrips(painter);
    drawDropped(painter);
}

int GraphClass::type() const
{
    return GraphClassType;
}

void GraphClass::createGrips()
{
    if (!m_grips.isEmpty()) {
        Q_ASSERT(false);
        return;
    }

    m_grips << QSharedPointer<Grip>::create(Grip::LEFT_TOP_GRIP)
            << QSharedPointer<Grip>::create(Grip::LEFT_CENTER_GRIP)
            << QSharedPointer<Grip>::create(Grip::LEFT_BOTTOM_GRIP)
            << QSharedPointer<Grip>::create(Grip::BOTTOM_CENTER_GRIP)
            << QSharedPointer<Grip>::create(Grip::RIGHT_BOTTOM_GRIP)
            << QSharedPointer<Grip>::create(Grip::RIGHT_CENTER_GRIP)
            << QSharedPointer<Grip>::create(Grip::RIGHT_TOP_GRIP)
            << QSharedPointer<Grip>::create(Grip::TOP_CENTER_GRIP);

    m_selectedGrip = m_grips.end();
}

void GraphClass::berthGripsAt()
{
    const QSizeF &szGrip = Singleton<Settings>::instance().sizeOfGrip();

    QRectF br(0, 0, m_size.width(), m_size.height());
    QRectF gr(0.0, 0.0, szGrip.width(), szGrip.height());

    foreach (const auto& g, m_grips) {
        switch (g->m_type) {
        case Grip::LEFT_TOP_GRIP:
            // 左上角的grip
            g->m_rect.moveCenter(QPointF(br.left(), br.top()));
            break;

        case Grip::LEFT_CENTER_GRIP:
            // 左边中间的grip
            g->m_rect.moveCenter(QPointF(br.left(), br.height() / 2));
            break;

        case Grip::LEFT_BOTTOM_GRIP:
            // 左下角的grip
            g->m_rect.moveCenter(QPointF(br.left(), br.height()));
            break;

        case Grip::BOTTOM_CENTER_GRIP:
            // 下边中间的grip
            g->m_rect.moveCenter(QPointF(br.width() / 2, br.height()));
            break;

        case Grip::RIGHT_BOTTOM_GRIP:
            // 右下角的grip
            g->m_rect.moveCenter(QPointF(br.width(), br.height()));
            break;

        case Grip::RIGHT_CENTER_GRIP:
            // 右边中间的grip
            g->m_rect.moveCenter(QPointF(br.width(), br.height() / 2));
            break;

        case Grip::RIGHT_TOP_GRIP:
            // 右上角的grip
            g->m_rect.moveCenter(QPointF(br.width(), br.top()));
            break;

        case Grip::TOP_CENTER_GRIP:
            // 上边中间的grip
            g->m_rect.moveCenter(QPointF(br.width() / 2, br.top()));
            break;

        default:
            Q_ASSERT(false);
        }
    }

    // 更新名称的位置
    placeName();
}
