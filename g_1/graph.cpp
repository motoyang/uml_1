//#include <algorithm>
//#include <QMap>
//#include <QPen>
//#include <QBrush>
//#include <QFontMetrics>
//#include <QGraphicsScene>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QApplication>
//#include <QDebug>
#include "graph.h"

//
// Grip
//
const QSizeF Grip::s_size = {6, 6};

Qt::CursorShape Grip::cursorShape() const
{
    static const QMap<int, Qt::CursorShape> cs = {
        { Grip::TOP_CENTER_GRIP, Qt::SizeVerCursor },
        { Grip::BOTTOM_CENTER_GRIP, Qt::SizeVerCursor },
        { Grip::LEFT_CENTER_GRIP, Qt::SizeHorCursor },
        { Grip::RIGHT_CENTER_GRIP, Qt::SizeHorCursor },
        { Grip::LEFT_TOP_GRIP, Qt::SizeFDiagCursor },
        { Grip::RIGHT_TOP_GRIP, Qt::SizeBDiagCursor },
        { Grip::LEFT_BOTTOM_GRIP, Qt::SizeBDiagCursor },
        { Grip::RIGHT_BOTTOM_GRIP, Qt::SizeFDiagCursor }
    };

    return cs.value(m_type);
}

bool Grip::shouldChangeYPos() const
{
    bool bRet = false;

    switch (m_type) {
    case LEFT_TOP_GRIP:
    case TOP_CENTER_GRIP:
    case RIGHT_TOP_GRIP:
        bRet = true;
        break;
    default:
        break;
    }

    return bRet;
}

bool Grip::shouldChangeXPos() const
{
    bool bRet = false;

    switch (m_type) {
    case LEFT_TOP_GRIP:
    case LEFT_CENTER_GRIP:
    case LEFT_BOTTOM_GRIP:
        bRet = true;
        break;
    default:
        break;
    }

    return bRet;
}

void Grip::pretreateSize(const QSizeF &diff, QSizeF &sz) const
{
    switch (m_type) {
    case LEFT_TOP_GRIP:
        sz -= diff;
        break;
    case LEFT_CENTER_GRIP:
        sz.setWidth(sz.width() - diff.width());
        break;
    case LEFT_BOTTOM_GRIP:
        sz.setWidth(sz.width() - diff.width());
        sz.setHeight(sz.height() + diff.height());
        break;
    case BOTTOM_CENTER_GRIP:
        sz.setHeight(sz.height() + diff.height());
        break;
    case RIGHT_BOTTOM_GRIP:
        sz += diff;
        break;
    case RIGHT_CENTER_GRIP:
        sz.setWidth(sz.width() + diff.width());
        break;
    case RIGHT_TOP_GRIP:
        sz.setWidth(sz.width() + diff.width());
        sz.setHeight(sz.height() - diff.height());
        break;
    case TOP_CENTER_GRIP:
        sz.setHeight(sz.height() - diff.height());
        break;
    default:
        Q_ASSERT(false);
        break;
    }
}


//
// Graph
//

const TextPadding Graph::s_padding = {5.0, 5.0, 5.0, 5.0};

Graph::Graph(const QPointF &p, const QSizeF s)
    : m_size(s)
{
    setX(p.x());
    setY(p.y());

    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);
    setFlag(ItemIsFocusable);
    setFlag(ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);

    setZValue(-1);
}

void Graph::bringToTop()
{
    QList<QGraphicsItem *> overlapItems = collidingItems();

    qreal zValue = 0;
    foreach (QGraphicsItem *item, overlapItems) {
        if (item->zValue() >= zValue)
            zValue = item->zValue() + 0.1;
    }
    setZValue(zValue);
}
QRectF Graph::boundingRect() const
{
    qreal penHalfWidth = pen().widthF() / 2;
    return QRectF(0 - penHalfWidth, 0 - penHalfWidth, m_size.width()
                  + penHalfWidth *2, m_size.height() + penHalfWidth*2);
}

QVariant Graph::itemChange(GraphicsItemChange change, const QVariant &value)
{
/*
    if (change == ItemPositionChange && scene()) {
        // value is the new position.
        QPointF newPos = value.toPointF();
        QRectF rect = scene()->sceneRect();
        if (!rect.contains(newPos)) {
            // Keep the item inside the scene rect.
            newPos.setX(qMin(rect.right(), qMax(newPos.x(), rect.left())));
            newPos.setY(qMin(rect.bottom(), qMax(newPos.y(), rect.top())));
            return newPos;
        }
    }
*/
    return QGraphicsItem::itemChange(change, value);
}

QPen Graph::pen() const
{
    QVariant v = data(PEN_KEY);
    if (v.isNull()) {
        return QPen(Qt::darkBlue);;
    } else {
        return v.value<QPen>();
    }
}

void Graph::setPen(const QPen &p)
{
    setData(PEN_KEY, QVariant(p));
}

QBrush Graph::brush() const
{
    QVariant v = data(BRUSH_KEY);
    if (v.isNull()) {
        return QBrush(Qt::darkYellow);
    } else {
        return v.value<QBrush>();
    }
}

void Graph::setBrush(const QBrush &b)
{
    setData(BRUSH_KEY, QVariant(b));
}

QFont Graph::font() const
{
    QVariant v = data(FONT_KEY);
    if (v.isNull()) {
        return qApp->font();
    } else {
        return v.value<QFont>();
    }
}

void Graph::setFont(const QFont &f)
{
    setData(FONT_KEY, QVariant(f));
}

void Graph::drawGrips(QPainter *painter) const
{
    foreach (const auto& g, m_grips) {
        painter->drawRect(g->m_rect);
    }
}

void Graph::removeGrips()
{
    m_grips.clear();
}

void Graph::hoverOnGrip(const QPointF &p)
{
    int i = 0;
    for (i = 0; i < m_grips.size(); ++i) {
        const auto& g = m_grips.at(i);
        if (g->m_rect.contains(p)) {
            break;
        }
    }

    if (i >= m_grips.size()) {
        // 如果鼠标没有在grip中
        if (m_currentGripIndex < m_grips.size()) {
            // 原先在grip中，则退出grip
            m_currentGripIndex = std::numeric_limits<int>::max();
            setCursor(Qt::OpenHandCursor);
        }
    } else {
        // 如果鼠标在grip中
        if (m_currentGripIndex != i) {
            m_currentGripIndex = i;
            setCursor(m_grips.at(m_currentGripIndex)->cursorShape());
        }
    }
}

bool Graph::isGrippedState() const
{
    return m_currentGripIndex < m_grips.size();
}

void Graph::resize(const QSizeF &s)
{
    QSizeF szNew = m_size;
    auto g = m_grips.at(m_currentGripIndex);
    g->pretreateSize(s, szNew);

    if (szNew.width() >= m_minSize.width()) {
        m_size.setWidth(szNew.width());
        if (g->shouldChangeXPos()) {
            setX(x() + s.width());
        }
    }
    if (szNew.height() > m_minSize.height()) {
        m_size.setHeight(szNew.height());
        if (g->shouldChangeYPos()) {
            setY(y() + s.height());
        }
    }

    berthGripsAt();

    // 假装更新位置，强迫scene更新背景。否则右下角grip拖动缩小时有残影！！！
    if (!g->shouldChangeXPos() && !g->shouldChangeYPos()) {
        setX(x() + 0.000001);
    }
}

void Graph::focusInEvent(QFocusEvent *event)
{
    setAcceptHoverEvents(true);

    createGrips();
    berthGripsAt();

    bringToTop();

    QGraphicsItem::focusInEvent(event);
}

void Graph::focusOutEvent(QFocusEvent *event)
{
    removeGrips();

    setAcceptHoverEvents(false);

    QGraphicsItem::focusOutEvent(event);
}

void Graph::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mousePressEvent(event);
}

void Graph::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (isGrippedState() && event->buttons() & Qt::LeftButton) {
        QPointF p = event->scenePos() - event->lastScenePos();
        resize(QSizeF(p.x(), p.y()));
    } else {
        QGraphicsItem::mouseMoveEvent(event);
    }
}

void Graph::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseReleaseEvent(event);
}

void Graph::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    setCursor(QCursor(Qt::OpenHandCursor));
    QGraphicsItem::hoverMoveEvent(event);
}

void Graph::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    m_currentGripIndex = std::numeric_limits<int>::max();
    setCursor(QCursor(Qt::ArrowCursor));
    QGraphicsItem::hoverLeaveEvent(event);
}

void Graph::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    hoverOnGrip(event->pos());

    QGraphicsItem::hoverMoveEvent(event);
}

