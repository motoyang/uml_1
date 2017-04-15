#include <QtWidgets>
#include "defaultsettings.h"
#include "graph.h"

//
// Grip
//

Grip::Grip(Grip::GripType t)
    : m_type(t),
      m_rect(0, 0,
             Singleton<Settings>::instance().sizeOfGrip().width(),
             Singleton<Settings>::instance().sizeOfGrip().height())
{
}

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
        { Grip::RIGHT_BOTTOM_GRIP, Qt::SizeFDiagCursor },

        { Grip::SOURCE_GRIP, Qt::SizeAllCursor },
        { Grip::TARGET_GRIP, Qt::SizeAllCursor }

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
    case SOURCE_GRIP:
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
    case SOURCE_GRIP:
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
    case SOURCE_GRIP:
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
    case TARGET_GRIP:
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

Graph::Graph(const QPointF &p)
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

QPointF Graph::origin() const
{
    return QPointF(0.0, 0.0);
}

int Graph::type() const
{
    return GraphType;
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

QPointF Graph::centerPoint() const
{
    return boundingRect().center();
}

void Graph::setDroppedFlag(bool f)
{
    Q_UNUSED(f);
}

void Graph::drawGrips(QPainter *painter) const
{
    foreach (const auto& g, m_grips) {
        painter->drawEllipse(g->m_rect);
    }
}

void Graph::removeGrips()
{
    m_grips.clear();
}

void Graph::hoverOnGrip(const QPointF &p)
{
//    qDebug() << "hoverOnGrip" << p;

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
    if (event->button() == Qt::LeftButton) {
        m_bMouseLeftButtonPressed = true;
    }
    QGraphicsItem::mousePressEvent(event);
}

void Graph::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "mouseMoveEvent " << event->pos() << event->button();

    if (isGrippedState() && (event->buttons() & Qt::LeftButton)) {
        QPointF p = event->scenePos() - event->lastScenePos();
        resize(QSizeF(p.x(), p.y()));
    } else {
        qDebug() << "mouseMoveEvent222 " << event->pos();
        QGraphicsItem::mouseMoveEvent(event);
    }
}

void Graph::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_bMouseLeftButtonPressed = false;
        hoverOnGrip(event->pos());
    }

    QGraphicsItem::mouseReleaseEvent(event);
}

void Graph::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    setCursor(QCursor(Qt::OpenHandCursor));
    QGraphicsItem::hoverMoveEvent(event);
}

void Graph::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    if (!m_bMouseLeftButtonPressed) {
        m_currentGripIndex = std::numeric_limits<int>::max();
        setCursor(QCursor(Qt::ArrowCursor));
    }

    QGraphicsItem::hoverLeaveEvent(event);
}

void Graph::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    hoverOnGrip(event->pos());

    QGraphicsItem::hoverMoveEvent(event);
}
