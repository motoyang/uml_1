#ifndef VISUALENTITY_H
#define VISUALENTITY_H

#include <QGraphicsItem>

struct Grip {
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
        BROKEN_GRIP,
        INTERSECTED_GRIP,
        TARGET_GRIP
    };

    GripType m_type;
    int m_index;
    QRectF m_rect;

    Grip(GripType t);

    Qt::CursorShape cursorShape() const;
    bool shouldChangeYPos() const;
    bool shouldChangeXPos() const;
    void pretreateSize(const QSizeF& diff, QSizeF& sz) const;

};

class VisualEntity: public QGraphicsItem
{
    const int PEN_KEY = 1;
    const int BRUSH_KEY = 2;
    const int FONT_KEY = 3;

    using BaseClass = QGraphicsItem;

    void bringToTop();

protected:
    enum {
        VisualEntityType    = UserType + 1,
        GraphType           = UserType + 2,
        GraphClassType      = UserType + 3,
        GraphRelationType   = UserType + 4,
        LineType            = UserType + 5,
        BrokenLineType      = UserType + 6,
        OrthogonalLineType  = UserType + 7,
        CurvedLineType      = UserType + 8,

        LastOneType         = UserType + 10000
    };

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
    VisualEntity(const QPointF &p);

    QPen pen() const;
    void setPen(const QPen& p);
    QBrush brush() const;
    void setBrush(const QBrush& b);
    QFont font() const;
    void setFont(const QFont& f);

    void updatePosition();

    virtual bool shouldBeenDropped(const QPointF &p) = 0;
    virtual bool isLocked() const = 0;
};


#endif // VISUALENTITY_H
