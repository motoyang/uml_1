#ifndef GRAPH_H
#define GRAPH_H

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
        RIGHT_BOTTOM_GRIP,

        SOURCE_GRIP,
        TARGET_GRIP
    };

    GripType m_type;
    QRectF m_rect;

    Grip(GripType t)
        : m_type(t), m_rect(0, 0, s_size.width(), s_size.height())
    {}

    Qt::CursorShape cursorShape() const;
    bool shouldChangeYPos() const;
    bool shouldChangeXPos() const;
    void pretreateSize(const QSizeF& diff, QSizeF& sz) const;

};

class Graph: public QGraphicsItem
{
    const int PEN_KEY = 1;
    const int BRUSH_KEY = 2;
    const int FONT_KEY = 3;

    void bringToTop();

protected:
    static const TextPadding s_padding;

    bool m_bMouseLeftButtonPressed = false;
    QList<QSharedPointer<Grip>> m_grips;
    int m_currentGripIndex = std::numeric_limits<int>::max();

    // 子类要实现
    virtual void resize(const QSizeF &diff) = 0;

    // grip相关的操作，子类可以override或实现这些操作。
    virtual void createGrips() = 0;
    virtual void berthGripsAt() = 0;
    virtual void drawGrips(QPainter *painter) const;
    virtual void removeGrips();
    virtual void hoverOnGrip(const QPointF& p);
    virtual bool isGrippedState() const;

    // from QGraphicsItem
    virtual void focusInEvent(QFocusEvent *event) override;
    virtual void focusOutEvent(QFocusEvent *event) override;
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;

public:
    Graph(const QPointF& p);

    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

    QPen pen() const;
    void setPen(const QPen& p);
    QBrush brush() const;
    void setBrush(const QBrush& b);
    QFont font() const;
    void setFont(const QFont& f);
};


class GraphRelation: public Graph
{
    QPointF m_p1, m_p2;

    QPointF p1() const;
    QPointF p2() const;

protected:
    // grip相关的操作，子类可以override或实现这些操作。
    virtual void createGrips() override;
    virtual void berthGripsAt() override;

    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

    // 子类要实现
    virtual void resize(const QSizeF &diff) override;

public:
    GraphRelation(const QPointF& p1, const QPoint& p2)
        : Graph(p1), m_p1(p1), m_p2(p2)
    {}

    virtual QRectF boundingRect() const override;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    virtual QPainterPath shape() const override;

};

#endif // GRAPH_H
