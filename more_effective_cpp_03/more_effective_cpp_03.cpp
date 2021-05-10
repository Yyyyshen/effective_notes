// more_effective_cpp_03.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
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
class Widget {};
void func(Widget& w) {}
//catch (Widget& w) {}
//
//异常从语法上看，从抛出端传递一个异常到catch语句中，很类似于函数调用时传递一个变量
//
istream operator>>(istream& s, Widget& w) { /* 从输入流读入一个Widget */ }
void throw_widget() {
	Widget local_w;
	cin >> local_w;//传递引用，没有发生复制
	throw local_w;
}
//函数传参和异常传递都有三种方式：by value，by reference，by pointer
//不同的是，如上例，catch时不论是以值或引用传递，都会发生local_w的拷贝行为（以值传递甚至会发生两次复制，一次是产生的临时对象，还有一次是将临时对象复制到参数中）
// 因为离开生存空间后，这个临时变量就析构了，这就是为什么C++特别声明，一个对象被抛出作为exception时，总会发生复制
// （即使不是因为离开scope而析构，比如staic变量，依然会产生副本，这也是为什么相比函数传参，抛异常效率更低）
// 也意味着，即使是引用方式catch异常，也并不能修改这个变量，改变的只能是其副本
// 
//对象被当作exception进行复制时，复制行为由copy constructor执行，对应的是对象的静态类型
// 就是说，如果有继承关系在其中，使用了派生类但最终以基类的引用抛出异常，则会认定为是基类对象，这可能并非设计者所想要的
// 
//看一下catch块中继续传递异常的两个方式
// catch(Widget& w){
//		...
//		throw;//重新抛出异常，继续传播
// }
// catch(Widget& w){
//		...
//		throw w;//传播被捕获异常的副本
// }
// 即使不考虑拷贝行为产生的性能成本，这两种写法也是有区别的
// 由于是静态类型，所以应当使用第一个方式，而且比较有效率
//


//条款13：以by reference方式捕捉exceptions
// 
//考虑到效率，在选择传递异常方式时，理论上应该选择by pointer
// 而在此过程中，需要保证指针所指对象依然存在，如果指向了跳出scope后就析构的临时对象，就会出问题
// 如果使用了heap object，避免了指向不存在对象，却容易因不好处理删除逻辑而造成资源泄露
//
//以值传递会复制两次，效率较差
//所以一般都会选择catch-by-reference
//


//条款14：明智运用exception specifications
// 
void f1() {};
void f2() throw(int) {
	//...
	f1();//合法，即使是f1可能抛出违反f2中exception specifications的异常
};
//
//虽然合法，但一般不应该这样设计
// 
//在template中
// 没有任何方法可以知道template的类型参数可能抛出什么exceptions
// 所以，不要为template提供exception specifications
// 
//另外，避免unexpected方式还有
// 处理系统可能抛出的exceptions
// （常见bad_alloc）
//


//条款15：了解异常处理（exception handling）的成本
// 
//应尽量避免使用try/catch块
// 代码膨胀
// 效率较低
//


int main()
{
	std::cout << "Hello World!\n";
}
