#ifndef GRAPH_H
#define GRAPH_H

#include <QFont>
#include <QAbstractGraphicsShapeItem>
#include "entity.h"

struct TextPadding {
    qreal top;
    qreal left;
    qreal right;
    qreal bottom;
};

class Grip {
    enum GripType {
        TOP_GRIP,
        BOTTOM_GRIP,
        LEFT_GRIP,
        RIGHT_GRIP,
        LEFT_TOP_GRIP,
        RIGHT_TOP_GRIP,
        LEFT_BOTTOM_GRIP,
        RIGHT_BOTTOM_GRIP
    };

    GripType m_type;

public:
    Grip(GripType t)
        : m_type(t)
    {}

};

class Graph: public QGraphicsItem
{
    const int PEN_KEY = 1;
    const int BRUSH_KEY = 2;
    const int FONT_KEY = 3;

protected:
    static const TextPadding s_padding;

    QPointF m_pos;
    QSizeF m_size;
    QFont m_font;

    virtual QPen defaultPen() const = 0;
    virtual QBrush defaultBrush() const = 0;
    virtual QFont defaultFont() const = 0;

public:
    Graph(const QPointF& p, const QSizeF s);

    virtual QRectF boundingRect() const override;
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

    QPen pen() const;
    void setPen(const QPen& p);
    QBrush brush() const;
    void setBrush(const QBrush& b);
    QFont font() const;
    void setFont(const QFont& f);
};

class GraphClass: public Graph
{
    uml::Class m_c;

    QString m_name;
    QList<QString> m_operations;
    QList<QString> m_attributes;

    qreal m_nameXPos;

    QSizeF getSize(const QFont &f, const QString &s) const;
    QSizeF getSize(const QFont& f, const QList<QString>& l) const;
    void init();

protected:
    virtual QPen defaultPen() const override;
    virtual QBrush defaultBrush() const override;
    virtual QFont defaultFont() const override;

public:
    GraphClass(const QPointF& p, const QSizeF s, const QString& name);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};

#endif // GRAPH_H
