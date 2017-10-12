#ifndef GRAPHCLASS_H
#define GRAPHCLASS_H

#include "entity.h"
#include "graph.h"

class GraphClass: public Graph
{
    using BaseClass = Graph;

    uml::Class m_c;
    QSizeF m_size, m_minSize;
    bool m_bDroppedVisible;
    qreal m_nameXPos;
    QMap<const Link*, QPointF> m_sources, m_targets;
    QPointF m_droppedPoint;

    QString m_name;
    QList<QString> m_operations;
    QList<QString> m_attributes;

    QSizeF getSize(const QString &s) const;
    QSizeF getSize(const QList<QString>& l) const;
    void init();
    void placeName();
    void drawDropped(QPainter* painter);
    void relationsUpdate() const;

protected:

    // drag and drop event
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

    // from Graph
    virtual void resizeByGrip(const QSizeF &diff) override;
    virtual void createGrips() override;
    virtual void berthGripsAt() override;

public:
    GraphClass(const QPointF& p, const QSizeF s, const QString& name);

    virtual QPainterPath shape() const override;
    virtual QRectF boundingRect() const override;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    virtual int type() const override;
    virtual void setDroppedFlag(bool f) override;

    virtual void connectTarget(const Link *r) override;
    virtual void disconnectTarget(const Link* const g) override;
    virtual void connectSource(const Link *r) override;
    virtual void disconnectSource(const Link* const g) override;

    virtual void showDroppedPoint(const QPointF &p) override;
    virtual bool sourceDroppedPoint(const Link* l, QPointF &p) const override;
    virtual bool targetDroppedPoint(const Link* l, QPointF &p) const override;

    virtual QPainterPath bodyPath() const override;

};

#endif // GRAPHCLASS_H
