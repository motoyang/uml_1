#ifndef GRAPHCLASS_H
#define GRAPHCLASS_H

#include "graph.h"

class GraphClass: public Graph
{
    uml::Class m_c;
    QSizeF m_size, m_minSize;
    bool m_bDroppedVisible;

    QString m_name;
    QList<QString> m_operations;
    QList<QString> m_attributes;

    qreal m_nameXPos;

    QSizeF getSize(const QString &s) const;
    QSizeF getSize(const QList<QString>& l) const;
    void init();
    void placeName();
    void drawDropped(QPainter* painter);

    virtual void resize(const QSizeF &diff) override;


protected:

    // drag and drop event
    virtual void dragEnterEvent(QGraphicsSceneDragDropEvent *event) override;
    virtual void dragLeaveEvent(QGraphicsSceneDragDropEvent *event) override;
    virtual void dragMoveEvent(QGraphicsSceneDragDropEvent *event) override;
    virtual void dropEvent(QGraphicsSceneDragDropEvent *event) override;

    // from Graph
    virtual void createGrips() override;
    virtual void berthGripsAt() override;

public:
    GraphClass(const QPointF& p, const QSizeF s, const QString& name);

    virtual QPainterPath shape() const override;
    virtual QRectF boundingRect() const override;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    virtual int type() const override;
    virtual bool shouldBeenDropped(const QPointF &p) override;
    virtual void setDroppedFlag(bool f) override;

};

#endif // GRAPHCLASS_H
