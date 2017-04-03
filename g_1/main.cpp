#include <QApplication>
#include "graphview.h"
#include "widget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    Widget w;
//    w.show();

//    GraphView *gv = new GraphView;
//    gv->show();

    GraphView g;
    g.show();

    return a.exec();
}
