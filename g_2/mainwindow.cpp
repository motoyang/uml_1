#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    createScene();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createScene()
{
    QScreen *q = qApp->primaryScreen();
    qreal dpi = q->logicalDotsPerInch() / 72;
    qreal ppi = q->physicalDotsPerInch() / 72;
    qDebug() << dpi;
    qDebug() << ppi;

    QPixmap pixmap(5, 5);
    pixmap.fill();
    QPainter painter(&pixmap);
    QPen pen(Qt::lightGray, 0.1);
    painter.setPen(pen);
    painter.drawLine(0, 0, 0, 4);
    painter.drawLine(1, 0, 4, 0);
    painter.end();

    auto scene = new QGraphicsScene(0.5, 0.5, 600, 6000);
    scene->setBackgroundBrush(QBrush(pixmap));
    //    scene->setForegroundBrush(QColor(0, 160, 0, 20));
    scene->addRect(20, 10, dpi * 300, dpi * 1 * 72, QPen(Qt::green), QBrush(Qt::black));
    //      QRelationLine* rl = new QRelationLine();

    //       test1(scene);
    auto view = new QGraphicsView();
    view->setScene(scene);
    view->scale(ppi/dpi, ppi/dpi);
    setCentralWidget(view);
}
