#ifndef BROKENLINE_H
#define BROKENLINE_H

#include "link.h"

class BrokenLine: public Link
{
    using BaseClass = Link;

    QList<QSharedPointer<QPointF>> m_points;

    QPainterPath path() const;
    bool simplify();
    void brokenAt(const QPointF &pos);

protected:
    // grip相关的操作，子类可以override或实现这些操作。
    virtual void createGrips() override;
    virtual void berthGripsAt() override;
    virtual void resizeByGrip(const QSizeF &diff) override;

    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
//    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    // 子类要实现
    virtual QPointF startPoint() const override;
    virtual QPointF endPoint() const override;
    virtual void setStartPoint(const QPointF& p) override;
    virtual void setEndPoint(const QPointF& p) override;
    virtual QPointF closestPointToTarget() const override;
    virtual QPointF closestPointToSource() const override;

public:
    BrokenLine(const QPointF &p);

    virtual int type() const override;
    virtual QRectF boundingRect() const override;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    virtual QPainterPath shape() const override;

};

#endif // BROKENLINE_H
