﻿// more_effective_cpp_03.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>

/**
 * 异常
 */

using namespace std;

//条款9：利用destructors避免泄漏资源
//
//在一段生命周期内如果使用了heap对象，则应该把资源封装在对象内
//利用对象的析构自动处理资源删除
//这样通常可以在发生异常时避免没有回收资源而发生泄漏
// 
//智能指针就是这样的思想
//


//条款10：在constructors内阻止资源泄漏
// 
//假设一个类设计中有两个heap资源
// 构造负责new，析构负责delete。由于C++保证删除null指针时安全的，所以删除前也不必检查是否真正指向某些东西
// 这样在正常情况下，流程很清晰，没问题
// 而试想，在构造中，第二个资源new operator时发生了异常，此时析构时不会被调用的，第一个资源又已经产生，就发生了资源泄漏
// 即使是将该对象分配在堆中，发生异常时delete，对象内资源还是泄漏了，因为除非new动作成功，否则对指针的赋值动作不会执行，删除时也是delete了null指针
// 
//在这种情况，就只能自己在构造中捕获异常并处理了
//或者说，使用智能指针，遵循条款9
//


//条款11：禁止异常流出destructors之外
// 
//析构在两种情况下被调用
// 离开生存空间（scope）后正常被销毁或明确被delete
// 对象被exception处理机制stack-unwinding栈展开销毁
// 
//也就是在设计destructor函数时，应该保守假设又一个异常正在作用
//当析构内调用了一个会产生异常的函数，应该使用trycatch处理，避免异常传播出去
//


//条款12：了解“抛出一个exception”与“传递一个参数”或“调用一个虚函数”之间的差异
// 
//
//


int main()
{
    std::cout << "Hello World!\n";
}
