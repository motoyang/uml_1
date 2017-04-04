#include <algorithm>
#include <QPen>
#include <QBrush>
#include <QFontMetrics>
#include <QPainter>
#include <QApplication>

#include "graph.h"

TextPadding Graph::padding() const
{
    const TextPadding tp = {
        5, 5, 5, 5
    };

    return tp;
}

QFont Graph::font() const
{
    QFont f = qApp->font();
    return f;
}

QSize Graph::minMargin() const
{
    const QSize sz(10, 10);
    return sz;
}

Graph::Graph(const QPoint& p, const QSize s)
    :m_pos(p), m_size(s)
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

    init3();
}

void Graph::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QGraphicsPathItem::paint(painter, option, widget);



//    painter->setBrush(QBrush(Qt::darkYellow));
    if (isSelected()) {
        painter->setPen(Qt::darkBlue);
    } else {
        painter->setPen(Qt::darkGray);
    }

//    painter->drawText(30, 30, "aaaaaaa");

    QPen p = painter->pen();
    int halfWidthPen = p.width() / 2;
    QFontMetrics fm(painter->fontMetrics());

    QRect r(0, 0, m_size.width() + p.width(), m_size.height() + p.width() * 3);
    painter->drawRect(r);

    int x = halfWidthPen + padding().left + m_nameXPos;
    int y = halfWidthPen + padding().top + fm.ascent();
    painter->drawText(x, y, m_name);

    int yLine = padding().top + m_nameHeight + padding().bottom + p.width();
    painter->drawLine(0, yLine, m_size.width() + p.width(), yLine);

    x = halfWidthPen + padding().left;
    y += m_nameHeight + padding().bottom + p.width() + padding().top;
    foreach (const QString& s, m_attributes) {
        painter->drawText(x, y, s);
        y+= fm.lineSpacing();
    }

    yLine += p.width() + padding().top + padding().bottom + m_attributesHeight - fm.leading();
    painter->drawLine(0, yLine, m_size.width() + p.width(), yLine);

    y += halfWidthPen + padding().top;
    foreach (const QString& s, m_operations) {
        painter->drawText(x, y, s);
        y+= fm.lineSpacing();
    }

}

QRectF Graph::boundingRect() const
{
    qreal penWidth = 1;
    return QRectF(-10 - penWidth / 2, -10 - penWidth / 2,
                  20 + penWidth, 20 + penWidth);
}

void Graph::init3()
{
    setBrush(QBrush(Qt::darkYellow));
    setPos(m_pos);

    QFont f = font();
    QFontMetrics fm(f);

    // Name框的大小，其中width是可能的最小值
    QSize szName(fm.width(m_name), fm.height());
    m_size.setWidth(std::max(m_size.width(), szName.width()));
    m_nameHeight = szName.height();

    // Attributes框的大小，其中width是可能的最小值
//    int yPos1 = szName.height();
    QSize szAttributes = getSize2(m_attributes);
    m_size.setWidth(std::max(m_size.width(), szAttributes.width()));
    m_attributesHeight = szAttributes.height();

    // Operations框的大小，其中width是可能的最小值
//    int yPos2 = yPos1 + szAttributes.height();
    QSize szOperations = getSize2(m_operations);
    m_size.setWidth(std::max(m_size.width(), szOperations.width()));
    m_operationsHeight = szOperations.height();

    m_size.setHeight(szName.height() + szAttributes.height() + szOperations.height());

    m_nameXPos = (m_size.width() - szName.width()) / 2;

    QPainterPath pp = path();
    pp.addRect(0, 0, m_size.width() + 20 , m_size.height()+20);
//    pp.addText(10, 10, f, "asdasb");
    setPath(pp);
}

void Graph::init2()
{
    setBrush(QBrush(Qt::darkYellow));
    setPos(m_pos);

    QFont f = qApp->font();
    QFontMetrics fm = qApp->fontMetrics();
    QPainterPath pp = path();

    // Name框的大小，其中width是可能的最小值
    QSize szName(minMargin().width() + fm.width(m_name), minMargin().height() + fm.height());
    m_size.setWidth(std::max(m_size.width(), szName.width()));

    // Attributes框的大小，其中width是可能的最小值
    int yPos1 = szName.height();
    QSize szAttributes = getSize(m_attributes);
    m_size.setWidth(std::max(m_size.width(), szAttributes.width()));

    // Operations框的大小，其中width是可能的最小值
//    int yPos2 = yPos1 + szAttributes.height();
    QSize szOperations = getSize(m_operations);
    m_size.setWidth(std::max(m_size.width(), szOperations.width()));

    int height = szName.height() + szAttributes.height() + szOperations.height();
    m_size.setHeight(std::max(m_size.height(), height));
    pp.addRect(0, 0, m_size.width(), m_size.height());

/*
    int x = (m_size.width() - szName.width()) / 2;
    int y = minMargin().height() / 2 + fm.ascent();
    pp.addText(x, y, f, m_name);
    pp.moveTo(0, yPos1);
    pp.lineTo(m_size.width(), yPos1);

    x = minMargin().width() / 2;
    y = yPos1 + minMargin().height() / 2 + fm.ascent();
    foreach (const QString& s, m_attributes) {
        pp.addText(x, y, f, s);
        y+= fm.lineSpacing();
    }

    pp.moveTo(0, yPos2);
    pp.lineTo(m_size.width(), yPos2);
    y = yPos2 + minMargin().height() / 2 + fm.ascent();
    foreach (const QString& s, m_operations) {
        pp.addText(x, y, f, s);
        y+= fm.lineSpacing();
    }
*/
    setPath(pp);
}

QSize Graph::getSize(const QList<QString> l)
{
    QFont f = font();
    QFontMetrics fm(f);

    int width = minMargin().width();
    int height = minMargin().height();

    foreach (const QString& s, l) {
        width = std::max(fm.width(s), width);
        height += fm.lineSpacing();
    }
    if (m_attributes.size() > 0) {
        height -= fm.leading();
    }

    return QSize(width + minMargin().width(), height);
}

QSize Graph::getSize2(const QList<QString> l)
{
    QFontMetrics fm(font());
    int width = 0, height = 0;

    foreach (const QString& s, l) {
        width = std::max(fm.width(s), width);
        height += fm.lineSpacing();
    }
    if (m_attributes.size() > 0) {
        height -= fm.leading();
    }

    return QSize(width, height);
}

QSizeF Graph2::getSize(const QFont &f, const QString &s) const
{
    QFontMetrics fm(f);
    return QSizeF(fm.width(s), fm.height());
}

QSizeF Graph2::getSize(const QFont &f, const QList<QString> &l) const
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

void Graph2::init()
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

Graph2::Graph2(const QPointF &p, const QSizeF s)
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

TextPadding2 Graph2::s_padding = {5.0f, 5.0f, 5.0f, 5.0f};

void Graph2::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
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

QRectF Graph2::boundingRect() const
{
    qreal penHalfWidth = pen().widthF() / 2;
    return QRectF(0 - penHalfWidth, 0 - penHalfWidth, m_size.width()
                  + penHalfWidth, m_size.height() + penHalfWidth);
}
