#include <iostream>
#include <list>
#include "things.h"

#ifndef   UNUSED_PARAM
#define   UNUSED_PARAM(v)   (void)(v)
#endif

using namespace std;
using namespace uml;
/*
int main(int argc, char *argv[])
{
    UNUSED_PARAM(argc);
    UNUSED_PARAM(argv);

    uml::Class c;
    uml::Parameter p("inout", "p1", "t1", "d1");
    uml::Parameter p2("p2", "t2", "d2");
    uml::Parameter p3("p3", "t3");
    uml::Parameter p4("p4");
    cout << p.toString().toStdString() << endl << p2.toString().toStdString() << endl
         << p3.toString().toStdString() << endl << p4.toString().toStdString() << endl;

    uml::Operation o1("o1");
    cout << o1.toString().toStdString() << endl;
    o1.stereotype = "<<abstract>>";
    o1.visibility = uml::VisibilityType::Type::Public;
    o1.parameters << p << p2 << p3 << p4;
    cout << o1.toString().toStdString() << endl;

    uml::Attribute a1("a1");
    a1.stereotype = "<<static>>";
    cout << a1.toString().toStdString() << endl;
    a1.stereotype.clear();
    a1.visibility = uml::VisibilityType::Type::Private;
    a1.type = "t1";
    cout << a1.toString().toStdString() << endl;
    a1.multiplicity = "multi1";
    a1.defaultValue = "d1";
    a1.property = "property1";
    cout << a1.toString().toStdString() << endl;


    //    c.attributes.push_back();

    QString s("QString");
    cout << s.toStdString() << endl;

    uml::ObjectFlow of;

    cout << "Hello World!" << endl;
    return 0;
}
*/

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

