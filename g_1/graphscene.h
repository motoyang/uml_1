#ifndef GRAPHSCENE_H
#define GRAPHSCENE_H

#include <QGraphicsScene>

class GraphScene : public QGraphicsScene
{
public:
    GraphScene();

    // Returns true if projectilePath intersects with targetPath,
    // setting hitPos to the position of the intersection.
    bool hit(const QPainterPath &projectilePath, const QPainterPath& targetPath, QPointF &hitPos) const;
};

#endif // GRAPHSCENE_H
