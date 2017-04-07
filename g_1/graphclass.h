#ifndef GRAPHCLASS_H
#define GRAPHCLASS_H

#include "graph.h"

class GraphClass: public Graph
{
    uml::Class m_c;

    QString m_name;
    QList<QString> m_operations;
    QList<QString> m_attributes;

    qreal m_nameXPos;

    QSizeF getSize(const QString &s) const;
    QSizeF getSize(const QList<QString>& l) const;
    void init();
    void placeName();

protected:

    // from Graph
    virtual void createGrips() override;
    virtual void berthGripsAt() override;

public:
    GraphClass(const QPointF& p, const QSizeF s, const QString& name);

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};

#endif // GRAPHCLASS_H
