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

    s0->addRect(s0->sceneRect());
//    s0->addRect(-100, -100, 200, 200);
    s0->addEllipse(0, 0, 20, 50);
    s0->addText(tr("this is text in the scene."));

    QPainterPath pp;
//    pp.addRect(0, 0, 100, 20);
    QString s = tr("item test这是一个政治的人");
    QFont f;
    QFontMetrics fm(f);
    QRect r = fm.boundingRect(s);
    pp.addRect(r);
    pp.addText(0, 0, f, s);
//    s0->addPath(pp);

    Graph *u3i = new Graph;
    u3i->setPos(30, -50);
    u3i->setPath(pp);
    s0->addItem(u3i);
}
