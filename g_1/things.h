#ifndef THINGS_H
#define THINGS_H

#include <QString>
#include <QList>

namespace uml {


/*
UML结构，也就是总览

UmlStructure
    BuildingBlocks，基本UML建模元素、关系和图
        Things，建模元素本身
            StructuralThing，UML模型中的名词，如类、接口、协作、用例、活动类、组件、节点
                Class
                Interface
                Collatoration
                UseCase
                ActiveClass
                Component
                Node
                （还有其他变体，如Actor、信号、实用程序、进程、线程、应用、文档、文件、库、页、表）

            BeavioralThing，UML模型的动词，如交互、状态机
                Interaction
                StateMachine

            GroupingThing，包，它用于把语义上相关的建模元素分组为内聚的单元
                Package （变体有框架、模型、子系统等）

            Annotational，注解，它附加到模型以捕获特殊信息，同黄色便笺很相像
                （note是一个依附于一个元素或一组元素之上，对它们进行约束或解释等简单符号）

        Relationships，把物件联系在一起，关系说明两个或多个物件时如何语义相关的
            Dependency （变体有精化、跟踪、包含和延伸）
            Association
            Generalization
            Realization

        Diagrams，UML模型的图，它们展现物件的集合，“讲述关于软件系统的故事”，是我们可视化系统将做什么（分析级图）或者系统如何做（设计级图）的方法
            ClassDiagram
            ObjectDiagram
            UseCaseDiagram
            InteractionDiagram
            SequenceDiagram
            StatechartDiagram
            ActivityDiagram
            ComponentDiagram
            DeploymentDiagram

    CommonMechanisms，达到特定目标的公共UML方法
        Specifications
        Adornments
        CommonDivisions
        ExtensibilityMechanisms

    Architecture，系统架构的UML视图
        UseCaseView
        LogicView
        ProcessView
        ImplementationView
        DeploymentView

*/

enum class EndType {
    None = 0,
    Component,
};

/*
 *
依赖(dependency)是一种使用关系，他描述一个事物的规约变化可能影响到使用它的另一个事物。个人认为在参数或者方法体中使用到另外的类就是对该类有依赖的关系。use a

泛化(dependency)用于描述子类到父类之间的关系。 Is a kind of

关联(association)是一种结构关系，他描述一个对象与另一个对象相联系。 Has a
双向关联(association)通过A对象可以找到B对象，B对象同样可以找到A对象的关联为双向关联。

单向关联(direction-association)通过A对象可以找到B对象，但通过B对象不能找到A对象的关联为单向关联。

聚合(aggreation)A对象是B对象的一个组成部份，但A对象同时可能是C对象的组成部分这种关联为聚合。

组合(composition)A对象是B对象的一个组成部份，除非B对象将A对象转交给其他对象，否则A对象只能是B对象的组成部分，这种关联为组合。

 */
enum class LineType {
    NoneValue = 0,
    Generalization,
    Implementation,
    Dependence,
    Association,
    Aggregation,
    Composition,
    Included,
    Extended,
    Refined,
};

// 负责数据的存储与载入
struct ISerialize {
    virtual ~ISerialize() {};
    virtual void save(QDataStream &dataStream) const = 0;
    virtual void load(QDataStream &dataStream) = 0;

    // 重载输入输出运算符，只能用友元函数
    friend QDataStream & operator << (QDataStream &dataStream, const ISerialize& s);
    friend QDataStream & operator >> (QDataStream &dataStream, ISerialize& s);
};

// 可见性通过在属性或方法名称前增加特定的符号表示。公共的（+）私有的的（-）受保护的（#）包内的（~）
struct VisibilityType: public ISerialize {
    enum Type {
        None = 0,
        Private = 1,
        Protected = 2,
        Public = 3,
        Package = 4
    };

    Type t_;

    VisibilityType()
        :t_(Type::None)
    {}

    VisibilityType(Type t)
        : t_(t)
    {}

    virtual ~VisibilityType()
    {}

    virtual void save(QDataStream &dataStream) const override;
    virtual void load(QDataStream &dataStream) override;

    QString toString() const;
};

struct Parameter: public ISerialize {
    QString s;
    QString inout;      // in, out, inout
    QString name;
    QString type;
    QString defaultValue;

    Parameter(const QString& n)
        : name(n)
    {}
    Parameter(const QString& n, const QString& t)
        : name(n), type(t)
    {}
    Parameter(const QString& n, const QString& t, const QString d)
        : name(n), type(t), defaultValue(d)
    {}
    Parameter(const QString& io, const QString& n, const QString& t, const QString& d)
        : inout(io), name(n), type(t), defaultValue(d)
    {}

    virtual ~Parameter() {}

    // 实现序列化
    virtual void save(QDataStream &dataStream) const override;
    virtual void load(QDataStream &dataStream) override;

    QString toString() const;
};

// 属性在类下面的第二栏中列出，可以仅显示操作名
// 静态操作与静态属性同样通过在名称下加下划线表示
// 抽象操作与抽象类同样通过斜体字表示
// 操作特征完整语法: [可见性] 操作名 [([方向] 参数名 ':' 参数类型 ['=' 默认值])] [':' 返回类型] [{特征串}]
// 不能重写的操作与属性一样使用特征串中增加 leaf 表示
struct Operation: ISerialize {
    QString stereotype;     // <<abstract>>, <<leaf>>, <<static>>
    VisibilityType visibility = VisibilityType::None;
    QString name;
    QList<Parameter> parameters;
    QString returnType;
    QString property;

    Operation(const QString& n)
        : name(n)
    {}

    virtual ~Operation()
    {}

    // 实现序列化
    virtual void save(QDataStream &dataStream) const override;
    virtual void load(QDataStream &dataStream) override;

    QString toString() const;
};

// <<stereotype>>opt visibility_opt name multiplicity_opt: type_opt = default_opt {property - string }opt
// 属性其他特征完整语法: [可见性] 属性名 [':'类型] [多重性] ['='初始值] [{特性串]}]
// 属性在类下面的栏中列出，可以仅显示属性名
// 不能重写属性通过在特性串中增加 leaf 特性说明
// 静态属性通过在属性名下加下划线表示
struct Attribute: public ISerialize {
    QString stereotype;     // <<static>>
    VisibilityType visibility;
    QString name;
    QString type;
    QString multiplicity;
    QString defaultValue;
    QString property;

    Attribute(const QString& n)
        :name(n)
    {}

    ~Attribute()
    {}

    // 实现序列化
    virtual void save(QDataStream &dataStream) const override;
    virtual void load(QDataStream &dataStream) override;

    QString toString() const;
};

struct Entity: public ISerialize {
    QString name;
    QString comment;

    Entity()
    {}

    Entity(const QString& n)
        : name(n)
    {}

    virtual ~Entity()
    {}

    // 实现序列化
    virtual void save(QDataStream &dataStream) const override;
    virtual void load(QDataStream &dataStream) override;
};

struct End: public Entity {
    Entity* entity = nullptr;
};

struct Line: public Entity {
    End source;
    End target;
};

struct Text: public Entity {
};

struct Note: public Entity {
};

struct Actor: public Entity {
};

struct UseCase: public Entity {
};

struct Subsystem: public Entity {
};

struct Interface: public Entity {
    QString stereotype;     // <<abstract>>, <<leaf>>
    QList<Operation> operations;

    Interface(const QString& n)
        : Entity(n)
    {}

    virtual ~Interface()
    {}
};

// 在UML中类以一个矩形表示，类的名称用一个字符串表示
// 抽象类通过将类名改为斜体字表
// 不能继承的类（叶子类，封闭类）通过在类名下面增加 leaf 特性说明。
struct Class: public Interface {
    QList<Parameter> templateParameters;
    QList<Attribute> attributes;

    Class(const QString& n)
        : Interface(n)
    {}

    virtual ~Class()
    {}
};

struct Package: public Entity {
};

/*
二、状态图元素（State Diagram Elements）
状态有Opened、Closed、Locked。

事件有 Open、Close、Lock和Unlock。

注意：

1、             并不是所有的事件都会引起状态的转移，比如当门是处于【Opened】状态，不能进行【Lock】事件。

2、             转移（Transition）有警备条件（guard condition），比如只有doorWay->isEmpty 条件满足时，才会响应事件。
*/


// 状态（States）
struct State: public Entity {
};

// 初态和终态（Initial and Final States）
// 初态用实心圆点表示，终态用圆形内嵌圆点表示。
struct InitialState: public Entity {
};

struct FinalState: public Entity {
};

// 转移（Transitions）
/*
 转移（Transitions）是两个状态之间的一种关系，表示对象将在源状态（Source State）中执行一定的动作，并在某个特定事件发生而且某个特定的警界条件满足时进入目标状态（Target State）



      事件标记（Trigger）：是转移的诱因，可以是一个信号，事件、条件变化（a change in some condition）和时间表达式。

      警界条件（Guard Condition）：当警界条件满足时，事件才会引发转移（Transition）。

      结果（Effect）：对象状态转移后的结果。
      */
struct Transition: public Entity {
};

// 动作（State Actions）
struct StateAction: public Entity {
};

// 自身转移（Self-Transitions）
struct SelfTransition: public Entity {
};

// 组合状态（Compound States）
struct CompoundState: public Entity {
};

// 进入节点（Entry Point）
struct EntryPoint: public Entity {
};

// 退出节点（Exit Point）
struct ExitPoint: public Entity {
};

// 历史状态（History States）
struct HistoryStates: public Entity {
};

// 并发区域（Concurrent Regions）
struct ConcurrentRegions: public Entity {
};

//
// 活动图中的各个元素
//

// 活动
struct Activity: public Entity {
};

// 动作
struct Action: public Entity {
};

// 动作约束
struct Conditions: public Entity {
};

// 控制流
struct ActivityEdge: public Entity {
};

// 初始节点
struct InitialNode: public Entity {
};

// 结束节点
struct FinalNode: public Entity {
};

// 对象
struct ObjectInActivity: public Entity {
};

// 对象流
struct ObjectFlow: public Entity {
};

// 数据存储
struct DataStore: public Entity {
};

// 判断节点
struct DecisionNode: public Entity {
};

// 合并节点
struct MergeNode: public Entity {
};

// 分叉节点
struct ForkNode: public Entity {
};

// 结合节点
struct JoinNode: public Entity {
};

// 扩展域
struct ExpansionRegion: public Entity {
};

// 异常处理器
struct ExceptionHandler: public Entity {
};

// 可中断活动区
struct InterruptibleActivityRegion: public Entity {
};

// 分割
struct Partitions: public Entity {
};

};

#endif // THINGS_H
