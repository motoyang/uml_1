#include <algorithm>
#include <QMap>
#include <QPen>
#include <QBrush>
#include <QFontMetrics>
#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QApplication>
#include <QDebug>
#include "graph.h"

//
// Graph
//
Graph::Graph(const QPointF &p, const QSizeF s)
    : m_pos(p), m_size(s)
{
    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);
    setFlag(ItemIsFocusable);
    setFlag(ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);

    setZValue(-1);
}

const TextPadding Graph::s_padding = {5.0, 5.0, 5.0, 5.0};

QVariant Graph::itemChange(GraphicsItemChange change, const QVariant &value)
{
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
    return QGraphicsItem::itemChange(change, value);
}

QRectF Graph::boundingRect() const
{
    qreal penHalfWidth = pen().widthF() / 2;
    return QRectF(0 - penHalfWidth, 0 - penHalfWidth, m_size.width()
                  + penHalfWidth, m_size.height() + penHalfWidth);
}

QPen Graph::pen() const
{
    QVariant v = data(PEN_KEY);
    if (v.isNull()) {
        return defaultPen();
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
        return defaultBrush();
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
        return defaultFont();
    } else {
        return v.value<QFont>();
    }
}

void Graph::setFont(const QFont &f)
{
    setData(FONT_KEY, QVariant(f));
}

//
// GraphClass
//
QSizeF GraphClass::getSize(const QFont &f, const QString &s) const
{
    QFontMetrics fm(f);
    return QSizeF(fm.width(s), fm.height());
}

QSizeF GraphClass::getSize(const QFont &f, const QList<QString> &l) const
{
    QFontMetrics fm(f);
    qreal width = 0, height = 0;

    foreach (const QString& s, l) {
        width = std::max<qreal>(fm.width(s), width);
        height += fm.lineSpacing();
    }
    if (m_attributes.size() > 0) {
        height -= fm.leading();
    }

    return QSizeF(width, height);
}

void GraphClass::init()
{
    QSizeF szName = getSize(m_font, m_name);
    QSizeF szAttributes = getSize(m_font, m_attributes);
    qreal w = std::max<qreal>(szName.width(), szAttributes.width());
    QSizeF szOperations = getSize(m_font, m_operations);
    w = std::max(w, szOperations.width());

    qreal penWidth = pen().widthF();
    w = w + s_padding.left + s_padding.right + penWidth;
    qreal h = szName.height() + szAttributes.height() + szOperations.height()
            + penWidth * 3 + (s_padding.top + s_padding.bottom) * 3;

    if (w > m_size.width()) {
        m_size.setWidth(w);
    }
    if (h > m_size.height()) {
        m_size.setHeight(h);
    }
    m_nameXPos = (m_size.width() - szName.width()) / 2;
}

void GraphClass::bringToTop()
{
    QList<QGraphicsItem *> overlapItems = collidingItems();

    qreal zValue = 0;
    foreach (QGraphicsItem *item, overlapItems) {
        if (item->zValue() >= zValue)
            zValue = item->zValue() + 0.1;
    }
    setZValue(zValue);
}

void GraphClass::createGrips()
{
    QRectF br = boundingRect();
    QRectF gr(0.0, 0.0, Grip::s_size.width(), Grip::s_size.height());

    // 左上角的grip
    QPointF p(br.left(), br.top());
    gr.moveTopLeft(p);
    m_grips << Grip(Grip::LEFT_TOP_GRIP, gr);

    // 左边中间的grip
    p.setY(br.height() / 2 - gr.height() / 2);
    gr.moveTopLeft(p);
    m_grips << Grip(Grip::LEFT_CENTER_GRIP, gr);

    // 左下角的grip
    p.setY(br.height());
    gr.moveBottomLeft(p);
    m_grips << Grip(Grip::LEFT_BOTTOM_GRIP, gr);

    // 下边中间的grip
    p.setX(br.width() / 2 - gr.width() / 2);
    gr.moveBottomLeft(p);
    m_grips << Grip(Grip::BOTTOM_CENTER_GRIP, gr);

    // 右下角的grip
    p.setX(br.width());
    gr.moveBottomRight(p);
    m_grips << Grip(Grip::RIGHT_BOTTOM_GRIP, gr);

    // 右边中间的grip
    p.setY(br.height() / 2 - gr.height() / 2);
    gr.moveTopRight(p);
    m_grips << Grip(Grip::RIGHT_CENTER_GRIP, gr);

    // 右上角的grip
    p.setY(br.top());
    gr.moveTopRight(p);
    m_grips << Grip(Grip::RIGHT_TOP_GRIP, gr);

    // 上边中间的grip
    p.setX(br.width() / 2 - gr.width() / 2);
    gr.moveTopLeft(p);
    m_grips << Grip(Grip::TOP_CENTER_GRIP, gr);
}

void GraphClass::removeGrips()
{
    m_grips.clear();
}

void GraphClass::drawGrips(QPainter *painter) const
{
    foreach (const Grip& g, m_grips) {
        painter->drawRect(g.m_rect);
    }
}

QPen GraphClass::defaultPen() const
{
    static QPen p(Qt::darkBlue);
    return p;
}

QBrush GraphClass::defaultBrush() const
{
    static QBrush b(Qt::darkYellow);
    return b;
}

QFont GraphClass::defaultFont() const
{
    return qApp->font();
}

void GraphClass::focusInEvent(QFocusEvent *event)
{
    Q_UNUSED(event);
    qDebug() << m_name << "call focusInEvent";

    setAcceptHoverEvents(true);
    bringToTop();
    createGrips();
}

void GraphClass::focusOutEvent(QFocusEvent *event)
{
    Q_UNUSED(event);
    qDebug() << m_name << "call focusOutEvent";

    removeGrips();
    setAcceptHoverEvents(false);
}

void GraphClass::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << m_name << "mousePressEvent, " << event->scenePos() << " - " << event->lastScenePos();
    Graph::mousePressEvent(event);
}

void GraphClass::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << m_name << "mouseMoveEvent, " << event->scenePos() << " - " << event->lastScenePos();
    Graph::mouseMoveEvent(event);
}

void GraphClass::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << m_name << "mouseReleaseEvent, " << event->scenePos() << " - " << event->lastScenePos();
    Graph::mouseReleaseEvent(event);
}

void GraphClass::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    qDebug() << m_name << "hoverEnterEvent, " << event->scenePos() << " - " << event->lastScenePos();
    setCursor(QCursor(Qt::OpenHandCursor));
    Graph::hoverMoveEvent(event);
}

void GraphClass::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    qDebug() << m_name << "hoverLeaveEvent, " << event->scenePos() << " - " << event->lastScenePos();

    m_currentGripIndex = -1;
    setCursor(QCursor(Qt::ArrowCursor));
    Graph::hoverLeaveEvent(event);
}

void GraphClass::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
//    qDebug() << m_name << "hoverMoveEvent, " << event->scenePos() << " - " << event->lastScenePos();

    int i = 0;
    for (i = 0; i < m_grips.size(); ++i) {
        const Grip& g = m_grips.at(i);
        if (g.m_rect.contains(event->pos())) {
            qDebug() << g.m_rect << "contains " << event->pos() << "i is " << i;
            break;
        }
    }

    if (i >= m_grips.size()) {
        // 没有在grip中
        if (m_currentGripIndex < m_grips.size()) {
            // 原先在的grip中，则退出grip
            m_currentGripIndex = -1;
            setCursor(Qt::OpenHandCursor);
        }
    } else {
        // 如果鼠标在grip中
        if (m_currentGripIndex != i) {
            m_currentGripIndex = i;
            qDebug() << "m_currentGripIndex = " << m_currentGripIndex;
            setCursor(m_grips.at(m_currentGripIndex).cursorShape());
        }
    }

    Graph::hoverMoveEvent(event);
}

GraphClass::GraphClass(const QPointF &p, const QSizeF s, const QString& name)
    : Graph(p, s), m_c(name)
{
    m_name = m_c.name;
    m_attributes << "-attribute1: float" << "+att2" << "a3: string = abc" << "a4";
    m_operations << "#operation1(p1: int=3, p2: string)" << "+operation2()" << "-op3(p4: double)";

    init();
}

void GraphClass::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

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
    qreal y = halfWidthPen + s_padding.top + fm.ascent();
    painter->drawText(x, y, m_name);

    qreal yLine = s_padding.top + fm.height()+ s_padding.bottom + p.widthF();
    painter->drawLine(0, yLine, m_size.width(), yLine);

    x = halfWidthPen + s_padding.left;
    y += fm.height() + s_padding.bottom + p.widthF() + s_padding.top;
    foreach (const QString& s, m_attributes) {
        painter->drawText(x, y, s);
        y+= fm.lineSpacing();
        yLine += fm.lineSpacing();
    }

    yLine += p.widthF() + s_padding.top + s_padding.bottom - fm.leading();
    painter->drawLine(0, yLine, m_size.width(), yLine);

    y += p.widthF() + s_padding.bottom + s_padding.top;
    foreach (const QString& s, m_operations) {
        painter->drawText(x, y, s);
        y+= fm.lineSpacing();
    }

    drawGrips(painter);
}

const QSizeF Grip::s_size = {5, 5};

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
