#include <QtWidgets>
#include <QDebug>

//
// 找出path与contour相交的多个交点，方向是从path的起始点到终点，不包括path的起始点和终点
//
void Intersected(const QPainterPath &contour, const QPainterPath &path, QList<QPointF> &points)
{
    qreal len = path.length();
    QPointF p = path.pointAtPercent(0);
    QPointF last = p;

    bool intersected = false;
    if (contour.contains(p)) {
        // 排除path的起始点就在contour中的情况，不包括这样的点
        intersected = true;
    }

    for(int k = 1; k <= len; ++k)
    {
        if(contour.contains(p) && !intersected)
        {
            // 在非包含状态，发现点被包含，这个点就是交点
            intersected = true;
            points << p;
        }
        if(!contour.contains(p) && intersected)
        {
            // 在包含状态，发现点没有被包含，上个点就是交点
            intersected = false;
            points << last;
        }

        last = p;
        qreal per = path.percentAtLength(k);
        p = path.pointAtPercent(per);
    }
}

//
// 找出path与contour相交的path（实际是多个line相连），方向是从path的起始点到终点，包括path的起始点和终点
//
QPainterPath Intersected(const QPainterPath &contour, const QPainterPath &line)
{
    qreal len = line.length();
    QPointF p = line.pointAtPercent(0);
    QPointF pLast = p;
    bool intersected = false;

    QPainterPath inter_path;
    for(int k = 1; k <= len; ++k)
    {
        if(contour.contains(p) && !intersected)
        {
            intersected = true;
            if (!inter_path.elementCount()) {
                inter_path.moveTo(p);
            } else {
                inter_path.lineTo(p);
            }
        }
        else if(!contour.contains(p) && intersected)
        {
            intersected = false;
            inter_path.lineTo(pLast);
        }

        pLast = p;
        qreal per = line.percentAtLength(k);
        p = line.pointAtPercent(per);
    }

    if (contour.contains(p)) {
        inter_path.lineTo(p);
    }

    return inter_path;
}

/*!
    Returns the closest element (position) in \a sourcePath to \a target,
    using \l{QPoint::manhattanLength()} to determine the distances.
*/
QPointF closestPoint(const QPointF &target, const QPainterPath &sourcePath)
{
    Q_ASSERT(!sourcePath.isEmpty());

    QPointF shortestDistance;
    qreal shortestLength = std::numeric_limits<int>::max();

    for (int i = 0; i < sourcePath.elementCount(); ++i) {
        const QPointF distance(sourcePath.elementAt(i) - target);
        const qreal length = distance.manhattanLength();
        if (length < shortestLength) {
            shortestDistance = sourcePath.elementAt(i);
            shortestLength = length;
        }
    }

    return shortestDistance;
}

QPointF farthestPoint(const QPointF &target, const QPainterPath &sourcePath)
{
    Q_ASSERT(!sourcePath.isEmpty());

    QPointF farthestDistance;
    qreal farthestLength = 0;

    for (int i = 0; i < sourcePath.elementCount(); ++i) {
        const QPointF distance(sourcePath.elementAt(i) - target);
        const qreal length = distance.manhattanLength();
        if (length > farthestLength) {
            farthestDistance = sourcePath.elementAt(i);
            farthestLength = length;
        }
    }

    return farthestDistance;
}

//
// 找出line与contour相交的第一个交点，方向是从line.p1()到line.p2()
//
bool IntersectedPoint(const QLineF& line, const QPainterPath& contour, QPointF& p)
{
    // QLineF has normalVector(), which is useful for extending our path to a rectangle.
    // The path needs to be a rectangle, as QPainterPath::intersected() only accounts
    // for intersections between fill areas, which projectilePath doesn't have.
    QLineF startEdge = line.normalVector();
    startEdge.setLength(3);
    // The end point is currently pointing the wrong way; move it to face the same
    // direction as startEdge.
    QLineF endEdge = QLineF(line.p2(), line.p1()).normalVector();
    endEdge.setLength(-3);

    // Now we can create a rectangle from our edges.
    QPainterPath rectPath(startEdge.p1());
    rectPath.lineTo(startEdge.p2());
    rectPath.lineTo(endEdge.p2());
    rectPath.lineTo(endEdge.p1());
    rectPath.lineTo(startEdge.p1());

    // 计算相交区域
    QPainterPath ppIntersected = contour.intersected(rectPath);
    if (ppIntersected.isEmpty()) {
        // 没有相交的区域，找不到交点
        return false;
    }

    if (contour.contains(line.p1())) {
        // 如果line.p1()在contour内，就找最远的点
        p = farthestPoint(line.p1(), contour.intersected(rectPath));
    } else {
        // The hit position will be the element (point) of the rectangle that is the
        // closest to where the line was fired from.
        p = closestPoint(line.p1(), contour.intersected(rectPath));
    }

    return true;
}

void  test2(QGraphicsScene* scene)
{
    QPainterPath ppTarget;
    ppTarget.addEllipse(0, 0, 25, 25);
    ppTarget.addRect(-10, -10, 30, 30);
    QGraphicsItem* target = scene->addPath(ppTarget);
    target->setTransformOriginPoint(QPointF(12.5, 12.5));
    target->setRotation(15);
    target->setPos(100, 100);

    QPainterPath projectilePath;
    projectilePath.moveTo(105, 115);
    projectilePath.lineTo(83, 95);
    scene->addPath(projectilePath, QPen(Qt::DashLine));
    scene->addText("start")->setPos(180, 150);
    scene->addText("end")->setPos(20, 0);

    QList<QPointF> points;
    int i = 1;
    QPainterPath p2 = target->mapToScene(target->shape());
    Intersected(p2, projectilePath, points);
    foreach (const QPointF &p, points) {
        Q_ASSERT(p2.contains(p));
        scene->addEllipse(p.x() - i, p.y() - i, 2*i, 2*i, QPen(Qt::red));
        ++i;
    }
}

void test3(QGraphicsScene* scene)
{
    QPainterPath ppTarget;
    ppTarget.addEllipse(0, 0, 25, 25);
    ppTarget.addRect(-10, -10, 30, 30);
    QGraphicsItem* target = scene->addPath(ppTarget);
    target->setTransformOriginPoint(QPointF(12.5, 12.5));
    target->setRotation(15);
    target->setPos(100, 100);

    QLineF line(128, 110, 83, 95);
    scene->addLine(line, QPen(Qt::DashLine));
    scene->addText("start")->setPos(180, 150);
    scene->addText("end")->setPos(20, 0);

    QPointF hitPos;
    if (IntersectedPoint(line, target->mapToScene(target->shape()), hitPos)) {
        scene->addEllipse(hitPos.x() - 2, hitPos.y() - 2, 4, 4, QPen(Qt::red));
    }
}

int main2(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QGraphicsView view;
    QGraphicsScene *scene = new QGraphicsScene;
    view.setScene(scene);
    view.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view.scale(qreal(15.0), qreal(15.0));

//    test2(scene);
    test3(scene);

    view.show();
    return app.exec();
}
