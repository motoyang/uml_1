#ifndef GRAPH_H
#define GRAPH_H

#include <QGraphicsPathItem>

struct TextMargin {
    int top;
    int left;
    int right;
    int bottom;
};

class Graph : public QGraphicsPathItem
{
    QPoint m_pos;
    QSize m_size;
//    QFont m_font;

    QString m_name;
    QList<QString> m_operations;
    QList<QString> m_attributes;


    QFont font() const;
    QSize minMargin() const;

public:
    Graph(const QPoint& p, const QSize s);


    void init2();
    QSize getSize(const QList<QString> l);
};

#endif // GRAPH_H
