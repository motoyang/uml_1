#include <algorithm>
#include <QPen>
#include <QBrush>
#include <QFontMetrics>
#include <QApplication>

#include "graph.h"

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
    setFlag(ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);
    //    setZValue(-1);

    m_name = "classname1";
    m_attributes << "-attribute1: float" << "+att2" << "a3: string = abc" << "a4";
    m_operations << "#operation1(p1: int=3, p2: string)" << "+operation2()" << "-op3(p4: double)";

    init2();
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
    int yPos2 = yPos1 + szAttributes.height();
    QSize szOperations = getSize(m_operations);
    m_size.setWidth(std::max(m_size.width(), szOperations.width()));

    int height = szName.height() + szAttributes.height() + szOperations.height();
    m_size.setHeight(std::max(m_size.height(), height));
    pp.addRect(0, 0, m_size.width(), m_size.height());

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
