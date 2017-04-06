#include <QPainterPath>
#include <QtWidgets>


QPainterPath Intersect(const QPainterPath &contour, const QPainterPath &line)
{
    qreal len = line.length();
    QPointF p = line.pointAtPercent(0);
    bool intersected = false;

    QPainterPath inter_path;

    for(int k = 1; k <= len; ++k)
    {
        if(contour.contains(p) && !intersected)
        {
            intersected = true;
            inter_path.moveTo(p);
        }
        else if(!contour.contains(p) && intersected)
        {
            inter_path.lineTo(p);
            return inter_path;
        }

        qreal per = line.percentAtLength(k);
        p = line.pointAtPercent(per);
    }
}


/*!
    Returns the closest element (position) in \a sourcePath to \a target,
    using \l{QPoint::manhattanLength()} to determine the distances.
*/
QPointF closestPointTo(const QPointF &target, const QPainterPath &sourcePath)
{
    Q_ASSERT(!sourcePath.isEmpty());
    QPointF shortestDistance = sourcePath.elementAt(0) - target;
    qreal shortestLength = shortestDistance.manhattanLength();
    for (int i = 1; i < sourcePath.elementCount(); ++i) {
        const QPointF distance(sourcePath.elementAt(i) - target);
        const qreal length = distance.manhattanLength();
        if (length < shortestLength) {
            shortestDistance = sourcePath.elementAt(i);
            shortestLength = length;
        }
    }
    return shortestDistance;
}

QPointF closestPointTo2(const QPointF &target, const QPainterPath &sourcePath)
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

/*!
    Returns \c true if \a projectilePath intersects with any items in \a scene,
    setting \a hitPos to the position of the intersection.
*/
bool hit(const QPainterPath &projectilePath, QGraphicsScene *scene, QPointF &hitPos)
{
    const QList<QGraphicsItem *> itemsInPath = scene->items(projectilePath, Qt::IntersectsItemBoundingRect);
    if (!itemsInPath.isEmpty()) {
        const QPointF projectileStartPos = projectilePath.elementAt(0);
        float shortestDistance = std::numeric_limits<float>::max();
        QGraphicsItem *closest = 0;
        foreach (QGraphicsItem *item, itemsInPath) {
            QPointF distanceAsPoint = item->pos() - projectileStartPos;
            float distance = abs(distanceAsPoint.x() + distanceAsPoint.y());
            if (distance < shortestDistance) {
                shortestDistance = distance;
                closest = item;
            }
        }

        QPainterPath targetShape = closest->mapToScene(closest->shape());
        // QLineF has normalVector(), which is useful for extending our path to a rectangle.
        // The path needs to be a rectangle, as QPainterPath::intersected() only accounts
        // for intersections between fill areas, which projectilePath doesn't have.
        QLineF pathAsLine(projectileStartPos, projectilePath.elementAt(1));
        // Extend the first point in the path out by 1 pixel.
        QLineF startEdge = pathAsLine.normalVector();
        startEdge.setLength(1);
        // Swap the points in the line so the normal vector is at the other end of the line.
        pathAsLine.setPoints(pathAsLine.p2(), pathAsLine.p1());
        QLineF endEdge = pathAsLine.normalVector();
        // The end point is currently pointing the wrong way; move it to face the same
        // direction as startEdge.
        endEdge.setLength(-1);
        // Now we can create a rectangle from our edges.
        QPainterPath rectPath(startEdge.p1());
        rectPath.lineTo(startEdge.p2());
        rectPath.lineTo(endEdge.p2());
        rectPath.lineTo(endEdge.p1());
        rectPath.lineTo(startEdge.p1());
        // Visualize the rectangle that we created.
        scene->addPath(rectPath, QPen(QBrush(Qt::blue), 2));
        // Visualize the intersection of the rectangle with the item.
        scene->addPath(targetShape.intersected(rectPath), QPen(QBrush(Qt::cyan), 2));
        // The hit position will be the element (point) of the rectangle that is the
        // closest to where the projectile was fired from.
        hitPos = closestPointTo2(projectileStartPos, targetShape.intersected(rectPath));

        return true;
    }
    return false;
}
/*
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QGraphicsView view;
    QGraphicsScene *scene = new QGraphicsScene;
    view.setScene(scene);
    view.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QGraphicsItem *target = scene->addEllipse(0, 0, 25, 25);
    target->setTransformOriginPoint(QPointF(12.5, 12.5));
    target->setRotation(35);
    target->setPos(100, 100);

    QPainterPath projectilePath;
    projectilePath.moveTo(200, 200);
    projectilePath.lineTo(0, 0);
    projectilePath.lineTo(200, 200);

    QPointF hitPos;
    if (hit(projectilePath, scene, hitPos)) {
        scene->addEllipse(hitPos.x() - 2, hitPos.y() - 2, 4, 4, QPen(Qt::red));
    }

    scene->addPath(projectilePath, QPen(Qt::DashLine));
    scene->addText("start")->setPos(180, 150);
    scene->addText("end")->setPos(20, 0);

    view.show();

    return app.exec();
}
*/


/*
// 存放一些常用的函数或者一些常用功能的实现算法;


//////////////////////////////////////////////////////////////////////////
// 圆周率;
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


//////////////////////////////////////////////////////////////////////////
// 该函数返回的角度为从第一条线到第二条线逆时针方向选择所经过的角度，范围为0 - 360，不包括360度。
// QLineF::angleTo(const QLineF&)

// 该函数是从x轴0点从发，沿正方向的水平线到直线的角度，范围0 - 360，不包括360。
// QLineF::angle()
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// 已知屏幕坐标系上两点，且这两点不在的x坐标不相等，则他们所形成的 y = ax + b 形式的直线方程中：
// a = (y2 - y1) / (x2 - x1)， b = (x2 * y1 - x1 * y2) / ( x2 - x1)
// 注意：y1、y2的值为两点的y值的相反数，如果要计算第三点的坐标，所得到的第三点y值同样需要再取其相反数作为屏幕坐标的真正的值
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// 计算平方的函数;
double square(const double num){return num * num;}


//////////////////////////////////////////////////////////////////////////
// 计算屏幕上面两个点之间的直线距离的函数，需要与计算平方函数同时使用;
double TwoPtDistance(const QPointF& pt1, const QPointF& pt2)
{return sqrt(double(square(pt2.x() - pt1.x()) + square(pt2.y() - pt1.y())));}


//////////////////////////////////////////////////////////////////////////
//
// Method:    caculateThirdPtFromTwoPoint
// Returns:   返回计算出的第三个点坐标;
// Qualifier: 根据两个点的位置和第三个点与第一个点连线的长度与前两个点连线逆时针旋转的角度，计算出第三个点的位置;
// Parameter: QPointF pt1  第一个点坐标，该点也是关键点;
// Parameter: QPointF pt2  第二个点坐标
// Parameter: double nLength 第一个点与第三个点之间的直线距离;
// Parameter: double nAngle 第一个点和第二个点的连线 与 第一个点和第三个点的连线之间的夹角，逆时针旋转，使用的是弧度。
//
QPointF caculateThirdPtFromTwoPoint(const QPointF& pt1, const QPointF& pt2, const double nLength, const double nAngle)
{
double x1 = pt1.x();
double y1 = pt1.y();
double x2 = pt2.x();
double y2 = pt2.y();
double x3 = 0.0, y3 = 0.0;


x3 = x1 + nLength * cos(double(atan2(y2 - y1, x2 - x1)) - nAngle);
y3 = y1 + nLength * sin(double(atan2(y2 - y1, x2 - x1)) - nAngle);


return QPointF(x3, y3);
}


//////////////////////////////////////////////////////////////////////////
// Qt中获取文字的宽度和高度;
QFontMetrics fm;
QPainter p(this);
fm = p.fontMetrics();
fm.width("12345"); //获取字符串的宽度;
fm.ascent() + fm.descent(); //获取字符串的高度;
fm.ascent(); //一个字的基线到最顶部的距离;
fm.descent(); //一个字的基线到最底部的距离;




//////////////////////////////////////////////////////////////////////////
// 获取两个点的中心点坐标;
QPointF GetMiddlePoint(const QPointF& pt1, const QPointF& pt2)
{
return QPointF((pt2.x() + pt1.x()) / 2.0, (pt1.y() + pt2.y()) / 2.0);
}




//////////////////////////////////////////////////////////////////////////
//
// Method:    GetDoubleArrowLine
// Qualifier: 根据一条直线的两个端点计算出双箭头直线的箭头坐标;
// Parameter: const QPointF& pt1 直线的第一个端点;
// Parameter: const QPointF& pt2 直线的第二个端点;
// Parameter: QVector<QPointF> & arrow1 返回的直线的第一个点处的箭头坐标;
// Parameter: QVector<QPointF> & arrow2 返回的直线的第二个点处的箭头坐标;
//
void GetDoubleArrowLine(const QPointF& pt1, const QPointF& pt2, QVector<QPointF>& arrow1, QVector<QPointF>& arrow2)
{
float x1 = pt1.x();         //lastPoint 起点
float y1 = pt1.y();
float x2 = pt2.x();          //endPoint 终点
float y2 = pt2.y();


float l = 10.0;                   //箭头的那长度
float a = 30.0 / 180.0 * M_PI;    //箭头与线段角度
int nAbs = 1;                     //用于控制箭头的正反向;


if(3 * l > TwoPtDistance(pt1, pt2))
nAbs = -1;


float x3 = x2 - nAbs * l * cos(atan2((y2 - y1), (x2 - x1)) - a);
float y3 = y2 - nAbs * l * sin(atan2((y2 - y1), (x2 - x1)) - a);
float x4 = x2 - nAbs * l * sin(atan2((x2 - x1), (y2 - y1)) - a);
float y4 = y2 - nAbs * l * cos(atan2((x2 - x1), (y2 - y1)) - a);
float x5 = x1 - nAbs * l * cos(atan2((y1 - y2), (x1 - x2)) - a);
float y5 = y1 - nAbs * l * sin(atan2((y1 - y2), (x1 - x2)) - a);
float x6 = x1 - nAbs * l * sin(atan2((x1 - x2), (y1 - y2)) - a);
float y6 = y1 - nAbs * l * cos(atan2((x1 - x2), (y1 - y2)) - a);


arrow2.append(QPointF(x3, y3));
arrow2.append(QPointF(x2, y2 - 0.5));
arrow2.append(QPointF(x4, y4));


arrow1.append(QPointF(x5, y5));
arrow1.append(QPointF(x1, y1 - 0.5));
arrow1.append(QPointF(x6, y6));
}


//////////////////////////////////////////////////////////////////////////
//
// Method:    计算一个点到一条直线的垂直点坐标;
// Returns:   QPointF 返回垂直点的坐标;
// Parameter: const QPointF & pt1 直线的第一个点;
// Parameter: const QPointF & pt2 直线的第二个点;
// Parameter: const QPointF pt3 目标点;
//
QPointF caculateCrossPosPtToLine(const QPointF& pt1, const QPointF& pt2, const QPointF pt3)
{
QPointF ptCross;


// 垂直的情况;
if (pt1.x() == pt2.x())
{
ptCross.setX(pt1.x());
ptCross.setY(pt3.y());
}
else
{
qreal x1 = pt1.x(), y1 = pt1.y();
qreal x2 = pt2.x(), y2 = pt2.y();
qreal x3 = pt3.x(), y3 = pt3.y();


qreal ss1 = square(x1 - x3) + square(y1 - y3);
qreal ss2 = square(x2 - x3) + square(y2 - y3);
qreal ll = square(x1 - x2) + square(y1 - y2);
qreal a = ss1 - ss2;


qreal t = a / (2.0 * ll) + 0.5;


ptCross = QLineF(pt1, pt2).pointAt(t);
}


return ptCross;
}


//////////////////////////////////////////////////////////////////////////
//
// Method:    计算一个点一条直线的垂直距离;
// Returns:   qreal 返回垂直距离;
// Parameter: const QPointF & pt1  直线的第一个点;
// Parameter: const QPointF & pt2  直线的第二个点;
// Parameter: const QPointF pt3  目标点;
//
qreal caculateCrossLengthPtToLine(const QPointF& pt1, const QPointF& pt2, const QPointF pt3)
{
// 直线垂直的情况;
if (pt1.x() == pt2.x())
{
return pt3.x() - pt1.x();
}
else
{
qreal angle = QLineF(pt1, pt2).angleTo(QLineF(pt1, pt3));
qreal leng = sin(angle / 180 * M_PI) * (QLineF(pt1, pt3).length());


return leng;
}
}


//
// Method:    计算两条直线的相交点坐标，平行则返回0,0点坐标;
// Returns:   返回交点坐标;
// Parameter: QLineF line1  第一条直线;
// Parameter: QLineF line2 第二条直线;
//
QPointF CaculateTwoLineCrossPt(QLineF line1, QLineF line2)
{
// 斜率相等为平行线，没有交点;
if (line1.angle() == line2.angle())
return QPointF();


qreal x1 = line1.p1().x();
qreal y1 = 0 - line1.p1().y();
qreal x2 = line1.p2().x();
qreal y2 = 0 - line1.p2().y();
qreal x3 = line2.p1().x();
qreal y3 = 0 - line2.p1().y();
qreal x4 = line2.p2().x();
qreal y4 = 0 - line2.p2().y();


if (x2 == x1 && x3 != x4)
{
qreal x = x1;
qreal y = 0 - (x3 * y4 - x4 * y3 + x1 * (y3 - y4)) / (x3 - x4);
return QPointF(x, y);
}
else if (x3 == x4 && x1 != x2)
{
qreal x = x3;
qreal y = 0 - (x1 * y2 - x2 * y1 + x3 * (y1 - y2)) / (x1 - x2);
return QPointF(x, y);
}
else
{
qreal a1 = (y2 - y1) / (x2 - x1);
qreal b1 = (x2 * y1 - x1 * y2) / (x2 - x1);
qreal a2 = (y4 - y3) / (x4 - x3);
qreal b2 = (x4 * y3 - x3 * y4) / (x4 - x3);


qreal x = (b2 - b1) / (a1 - a2);
qreal y = 0 - (a1 * b2 - a2 * b1) / (a1 - a2);


return QPointF(x, y);
}
}


//
// Method:    求一条直线的哪个端点到目标点的距离最近;
// Returns:   bool 目标点在直线之内，返回true，否则返回false;
// Parameter: const QLineF & line 直线;
// Parameter: const QPointF & pt 目标点;
// Parameter: QPointF & nearPt 返回最近的端点;
//
bool GetLineNearPt(const QLineF& line, const QPointF & pt, QPointF& nearPt)
{
QLineF line1(line.p1(), pt);
QLineF line2(line.p2(), pt);


nearPt = (line1.length() <= line2.length() ? line.p1() : line.p2());


if (line1.length() + line2.length() <= line.length())
return true;
else
return false;
}





//////////////////////////////////////
// Method:    计算两条直线是否在平行误差范围内平行;
// Returns:   bool  平行true，不平行false;
// Parameter: const QLineF & line1  第一条直线;
// Parameter: const QLineF & line2 第二条直线;
//////////////////////////////////////
#define ANGLE_ERROR_COEFFICIENT 3 //角度误差系数，度;
bool GetTwoLineIsCloseParallel(const QLineF& line1, const QLineF& line2)
{
qreal a1 = line1.angle();
qreal a2 = line2.angle();
qreal nT = qAbs(a1 - a2);


while (nT > 180)
{
nT -= 180;
}


if (nT <= ANGLE_ERROR_COEFFICIENT || nT >= 180 - ANGLE_ERROR_COEFFICIENT)
return true;
else
return false;
}


//////////////////////////////////////
// Method:    计算两条直线所形成夹角的最小角度，范围从 -180 到 180;
// Returns:   double 返回形成的夹角;
// Parameter: const QLineF & line1 直线1;
// Parameter: const QLineF & line2 直线2;
// Parameter: const QPointF & circleCenter 圆弧的中心点;
// Parameter: const double & angle 角度;
// Parameter: const QPointF & startPt 起始点坐标;
//////////////////////////////////////
double caculateCrossAngle(const QLineF &line1, const QLineF &line2, const QPointF &circleCenter, const double &angle, const QPointF& startPt)
{
// 计算两条直线之间的夹角，而不是与某条直线的延长线的夹角;
double crossAngle = angle;
// 圆弧的半径;
double nRadius = QLineF(circleCenter, startPt).length();
// 计算第一条线与经过圆心的平行线的角度;
double nAngle = QLineF(circleCenter.x(), circleCenter.y(), circleCenter.x() + nRadius, circleCenter.y()).angleTo(QLineF(circleCenter, startPt));
//  计算终点坐标圆心形成直线的正负角度;
double angle1 = nAngle + crossAngle;
double angle2 = nAngle - (180 - crossAngle);
// 计算终点坐标;
double ptX1 = circleCenter.x() + nRadius * cos(angle1 / 180.0 * M_PI);
double ptY1 = circleCenter.y() - nRadius * sin(angle1 / 180.0 * M_PI);
double ptX2 = circleCenter.x.D() + nRadius * cos(angle2 / 180.0 * M_PI);
double ptY2 = circleCenter.y.D() - nRadius * sin(angle2 / 180.0 * M_PI);
// 计算两个终点坐标与第二条线两个端点的距离之和;
double length1 = QLineF(line2.p1(), QPointF(ptX1, ptY1)).length() + QLineF(line2.p2(), QPointF(ptX1, ptY1)).length();
double length2 = QLineF(line2.p1(), QPointF(ptX2, ptY2)).length() + QLineF(line2.p2(), QPointF(ptX2, ptY2)).length();
// 取距离较短的对应的角度，作为圆弧的弧度;
if (length1 > length2)
crossAngle = crossAngle - 180;


if (crossAngle < -180)
crossAngle += 360;


return crossAngle;
}


//  用于计算第二条直线的哪个点与第一条直线的第一个点处于同一侧;
QPointF CaculateOneSidePoint(const QLineF& l1, const QLineF& l2)
{
QLineF tL(l2.p2(), l2.p1());


double a1 = l1.angleTo(l2);
double a2 = l1.angleTo(tL);


if (a1 > 180)
a1 = 360 - a1;
if (a2 > 180)
a2 = 360 - a2;


if (a1 < a2)
return l2.p1();
else
return l2.p2();
}


//////////////////////////////////////////////////////////////////////////
//  计算两条直线的中线，计算规则两条直线的角平分线，如果两条直线平行，则为等距离的线;
//  参数isAcuteAngle为是取两条直线连线的锐角的角平分线还是钝角的角平分线;
QLineF CaculateMiddleLine(const QLineF& l1, const QLineF& l2, bool isAcuteAngle = true)
{
QPointF p1, p2, p3, p4;
p1 = l1.p1();
p2 = l1.p2();
p3 = CaculateOneSidePoint(l1, l2);
p4 = (p3 == l2.p1() ? l2.p2() : l2.p1());


if (!isAcuteAngle)
qSwap(p3, p4);


double a1 = QLineF(p1, p2).angleTo(QLineF(p3, p4));


//  两条直线平行, 返回两条直线的两个端点连线的中点所形成的线;
if (a1 == 0 || a1 == 180)
{
return QLineF(GetMiddlePoint(p1, p3), GetMiddlePoint(p2, p4));
}


QLineF crossLine;
crossLine.setP1(CaculateTwoLineCrossPt(l1, l2));
crossLine.setAngle(a1 / 2 + l1.angle());


QPointF m1, m2;
m1 = CaculateTwoLineCrossPt(QLineF(p1, p3), crossLine);
m2 = CaculateTwoLineCrossPt(QLineF(p2, p4), crossLine);


return QLineF(m1, m2);
}






// BMP图像文件结构体;
typedef struct tagBITMAPFILEHEADER { // bmfh
tagBITMAPFILEHEADER(){
bfType = 0x4D42;
bfSize = 0;
bfReserved1 = bfReserved2 = 0;
bfOffBits = 0;
}
unsigned short    bfType;//
文件类型，必须为 0x4D42，即为"BM"，否则表示根本不是BMP
unsigned long    bfSize; //   该位图文件的大小，用字节为单位;
unsigned short    bfReserved1;//
保留，为0;
unsigned short    bfReserved2;//
保留，为0;
unsigned long     bfOffBits;//
从文件头开始到实际的图像数据之间的字节的偏移量;
} BITMAPFILEHEADER;


// 位图信息段;
typedef struct tagBITMAPINFOHEADER{
tagBITMAPINFOHEADER(){
biSize = 0;
biWidth = biHeight = 0;
biPlanes = 1;
biBitCount = 1;
biCompression = 0;
biSizeImage = 0;
biXPelsPerMeter = biYPelsPerMeter = 0;
biClrUsed = 0;
biClrImportant = 0;
}
unsigned long      biSize;//
BITMAPINFOHEADER 结构体的大小;
long       biWidth;//
图像宽度;
long       biHeight;//
图像高度;
unsigned short       biPlanes;//
bmp图片的平面属，为1;
unsigned short       biBitCount;//
比特数/像素, 1/4/8/16/24/32;
unsigned long      biCompression;//
图像数据压缩类型: BI_RGB 0L没有压缩; BI_RLE8 1L; BI_RLE4 2L; BI_BITFIELDS 3L; BI_JPEG 4L;
unsigned long      biSizeImage;//
图像的大小，以字节为单位，当使用BI_RGB 格式时，可设置为0;
long       biXPelsPerMeter;//
水平分辨率，用像素/米表示;
long       biYPelsPerMeter;//
垂直分表率;
unsigned long      biClrUsed;//
位图实际使用的颜色表中颜色索引数，设置为0的话，表示使用所有的调色板项;
unsigned long      biClrImportant;//
对图像显示有重要影响的颜色索引的数目，如果是0，表示都重要;
} BITMAPINFOHEADER, FAR *LPBITMAPINFOHEADER, *PBITMAPINFOHEADER;


// 调色板;
typedef struct tagRGBQUAD {
unsigned char    rgbBlue;
unsigned char    rgbGreen;
unsigned char    rgbRed;
unsigned char    rgbReserved;
} RGBQUAD;




typedef struct tagBITMAPINFO {
BITMAPINFOHEADER    bmiHeader;
RGBQUAD             bmiColors[1];
} BITMAPINFO, FAR *LPBITMAPINFO, *PBITMAPINFO;






//////////////////////////////////////
// Method:    BMP文件内容填充;
// Returns:   void
// Parameter: const uchar * pR red 通道;
// Parameter: const uchar * pG green 通道;
// Parameter: const uchar * pB blue 通道;
// Parameter: int w 图片宽度;
// Parameter: int h 图片高度;
// Parameter: QImage & image 输出的图片对象;
//////////////////////////////////////
void writeBMPImage(const uchar* pR, const uchar* pG, const uchar* pB, int w, int h, QImage & image)
{
int rW = w;
// 宽度对4取整;
if (w % 4)
rW = (rW / 4 + 1) * 4;


// 填写文件头;
BITMAPFILEHEADER bmpFileHead;
bmpFileHead.bfSize = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+rW * h * 32 / 8;
bmpFileHead.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);


// 填写图片信息头;
BITMAPINFOHEADER bmpInfoHead;
bmpInfoHead.biSize = sizeof(BITMAPINFOHEADER);
bmpInfoHead.biWidth = w;
bmpInfoHead.biHeight = h;
bmpInfoHead.biBitCount = 24;
bmpInfoHead.biCompression = BI_RGB;
bmpInfoHead.biSizeImage = rW * h * 24 / 8;


// 分配整个文件的大小的内存;
char * pBuf = new char[bmpFileHead.bfSize];
memset(pBuf, 0, bmpFileHead.bfSize);


// 分配图像数据大小的内存;
char *pImage = new char[rW * h * 3];
memset(pImage, 0, rW * h * 3);


// 将图片颜色合成到图像数据中;
for (int i = 0; i < h; i++)
{
for (int j = 0; j < w; j++)
{
pImage[i * rW * 3 + j * 3] = pR[i * w + j];
pImage[i * rW * 3 + j * 3 + 1] = pG[i * w + j];
pImage[i * rW * 3 + j * 3 + 2] = pB[i * w + j];
}
}


// 将所有数据放入buf中;
int nOffset = 0;
memcpy(pBuf + nOffset, bmpFileHead, sizeof(bmpFileHead));
nOffset += sizeof(bmpFileHead);


int nOffset = 0;
memcpy(pBuf + nOffset, bmpInfoHead, sizeof(bmpInfoHead));
nOffset += sizeof(bmpInfoHead);


int nOffset = 0;
memcpy(pBuf + nOffset, pImage, sizeof(rW * h * 3));
nOffset += sizeof(rW * h * 3);


// image对象加载图片;
image.loadFromData(pBuf, nOffset);
}
*/
