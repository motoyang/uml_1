#ifndef GRAPH_H
#define GRAPH_H

#include <QFont>
#include <QGraphicsPathItem>

struct TextPadding {
    int top;
    int left;
    int right;
    int bottom;
};

struct TextPadding2 {
    qreal top;
    qreal left;
    qreal right;
    qreal bottom;
};

class Graph : public QGraphicsPathItem
{
    QPoint m_pos;
    QSize m_size;
//    QFont m_font;

    TextPadding padding() const;
    QString m_name;
    QList<QString> m_operations;
    QList<QString> m_attributes;


    QFont font() const;
    QSize minMargin() const;

    int m_nameHeight, m_nameXPos;
    int m_operationsHeight;
    int m_attributesHeight;
//    QPoint m_ptName;
//    QPoint m_ptAttributes;
//    QPoint m_ptOperations;

public:
    Graph(const QPoint& p, const QSize s);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    QRectF boundingRect() const override    ;

    void init3();
    void init2();
    QSize getSize(const QList<QString> l);
    QSize getSize2(const QList<QString> l);
};

class Graph2: public QAbstractGraphicsShapeItem
{
    static TextPadding2 s_padding;

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
    Graph2(const QPointF& p, const QSizeF s);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    QRectF boundingRect() const override;

};

class GraphClass: public Graph
{

};

#endif // GRAPH_H
