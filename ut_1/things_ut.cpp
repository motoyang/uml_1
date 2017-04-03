#include <iostream>
#include <list>
#include "doctest/doctest.h"
#include "things.h"

using namespace std;
//using namespace uml;

void print_interface(const uml::Interface& i)
{
    cout << "Interface output--------------------------------------------------" << endl;
    cout << i.stereotype.toStdString() << endl;
    cout << i.name.toStdString() << endl;
    for_each(i.operations.cbegin(), i.operations.cend(), [] (const uml::Operation& o) {
        cout << o.toString().toStdString() << endl;
    });
}

void print_class(const uml::Class& c)
{
    cout << "Class output------------------------------------------------------" << endl;
    cout << c.stereotype.toStdString() << endl;
    cout << c.name.toStdString();
    if (c.templateParameters.size()) {
        cout << "<";
        for (auto i = c.templateParameters.cbegin(); i != c.templateParameters.cend(); ++i) {
            if (i != c.templateParameters.cbegin()) {
                cout << ", ";
            }
            cout << i->toString().toStdString();
        }
        cout << ">";
    }
    cout << endl;

    for_each(c.attributes.cbegin(), c.attributes.cend(), [] (const uml::Attribute& a) {
        cout << a.toString().toStdString() << endl;
    });
    for_each(c.operations.cbegin(), c.operations.cend(), [] (const uml::Operation& o) {
        cout << o.toString().toStdString() << endl;
    });
}

void test_out_1()
{
    uml::Parameter p("inout", "p1", "t1", "d1");
    uml::Parameter p2("p2", "t2", "d2");
    uml::Parameter p3("p3", "t3");
    uml::Parameter p4("p4");
    cout << "Parameter output--------------------------------------------------" << endl;
    cout << p.toString().toStdString() << endl << p2.toString().toStdString() << endl
         << p3.toString().toStdString() << endl << p4.toString().toStdString() << endl;

    cout << "Operation output--------------------------------------------------" << endl;
    uml::Operation o1("o1");
    cout << o1.toString().toStdString() << endl;
    o1.stereotype = "<<abstract>>";
    o1.visibility = uml::VisibilityType::Type::Public;
    o1.parameters << p << p2 << p3 << p4;
    cout << o1.toString().toStdString() << endl;

    cout << "Attribute output--------------------------------------------------" << endl;
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

    uml::Interface i1("i1");
    i1.stereotype = "<<abstract>>";
    i1.operations << o1;
    print_interface(i1);

    uml::Class c1("c1");
    c1.stereotype = "<<static>>";
    c1.templateParameters << p3 << p4;
    c1.operations << o1;
    c1.attributes << a1;
    print_class(c1);
}

TEST_CASE("lots of nested subcases") {
    cout << endl << "root" << endl;
    SUBCASE("") {
        cout << "1" << endl;
        SUBCASE("") {
            cout << "1.1" << endl;
            test_out_1();
        }
    }
/*
    SUBCASE("") {
        cout << "2" << endl;
        SUBCASE("") { cout << "2.1" << endl; }
        SUBCASE("") {
            cout << "2.2" << endl;
            SUBCASE("") {
                cout << "2.2.1" << endl;
                SUBCASE("") { cout << "2.2.1.1" << endl; }
                SUBCASE("") { cout << "2.2.1.2" << endl; }
            }
        }
        SUBCASE("") { cout << "2.3" << endl; }
        SUBCASE("") { cout << "2.4" << endl; }
    }
*/
}


TEST_CASE("lots of nested subcases2") {
    cout << endl << "root2" << endl;
    SUBCASE("") {
        cout << "1" << endl;
        SUBCASE("") {
            cout << "1.1" << endl;
            test_out_1();
        }
    }
/*
    SUBCASE("") {
        cout << "2" << endl;
        SUBCASE("") { cout << "2.1" << endl; }
        SUBCASE("") {
            cout << "2.2" << endl;
            SUBCASE("") {
                cout << "2.2.1" << endl;
                SUBCASE("") { cout << "2.2.1.1" << endl; }
                SUBCASE("") { cout << "2.2.1.2" << endl; }
            }
        }
        SUBCASE("") { cout << "2.3" << endl; }
        SUBCASE("") { cout << "2.4" << endl; }
    }
*/
}

