协程是能暂停执行以在之后恢复的(函数)
每个协程都与下列对象关联：
- promise
- handle
- state, 协程启动后自动分配，位于堆上，包含协程所需的各类数据, 如
	- promise
	- handle
	- 协程参数 按值复制
	- 暂停点
	- 局部变量和临时变量

作为函数，协程拥有参数和返回类型，参数是可选的，返回类型需要满足以下形式

```cpp
struct ReturnObject  
{  
    struct promise_type  
    {  
        ReturnObject get_return_object() { return {}; }  //协程使用该函数构建返回值对象
        awaitable initial_suspend() { return {}; } //获取初始暂停点
	    awaitable final_suspend() noexcept { return {}; } //获取最终暂停点
        void unhandled_exception() {}  
    };
};
```

编译器用 std::coroutine_traits 从协程的返回类型获取promise_type

协程启动后，在堆上分配内存，构建state，拷贝函数形参

接着构建promise，如果promise的构造函数和协程参数一致，那么将使用拷贝后的参数调用构造函数，否则调用默认构造函数

promise构建后, 使用 promise.get_return_object()构建返回值类型

然后开始进入初始暂停点(initial_suspend)

协程到达co_return后，以创建顺序的逆序销毁所有具有自动存储期的变量

然后进入最终暂停点(final_suspend)

暂停点返回awaitable对象，控制协程的行为，内置的awaitable对象如下

```cpp
struct suspend_never  //不暂停
{  
  constexpr bool await_ready() const noexcept { return true; } 
  
  constexpr void await_suspend(coroutine_handle<>) const noexcept {}  
  
  constexpr void await_resume() const noexcept {}  
};

struct suspend_always  //暂停
{  
  constexpr bool await_ready() const noexcept { return false; }
  
  constexpr void await_suspend(coroutine_handle<>) const noexcept {}  
  
  constexpr void await_resume() const noexcept {}  
};
```

当协程结束，会进行以下操作
-   调用承诺对象的析构函数。
-   调用各个函数形参副本的析构函数。
-   调用 [operator delete](https://zh.cppreference.com/w/cpp/memory/new/operator_delete "cpp/memory/new/operator delete") 以释放协程状态所用的内存。
-   转移执行回到调用方/恢复方

下面用个简单的例子来验证这个流程

```cpp
#include <coroutine>  
#include <iostream>  
  
struct HelloWorldCoro  
{  
    struct promise_type  
    {  // compiler looks for `promise_type`  
        HelloWorldCoro get_return_object() { return this; }  
        std::suspend_never initial_suspend() noexcept { return {}; } //不暂停 
        std::suspend_always final_suspend() noexcept { return {}; }  //暂停
        void unhandled_exception() {}  
    };  
    HelloWorldCoro(promise_type* p)  
        :m_handle(std::coroutine_handle<promise_type>::from_promise(*p))  //在此获取handle
    {} 

	// 由于在最终结束点暂停，且没有进行恢复，因此需要手动释放state，不然会造成内存泄漏
	// 如果结束点不暂停，那么协程的state就会被自动释放，该处的destory就会变成double free
    ~HelloWorldCoro() { m_handle.destroy(); }  
  
    std::coroutine_handle<promise_type> m_handle;  
};  
  
HelloWorldCoro print_hello_world()  
{  
    std::cout << "Hello ";  
    co_await std::suspend_never{};  //不暂停
    std::cout << "World!" << std::endl;  
}  
  
int main()  
{  
    HelloWorldCoro mycoro = print_hello_world();  
    return EXIT_SUCCESS;  
}
```



co_await的行为
调用await_ready(),判断是否需要暂停
调用await_suspend(handler)切换控制权