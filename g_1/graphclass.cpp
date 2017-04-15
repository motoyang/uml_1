#include <QtWidgets>
#include "defaultsettings.h"
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
    if (m_bDroppedVisible) {
        painter->drawEllipse(boundingRect());
    }
}

void GraphClass::resize(const QSizeF &diff)
{
    QSizeF szNew = m_size;
    auto g = m_grips.at(m_currentGripIndex);
    g->pretreateSize(diff, szNew);

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

    // 假装更新位置，强迫scene更新背景。否则右下角grip拖动缩小时有残影！！！
    if (!g->shouldChangeXPos() && !g->shouldChangeYPos()) {
        setX(x() + 0.000001);
    }
}

bool GraphClass::shouldBeenDropped(const QPointF &p)
{
    return shape().contains(mapFromScene(p));
}

void GraphClass::setDroppedFlag(bool f)
{
    m_bDroppedVisible = f;
    update();
}

void GraphClass::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    qDebug() << "dragEnterEvent" << event->dropAction();
}

void GraphClass::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
    qDebug() << "dragLeaveEvent" << event->dropAction();
}

void GraphClass::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    qDebug() << "dragMoveEvent" << event->dropAction();
}

void GraphClass::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    qDebug() << "dropEvent" << event->dropAction();
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
    pp.addRect(0, 0, m_size.width(), m_size.height());
    return pp;
}

QRectF GraphClass::boundingRect() const
{
    qreal penHalfWidth = pen().widthF() / 2;
    return QRectF(0 - penHalfWidth, 0 - penHalfWidth, m_size.width()
                  + penHalfWidth *2, m_size.height() + penHalfWidth*2);
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
    foreach (const QString& s, m_attributes) {
        painter->drawText(x, y, s);
        y+= fm.lineSpacing();
        yLine += fm.lineSpacing();
    }

    yLine += p.widthF() + tp.top + tp.bottom - fm.leading();
    painter->drawLine(0, yLine, m_size.width(), yLine);

    y += p.widthF() + tp.bottom + tp.top;
    foreach (const QString& s, m_operations) {
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
            g->m_rect.moveTopLeft(QPointF(br.left(), br.top()));
            break;

        case Grip::LEFT_CENTER_GRIP:
            // 左边中间的grip
            g->m_rect.moveTopLeft(QPointF(br.left(), br.height() / 2 - gr.height() / 2));
            break;

        case Grip::LEFT_BOTTOM_GRIP:
            // 左下角的grip
            g->m_rect.moveBottomLeft(QPointF(br.left(), br.height()));
//            g->m_rect.moveTopLeft(QPointF(br.left(), br.height() - gr.height()));
            break;

        case Grip::BOTTOM_CENTER_GRIP:
            // 下边中间的grip
            g->m_rect.moveBottomLeft(QPointF(br.width() / 2 - gr.width() / 2, br.height()));
            break;

        case Grip::RIGHT_BOTTOM_GRIP:
            // 右下角的grip
            g->m_rect.moveBottomRight(QPointF(br.width(), br.height()));
            break;

        case Grip::RIGHT_CENTER_GRIP:
            // 右边中间的grip
            g->m_rect.moveTopRight(QPointF(br.width(), br.height() / 2 - gr.height() / 2));
            break;

        case Grip::RIGHT_TOP_GRIP:
            // 右上角的grip
            g->m_rect.moveTopRight(QPointF(br.width(), br.top()));
            break;

        case Grip::TOP_CENTER_GRIP:
            // 上边中间的grip
            g->m_rect.moveTopLeft(QPointF(br.width() / 2 - gr.width() / 2, br.top()));
            break;

        default:
            Q_ASSERT(false);
        }
    }

    // 更新名称的位置
    placeName();
}
