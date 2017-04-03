#include "graph.h"
#include "graphview.h"

GraphView::GraphView()
{

    QGraphicsScene *s0 = new QGraphicsScene(this);
    s0->setItemIndexMethod(QGraphicsScene::NoIndex);
    s0->setSceneRect(-200, -100, 400, 200);
    setScene(s0);

    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    scale(qreal(2.0), qreal(2.0));
    setMinimumSize(500, 500);
    setWindowTitle(tr("Graphics test"));
    setBackgroundBrush(QBrush(Qt::lightGray));

    s0->addItem(new Graph(QPoint(0, 0), QSize(30, 30)));
    s0->addItem(new Graph(QPoint(0, 50), QSize(230, 300)));
    s0->addItem(new Graph(QPoint(50, 0), QSize(350, 50)));
}
