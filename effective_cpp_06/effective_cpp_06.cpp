// effective_cpp_06.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>

/**
 * 5.实现
 * Implementation
 */

using namespace std;

//条款26：尽可能延后变量定义式的出现时间
//Postpone variable definitions as long as possible
// 
//照旧，举个栗子
void encrypt(std::string& str) {}//加密，省略实现
std::string encrypt_password(const std::string& password) {
	string encrypted;
	if (password.length() < 8) throw exception("Password is too short");
	//...加密
	return encrypted;
}
//问题产生，如果抛异常，则这个string凭空付出了构造和析构的成本，并且没有达到目的
//最简单的处理，将次序调换，先判断长度，再开始正常操作
//并且，应该去掉无意义的default构造过程
std::string encrypt_password_2(const std::string& password) {
	if (password.length() < 8) throw exception("Password is too short");
	std::string encrypted(password);//直接copy构造
	encrypt(encrypted);
	return encrypted;
}
//延后的真正意义在于，直到非得使用该变量的前一刻为止，甚至直到能够给它初值实参为止
//这样不仅避免非必要对象的构造和析构，也能减少无意义的default行为
//
//对于循环
class A {};
void test_loop() {
	A a;
	int n = 100;
	for (int i = 0; i < n; ++i) {
		//执行一些与a有关操作，例如 a=..
	}
	for (int i = 0; i < n; ++i) {
		A a_in_loop;
		//执行一些与a有关操作
	}
}
//第一种做法是1次构造+1次析构+n次赋值；第二种做法次构造+n次析构
//考虑两件事
//对于该类型对象，一个赋值成本是否低于一组构造+析构（copy assignment一般会构造新对象，析构之前对象，所以效率上不一定谁更高）
//该对象的作用域，考虑是否需要在循环外使用
//


//条款27：尽量少做转型操作
//Minimize casting
//
//几种转型语法
void test_casts() {
	//(T)expression;
	//T(expression);//C风格函数形式；如果是C++，是使用expression构造一个T对象
	//以上两种都是将expression转为T，C风格中是没有区别的，属于旧式转型

	//C++中四种新式转型
	//const_cast<T>(expression);//通常被用来将对象常量性移除
	//dynamic_cast<T>(expression);//安全向下转型，用来决定某对象是否归属某个继承体系
	//reinterpret_cast<T>(expression);//意图执行低级转型，实际动作和结果可能取决于编译器（涉及移植性）。例如将pointer to int转为int，很少见
	//static_cast<T>(expression);//强制隐式转换，例如将non-const转为const，int转double，基类指针转派生类指针等（但const转non-const只能用const_cast)
}
//栗子
class Widget {
public:
	explicit Widget(int size) {}
	//...
};
void do_something(const Widget& w) {}
void test_casts_2() {
	//do_something(15);//声明使用了explicit，所以不能直接这样用
	do_something(Widget(15));//这即能看成C++中使用了对应类型构造生成对象；也能看成C风格的旧式转型
	do_something(static_cast<Widget>(15));//是用新式转型强制转换

	//算术转换
	int x = 1, y = 3;
	double d = static_cast<double>(x) / y;
}
//继承体系中
class Base {};
class Derived :public Base {};
void test_derived() {
	Derived d;
	Base* pb = &d;//隐喻地将Derived*转换为Base*
}
//这就表明，单一对象（例如Derived）可能拥有一个以上地址（Base*指向它、Derived*指向它时的地址，两者之间有一个偏移量offset）
//C不会有这个情况（没有对象概念），其他面向对象语言（Java、C#）不会有这个情况，但C++可能，尤其是多重继承中
//所以，将对象地址转型为char*指针后进行指针运算，几乎总是导致不明确行为
//
//在许多应用框架，都要求derived classes内virtual函数代码第一个动作就是先调用base classes的对应函数（有点像Java里的super.member_func();）
class Window {
public:
	virtual void onResize() {}
};
class SpecialWindow :public Window {
public:
	virtual void onResize();
};
//如果需要先调用父类再做子类专属行为，尝试一个做法
/*void SpecialWindow::onResize() {
	static_cast<Window>(*this).onResize();
	//...子类行为
}*/
//强调转型动作，编译能够通过，但实际上是错误的方式
//*this确实转型为Window，调用的onResize也确实是Window::onResize，但并不是当前对象，而是转型动作所建立的“*this对象的base class成分”的临时副本的onResize
//（成员函数都隐喻了一个this指针，所以会影响成员函数操作的数据）
//如果Window中函数改变了对象内容（因为没有声明为const成员函数），则调用后当前对象并没有被改动，而是改动的那个临时副本
//此时如果子类该函数专属行为又改动了对象，则会导致不正确结果（base成分改动没落实、derived成分改动落实了）
//正确的做法
void SpecialWindow::onResize() {
	Window::onResize();//调用Window的onResize作用于*this
	//...子类行为
}
//这表示，并不是将*this看作base对象，只是想调用base版本的成员函数作用于当前对象上
//
//这个问题上，用dynamic_cast也是一样的，并且dynamic_cast许多实现版本执行速度相当慢
//效率慢的例子如：转换时对class名称的字符串比较，在n层深的单继承体系内某个对象身上执行dynamic_cast，可能会有n次strcmp调用
//使用该转型方式的时机需要考量，常见的场景是
//在一个认定为derived class对象上执行其操作函数，但你手上只有指向其base的指针或引用
//要避免这个问题，两个方法
//第一个是使用容器存储直接指向derived class对象的（智能）指针，缺点就是不能存储多种base的派生类了，那时将需要多个容器来维护
//第二个是将派生类可能的专属行为函数以virtual声明在base class内，并提供什么都不做的空实现
//虽然这些方法都需要注意类型安全，但尽量还是避免使用dynamic_cast
// 
//总结
// 尽量避免转型，特别是注重效率时避免dynamic_casts，当需要转型时，尝试重新设计成无需转型的结构
// 如果转型是必要的，尝试隐藏在某函数背后，调用者可以调用该函数而不用自己去转型
// 尽量使用新型转型，不要用旧式转型
//


//条款28：避免返回handles指向对象内部成分
//Avoid returning handles to object internals
// 
//看一个设计
class Point {
public:
	Point(int x, int y) {}
	void setX(int newVal) {}
	void setY(int newVal) {}
};
//定义一个形状，但不直接存储数据，以辅助struct进行
struct RectData {
	Point ulhc;
	Point lrhc;
};
//在类中只保存指向该数据结构的指针
class Rectangle {
private:
	std::shared_ptr<RectData> pData;
public:
	Rectangle(Point& p1, Point& p2) {}
	//提供返回指针指向数据的接口
	Point& upperLeft() const { return pData->ulhc; }
	Point& lowerRight() const { return pData->lrhc; }
};
//编译通过，但设计是错误的
//函数upperLeft和lowerRight被声明为const，提供调用者一个得知坐标点的方法，并没想令其修改
//而两个函数返回references指向private内部数据，调用者其实可以通过这些引用来修改数据
void test_Rect() {
	Point p1(0, 0);
	Point p2(100, 100);
	const Rectangle rec(p1, p2);
	rec.upperLeft().setX(50);//合法，但并不符合设计初想，因为rec声明为const，本应无法修改
}
//这就提醒了我们
// 第一，成员变量的封装性最多只等于“返回其reference”的函数的访问级别。
// 如果定义了private成员却又提供了public成员函数返回该成员的non-const reference，从封装角度看它就是public的
// 第二，如果const成员函数传出一个reference，后者所指数据于对象自身有关联，而所指数据通常又不在对象之内
// 那么这个函数调用者就可以修改那笔数据，这在讨论const成员函数时，也提到过（bitwise constness）
// 
//引用、指针、迭代器这类可以称为handle（windows里有个句柄）
//返回代表对象内部数据的handle就会降低封装性
//
//处理上面Rectangle中情况的最简单办法，在返回类型加const即可 —— const Point&
//这样调用者就是只可读，但不可写
//但尽量还是避免这种返回handles的情况，很容易出错
//


//条款29：为异常安全而努力
//Strive for exception-safe code
// 
//如同之前讨论过的，copying行为中要new新对象并删除旧对象，这期间new是可能发生异常的
//多线程环境下，这些步骤进行加锁处理，则如果发生异常，解锁就不会被调用，发生死锁；还可能发生资源泄漏
//解决锁问题可以使用自动锁，利用作用域和构造析构加解锁
//
//异常安全函数提供以下三个保证之一
// 基本承诺：如果异常被抛出，程序内任何事物仍然处于有效状态
// 强烈保证：如果异常被抛出，程序状态不改变。即如果失败，应该恢复到执行此段代码之前的状态
// nothrow保证：承诺绝不抛出异常。内置类型（int、指针等）的操作都提供nothrow保证
// 
//强烈保证往往能用copy-and-swap实现出来，强烈保证表示整体的保证，即使一个整体中有极小一部分没有做到强烈保证，也就不能说整体是强烈保证
//


//条款30：理解inlining的内外
//Understand the ins and outs of inlining
//
//inline函数，外观、动作像函数，比宏更优秀，调用他们又没有函数调用导致的额外开销
//编译器最优机制常用于“不含函数调用”的代码，用inline时，编译器就可能以最优来对待它
//inline的观念是——将“对该函数的每一个调用”都以函数本体替换
//所以，这会增加目标码（object code）大小，增大程序体积
//在内存有限的机器，过度使用inline造成的代码膨胀会导致额外的换页行为，降低指令高速缓存装置的集中率，往往伴随着效率损失
//
//inline是对编译器行为的一种申请，不是强制命令，具体是否被处理为inline，由编译器决定
//除了明确使用inline关键字声明，也有隐喻方式
class Person {
public:
	int age() const { return age_; }//定义于class定义式内部的成员函数，就是一个隐喻的inline申请
private:
	int age_;
};
template<typename T>
inline const T& y_max(const T& a, const T& b) {//std::max实现，明确声明inline
	return a < b ? b : a;
}
//有一个误解就是，通常inline和templates通常定义于头文件，并不意味着function templates一定要inline
//inline在C++程序中大多为编译期行为。将函数调用替换为被调用函数本体，这个操作需要知道函数的样子
//templates同样，一旦被使用，编译器为了将它具象化，需要知道长什么样子
//template具象化与inline无关，如果正在写一个template并且认为此模板具象的函数都应该inline，才进行声明
//inline需要成本，如果没必要将每种具象化函数都inline，就该避免
// 
//inline被编译器拒绝的常见情况：过于复杂的函数（循环、递归）、virtual函数的调用（虚函数是直到运行期才决定调用哪个，而inline是执行前先将调用动作替换为本体）
// 
//还有时，编译器有意愿inline某个函数，还是可能为该函数生成一个本体，例如
inline void f() {}//“对f的调用”将被inline
void test_f() {
	f();//正常调用，inlined
}
void (*pf)() = f;
void test_pf() {
	pf();//这个调用或许不被inlined，因为通过函数指针调用
}
//
//再看一个构造函数例子
class B {
public:
	//...
private:
	string str1, str2;
};
class C :public B {
public:
	C() {} //定义于class定义式内部，又什么都没做，是inline吗？
private:
	string str1, str2, str3;
};
//实际上这个构造函数做了很多事，用代码简单表示如下
//
//C::C() {
//	B::B();//初始化父类成分
//	try { /* 构造str1 */ }
//	catch (...) {
//		B::~B();//销毁父类
//		throw;
//	}
//	try { /* 构造str2 */ }
//	catch (...) {
//		//析构str1
//		B::~B();//销毁父类
//		throw;
//	}
//	try { /* 构造str3 */ }
//	catch (...) {
//		//析构str2
//		//析构str1
//		B::~B();//销毁父类
//		throw;
//	}
//}
//
//编译器的处理会更复杂
//
//还需要评估的一点是，inline函数无法随着程序库升级而升级
// 如果f为程序库内一个inline函数，调用者将f本体编进程序，若程序库中f改变，则用到f的程序都必须重新编译
// 对于lib库，重新链接比重新编译负担会少得多，对于dll库，甚至换个文件就行；而inline会打破这个便利
// 
//选择inline的依据：
// 在设计开始时，先不要将任何函数声明为inline，或只局限于“一定inline”和“实现确实十分简单”的函数身上
// 80-20法则，程序80%的执行时间往往花在20%的代码上，找出可以有效增进效率的这部分代码区选择性的进行inline优化
// （想要inline带来更高的投入产出比，配合profile信息优先inline hot function是必要的）
//


//条款31：将文件之间的编译依赖关系降至最低（解耦）
//Minimize compliation dependencies between files
//
//这个问题影响到每次修改代码后，需要重新编译和链接部分的效率
// 
//C++并没有把“将接口从实现中分离”这件事做的很好
//class定义式不仅描述了class接口，还有各种成员变量
//每个成员变量将需要对应的#include包含的文件来支持，这就形成了编译依赖，其他调用者使用我们自定义class文件时也同理
//编译器需要在编译期间直到对象大小，以便分配栈内存，而获得class对象大小的方式就是询问class定义式，所以如果定义式中没有列出实现细目，就无法得知所需空间
// 此问题在Java等语言并不存在，因为他们的编译器是分配足够空间给一个指针指向该对象来使用
// （对于这类语言，所有用户自定义的class对象都是分配在堆上的，栈上存放其大小固定的指针）
// 当然，C++也可以这样去写，比如分割一个类为两个，其中一个只提供实现，另一个只提供接口和一个其实现的指针（之前提过的pimpl）
// 使用pimpl方式设计的类做到了接口与实现分离
// 
//分离的关键在于以“声明依存性”替换“定义依存性”，这是编译依赖最小化的本质
//让头文件尽量自给自足，做不到时，则让它与其他文件内的声明式（而非定义式）相依赖
//几个设计策略：
// 1.能用object references或object pointers可以完成任务，就不要用objects。（只靠声明式就定义出指向该类型的引用或指针，如果定义某类型objects，就需要定义式了）
// 2.尽可能以class声明式替换class定义式。（当声明一个函数用到某个class，并不需要该class的定义）
class Date;//class声明式
void clearAppointments(Date d);
// 如果能将“提供class定义式”（#include）义务从“函数声明所在”的偷吻加转移到“内含函数调用”的文件，便可将“并非真正必要之类型定义”与客户端的编译依赖去掉
//（感觉这里说的太复杂了，其实就是平时最常见的习惯，在.cpp中写#include，而.h文件中不写）
// 3.为声明式和定义式提供不同头文件
// 
//Handle classes（使用pimpl技巧的类）和Interface classes（一种抽象基类，没有成员变量和构造函数，只有一个virtual析构及一组纯虚函数来描述接口）
// 这两种类解除了接口和实现之间的耦合度，降低文件间的编译依赖
// 但代价也是有的，一般这样的设计会让我们在运行期丧失若干速度、对每个对象超额付出若干内存
// 对于Handle classes，成员函数通过指针取得对象数据，则会增加每次访问的间接性，以及动态分配（释放）得来的额外开销
// 对于Interface classes，每个函数都是virtual得，所以每次函数调用都付出一个间接跳跃，派生对象也将内含一个vptr，增加内存使用
// 一旦脱离inline函数，都无法有太大作为
//
//总结
// 编译依赖最小化的一般构想是，依赖于声明式而非定义式
// 程序库头文件应该以“完全且仅有声明式（full and declaration-only forms）”形式存在
// 我们需要在编译及运行成本和使用这些技巧中间衡量利弊
//


int main()
{
	std::cout << "Hello World!\n";
}
