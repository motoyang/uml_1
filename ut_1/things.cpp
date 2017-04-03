#include <QDataStream>
#include <QMap>
#include "things.h"

namespace uml {

//
// ISerialize的友元函数
//

// 重载输出运算符，只能用友元函数
QDataStream & operator << (QDataStream &dataStream, const ISerialize& s)
{
    s.save(dataStream);
    return dataStream;
}

// 重载输入运算符，只能用友元函数
QDataStream & operator >> (QDataStream &dataStream, ISerialize& s)
{
    s.load(dataStream);
    return dataStream;
}

//
// VisibilityType
//
QString VisibilityType::toString() const
{
    const QMap<int, QString> m = {
        {VisibilityType::Type::None, QStringLiteral(" ")},
        {VisibilityType::Type::Package, QStringLiteral("~")},
        {VisibilityType::Type::Public, QStringLiteral("+")},
        {VisibilityType::Type::Protected, QStringLiteral("=")},
        {VisibilityType::Type::Private, QStringLiteral("-")},
    };

    return m.value(t_);
}

void VisibilityType::save(QDataStream &dataStream) const
{
    int i = t_;
    dataStream << i;
}

void VisibilityType::load(QDataStream &dataStream)
{
    int i;
    dataStream >> i;
    t_ = (Type)i;
}

//
// Entity
//
void Entity::save(QDataStream &dataStream) const
{
    dataStream << name << comment;
}

void Entity::load(QDataStream &dataStream)
{
    dataStream >> name >> comment;
}

//
// Parameter
//
void Parameter::save(QDataStream &dataStream) const
{
    dataStream << inout << name << type << defaultValue;
}

void Parameter::load(QDataStream &dataStream)
{
    dataStream >> inout >> name >> type >> defaultValue;
}

QString Parameter::toString() const
{
    QString r = QStringLiteral("%1%2%3%4").arg(inout.isEmpty()? inout: inout + " ")
            .arg(name).arg(type.isEmpty()? type: ": " + type)
            .arg(defaultValue.isEmpty()? defaultValue: "=" + defaultValue);
    return r;
}

//
// Operation
//
void Operation::save(QDataStream &dataStream) const
{
    dataStream << stereotype << visibility << name << parameters.size();
    foreach (const Parameter& p, parameters) {
        dataStream << p;
    }
    dataStream << returnType << property;
}

void Operation::load(QDataStream &dataStream)
{
    int count = 0;
    dataStream >> stereotype >> visibility >> name >> count;
    for (int i = 0; i < count; ++i) {
        Parameter p("");
        dataStream >> p;
        parameters << p;
    }
    dataStream >> returnType >> property;
}

QString Operation::toString() const
{
    QString allPara;
    foreach (const Parameter& p, parameters) {
        if (!allPara.isEmpty()) {
            allPara += QStringLiteral(", ");
        }
        allPara += p.toString();
    }

    // [可见性] 操作名 [([方向] 参数名 ':' 参数类型 ['=' 默认值])] [':' 返回类型] [{特征串}]
    QString r = QStringLiteral("%1%2%3(%4)%5%6")
            .arg(stereotype.isEmpty()? stereotype: stereotype + " ")
            .arg(visibility.toString())
            .arg(name)
            .arg(allPara)
            .arg(returnType.isEmpty()? returnType: ": " + returnType)
            .arg(property.isEmpty()? property: "{" + property + "}");

    return r;
}

//
// Attribute
//
void Attribute::save(QDataStream &dataStream) const
{
    dataStream << stereotype << visibility << name << type << multiplicity << defaultValue << property;
}

void Attribute::load(QDataStream &dataStream)
{
    dataStream >> stereotype >> visibility >> name >> type >> multiplicity >> defaultValue >> property;
}

QString Attribute::toString() const
{
    // [可见性] 属性名 [':'类型] [多重性] ['='初始值] [{特性串]}]
    QString r = QStringLiteral("%1%2%3%4%5%6%7")
            .arg(stereotype.isEmpty()? stereotype: stereotype + " ")
            .arg(visibility.toString())
            .arg(name)
            .arg(type.isEmpty()? type: ": " + type)
            .arg(multiplicity.isEmpty()? multiplicity: "[" + multiplicity + "]")
            .arg(defaultValue.isEmpty()? defaultValue: "=" + defaultValue)
            .arg(property.isEmpty()? property: "{" + property + "}");

    return r;
}

};
