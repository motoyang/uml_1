#ifndef GRAPH_H
#define GRAPH_H

#include <QFont>
#include <QAbstractGraphicsShapeItem>

struct TextPadding {
    qreal top;
    qreal left;
    qreal right;
    qreal bottom;
};

class Graph: public QAbstractGraphicsShapeItem
{
    static TextPadding s_padding;

    QPointF m_pos;
    QSizeF m_size;
    QFont m_font;

    QString m_name;
    QList<QString> m_operations;
    QList<QString> m_attributes;

    qreal m_nameXPos;

    QSizeF getSize(const QFont &f, const QString &s) const;
    QSizeF getSize(const QFont& f, const QList<QString>& l) const;
    void init();

public:
    Graph(const QPointF& p, const QSizeF s);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    QRectF boundingRect() const override;

};

class GraphClass: public Graph
{

};

#endif // GRAPH_H
