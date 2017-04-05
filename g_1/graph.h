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

struct Grip {
    static const QSizeF s_size;

    enum GripType {
        TOP_CENTER_GRIP,
        BOTTOM_CENTER_GRIP,
        LEFT_CENTER_GRIP,
        RIGHT_CENTER_GRIP,
        LEFT_TOP_GRIP,
        RIGHT_TOP_GRIP,
        LEFT_BOTTOM_GRIP,
        RIGHT_BOTTOM_GRIP
    };

    GripType m_type;
    QRectF m_rect;

    Grip(GripType t, const QRectF& r)
        : m_type(t), m_rect(r)
    {}

    Qt::CursorShape cursorShape() const;
//    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
//    HoverEventType hoverChanged(QGraphicsSceneHoverEvent *event);
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
    QList<Grip> m_grips;
    int m_currentGripIndex = -1;

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
    void bringToTop();
    void createGrips();
    void removeGrips();
    void drawGrips(QPainter *painter) const;

protected:
    virtual QPen defaultPen() const override;
    virtual QBrush defaultBrush() const override;
    virtual QFont defaultFont() const override;

    virtual void focusInEvent(QFocusEvent *event) override;
    virtual void focusOutEvent(QFocusEvent *event) override;
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;


public:
    GraphClass(const QPointF& p, const QSizeF s, const QString& name);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};

#endif // GRAPH_H
