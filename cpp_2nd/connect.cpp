#include <list>
#include <functional>

template<typename R = void, typename... Args>
class Fns
{
private:
    std::list<std::function<R(Args...)>> _calls;

public:
    virtual ~Fns()
    {
        _calls.clear();
    }

    void connect(std::function<R(Args...)> fct)
    {
        _calls.push_back(fct);
    }

    template<typename Object>
    void connect(Object* object, R (Object::*method)(Args...))
    {
        _calls.push_back([object,method](Args... args){(*object.*method)(args...);});
    }

    template<typename Object>
    void connect(Object* object, R (Object::*method)(Args...) const)
    {
        _calls.push_back([object,method](Args... args){(*object.*method)(args...);});
    }

    template<typename Object>
    void connect(const Object* object, R (Object::*method)(Args...) const)
    {
        _calls.push_back([object,method](Args... args){(*object.*method)(args...);});
    }

    void emit(Args... args)
    {
        for(auto call : _calls)
            call(args...);
    }
};

class Foo
{
public:

    void bar(int x, int y)
    {
        printf("Foo::bar(%d, %d)\n", x, y);
    }

    void bar3(int x, int y) const
    {
        printf("Foo::bar3(%d, %d)\n", x, y);
    }
};

void foobar(int x, int y)
{
    printf("foobar(%d, %d)\n", x, y);
}

int foobar2(int x, int y)
{
    printf("foobar2(%d, %d)\n", x, y);
    return x+y;
}

int main(void)
{
    Foo foo;

    Fns<int, int, int> s2;
    s2.connect(foobar2);
    s2.emit(3, 5);

    Fns<void, int, int> s;
    // Connect a function
    s.connect(foobar);
    // Connect a class method
    s.connect(&foo, &Foo::bar3);
    // Create and connect some lambda expression
    s.connect([&foo](int x, int y){
        printf("lambda::"); foo.bar(x, y);
    });
    // Emit the signal !
    s.emit(4, 2);
    getchar();
    return 0;
}
