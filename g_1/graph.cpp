#include <algorithm>
#include <QPen>
#include <QBrush>
#include <QFontMetrics>
#include <QPainter>
#include <QApplication>
#include "graph.h"

QSizeF Graph::getSize(const QFont &f, const QString &s) const
{
    QFontMetrics fm(f);
    return QSizeF(fm.width(s), fm.height());
}

QSizeF Graph::getSize(const QFont &f, const QList<QString> &l) const
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

void Graph::init()
{
    setBrush(QBrush(Qt::darkGreen));

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
    m_nameXPos = (m_size.width() - szName.width()) / 2 + s_padding.left + (penWidth / 2);
}

Graph::Graph(const QPointF &p, const QSizeF s)
    : m_pos(p), m_size(s)
{
    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);
//    setFlag(ItemIsFocusable);
    setFlag(ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);
    //    setZValue(-1);

    m_name = "classname1";
    m_attributes << "-attribute1: float" << "+att2" << "a3: string = abc" << "a4";
    m_operations << "#operation1(p1: int=3, p2: string)" << "+operation2()" << "-op3(p4: double)";

    init();
}

TextPadding Graph::s_padding = {5.0, 5.0, 5.0, 5.0};

void Graph::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

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
}

QRectF Graph::boundingRect() const
{
    qreal penHalfWidth = pen().widthF() / 2;
    return QRectF(0 - penHalfWidth, 0 - penHalfWidth, m_size.width()
                  + penHalfWidth, m_size.height() + penHalfWidth);
}
