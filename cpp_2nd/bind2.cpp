/*
 * File:   main2.cpp
 * Author: Vicky.H
 * Email:  eclipser@163.com
 */
#include <iostream>
#include <typeinfo>


void sayHello() {
    std::cout << "Hello world !" << std::endl;
}

int sum(int i, int j, int k) {
    return i + j + k;
}

template <typename T>
class Func {
public:

    Func(T fun) {
        if (!fun) {
            throw "fun nullptr";
        }
        _fun = fun;
    }

    template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
    R Call(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) {
        return _fun(a1, a2, a3, a4, a5);
    }

    template<typename R, typename A1, typename A2, typename A3, typename A4>
    R Call(A1 a1, A2 a2, A3 a3, A4 a4) {
        return _fun(a1, a2, a3, a4);
    }

    template<typename R, typename A1, typename A2, typename A3>
    R Call(A1 a1, A2 a2, A3 a3) {
        return _fun(a1, a2, a3);
    }

    template<typename R, typename A1, typename A2>
    R Call(A1 a1, A2 a2) {
        return _fun(a1, a2);
    }

    template<typename R, typename A1>
    R Call(A1 a1) {
        return _fun(a1);
    }

    template<typename R>
    R Call() {
        return _fun();
    }

    void Call() {
        _fun();
    }

private:
    T _fun;
};

#include <functional>

template<typename R = void, typename... Args>
class Fn {
public:
    Fn(std::function<R(Args...)> fun) : _fun(fun) {
    }

    R operator()(Args... args) {
        return _fun(args...);
    }
private:
    std::function<R(Args...) > _fun;
};

/*
 * 将函数注册到对象中，通过对象直接调用
 */
int main2(void) {


    Func<void(*)() > sayHelloFunc(sayHello);
    sayHelloFunc.Call();


    Func<int (*)(int, int, int) > sumFunc(sum);
    std::cout << "sumFunc.Call<int>(1, 2, 3) : " << sumFunc.Call<int>(1, 2, 3) << std::endl;


    std::cout << "\n---------------------------" << std::endl;

    Fn<> sayHelloFn(sayHello);
    sayHelloFn();

    Fn<int, int, int, int> sumFn(sum);
    std::cout << "sumFn(1, 2, 3) : " << sumFn(1, 2, 3) << std::endl;

    std::cout << "\n---------------------------" << std::endl;

    return 0;
}
