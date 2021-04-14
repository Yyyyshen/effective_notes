// effective_cpp_05.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>

/**
 * 4.设计与声明
 * Designs and Declarations
 */

using namespace std;

//条款18：让接口容易被正确使用
//Make interfaces easy to use correctly and hard to use incorrectly
//
//类型问题
//例如定义一个日期类
class y_date {
public:
	y_date(int month, int day, int year) {};
};
//实际上，这样定义的话，日期中的有效范围没有被限定，可以提供wrapper types，来包装一下
struct y_day {
	explicit y_day(int d) :val(d) {}
	int val;
};
struct y_month {
	explicit y_month(int m) :val(m) {}
	int val;
};
struct y_year {
	explicit y_year(int y) :val(y) {}
	int val;
};
class y_date_2 {
public:
	y_date_2(const y_month& m, const y_day& d, const y_year& y) {};
};
//对于月份，或者星期，还能更完善去设计
class y_month_2 {
private:
	int val;
	explicit y_month_2(int m) :val(m) {};//不允许自己创建，只使用我们提供的范围
public:
	static y_month_2 Jan() { return y_month_2(1); }
	static y_month_2 Feb() { return y_month_2(2); }
	//...
	static y_month_2 Dec() { return y_month_2(12); }
};
class y_date_3 {
public:
	y_date_3(const y_month_2& m, const y_day& d, const y_year& y) {}
};
void test_date() {
	y_date_3 date(y_month_2::Dec(), y_day(30), y_year(2021));
}
//对于设计的types，其行为应该与内置类型一致
//例如日期可以加减计算，所以我们定义的wrapper类型也应遵循ints类型表现
//
//任何接口如果要求调用者需要记得某些操作，就意味着出错的可能性，可以在设计之处就避免或强制调用者去做
//例如工厂函数，返回一个资源时应当使用智能指针，防止资源泄漏
//而调用者恰巧忘记使用，就会出现问题
//一个设计原则是，直接让工厂函数返回一个智能指针，这就意味着调用者必须用此类型接收，也就避免了忘记删除资源的可能
// 
//总结
//好的接口应当容易被正确使用，促进正确使用的办法包括接口一致性，以及与内置类型行为兼容
//阻止误用的办法包括建立新的类型、限制类型上的操作，束缚对象值，以及消除调用者的资源管理责任
//（shared_ptr知识点：支持自定义删除器，可防范跨dll的new/delete操作问题，还可被用来自动解除互斥锁）
//


//条款19：设计class犹如设计type
//Treat class design as type design
//
//定义一个新class就定义了一个新type，除了设计class，也应当重载函数、操作符、控制内存、定义初始化和终结等
//需要考虑的事项有：
//新的类型对象如何创建和销毁——影响class构造和析构，以及operator new/new[]/delete/delete[]
//对象初始化和对象赋值的差别——copy构造和copy assignment的区分
//对象如果被passed by value（以值传递），意味着——copy构造用来定义一个type的pass-by-value如何实现
//什么是合法值——对class成员变量而言，通常有一个有效范围，意味着需要维护约束条件、进行错误检查
//定义的type是否需要配合某继承系列——如果继承自某classes，就要收到其设计约束，如果允许其他classes继承你的class，就会影响你的函数设计，如析构是否为virtual
//新type需要什么样的转换——如果希望有T1到T2的隐式转换，就需要在class T1内写operator T2或者在class T2写一个non-explicit-one-arg的构造
//什么操作符和函数是合理的——其他条款会说明哪些不该声明
//什么样的标准函数该驳回——如果认为不需要某些编译器会自动生成的函数，就该声明为private或使用delete
//成员——使用private、protected、public决定谁能取用哪些成员
//未声明接口
//一般化——如果你并非为了定义一个新类型，而是定义一整个types家族，就不该定义这个新class，应该使用class template
//考虑是否需要一个新type
//


//条款20：以pass-by-reference-to-const替换pass-by-value更好
//Prefer pass-by-reference-to-const to pass-by-value
// 
//缺省条件下，C++以by value方式传递对象至函数，除非另外指定，否则都是使用实参的副本，同样，调用所得返回值也是一个副本
//这些副本由对象的copy构造产出，所以效率比较低
//尤其是在类设计较复杂时，既有继承关系又存在一些成员变量，那么by value传递一次，则是继承体系内所有类型的构造及成员的构造都执行一遍，走出区块周期之后的析构同理
//例如
class A {
private:
	std::string a;
};
class B :public A {
private:
	std::string b;
};
void do_something_to_B(B b) {};
//调用此函数时，会有A构造、string a构造、B构造、string b构造，4次构造，也同样有4次析构
//而如果声明为如下
void do_something_to_b(const B& b) {}
//则效率多的很，没有任何构造和析构
//const保证了传进来的对象不会被改变，&表示by reference方式传递，可以避免对象切割（slicing）问题
//（多态体系中，当一个derived class对象以by value方式传递并被视为一个base class对象，则其派生类对象的特性将被切割掉）
//在C++底层，references往往是指针形式实现出来，所以传引用通常真正传递的就是指针
//所以，当传递的类型大小小于一个指针（如char，int等内置类型），选用pass by value就足够了，甚至效率会更高
//但并非所有小型types都适合以值传递，对于用户自定义的classes，对象本身小不意味着copy构造函数不昂贵
//再极端的说，自定义类型并没有复杂的copy构造，也还是有争议的，编译器对于内置类型和自定义类型处理还是有区别的
//因此，基本上适合pass-by-value的，只有 内置类型、STL迭代器、函数对象
//其他东西都尽量使用pass-by-reference-to-const
//


//条款21：必须返回对象时，不要试图返回其reference
//Don't try to return a reference when you must return an object
// 
//在理解了pass-by-value带来的效率问题之后，经常会过度的想要去使用引用传值
//以一个有理数设计为例
class y_rational {
public:
	y_rational(int n = 0, int d = 1) :n_(n), d_(d) {}
private:
	int n_, d_;//分子分母
	friend const y_rational operator*(const y_rational& lhs, const y_rational& rhs);
};
//如果一定想以reference返回，可能写成这样
/*const y_rational& operator*(const y_rational& lhs, const y_rational& rhs) {
	y_rational result(lhs.n_ * rhs.n_, lhs.d_ * rhs.d_);
	return result;//烂代码，result是个local对象，函数退出时，local对象就销毁了
}*/
//任何函数，都不要试图返回一个reference指向某个local对象
//于是，可以考虑在heap中构造对象，并返回引用指向它
/*const y_rational& operator* (const y_rational& lhs, const y_rational& rhs) {
	y_rational* result = new y_rational(lhs.n_ * rhs.n_, lhs.d_ * rhs.d_);
	return *result;
}*/
//看上去可行，但实际上还是付出了构造函数的代价，并没有达到目的，并且，new出来的对象该由谁实施delete也是问题
//即使调用者意识良好，时刻想着合理的delete，但一些情况下，还是无法处理
//例如， x*y*z这样，实际上调用为operator*(operator*(x,y),z)，需要两次delete，但并没有合适的方法能获取其中某些references去删除
//再去想办法，使用static
/*const y_rational& operator* (const y_rational& lhs, const y_rational& rhs) {
	static y_rational result;
	//result = ...
	return result;
}*/
//这更烂，不仅需要考虑多线程安全性，并且会让a*b==c*d永远为真，因为返回的都是同一个对象
//
//一个必须返回对象的函数，正确写法就是——返回一个新对象
inline const y_rational operator*(const y_rational& lhs, const y_rational& rhs) {
	return y_rational(lhs.n_ * rhs.n_, lhs.d_ * rhs.d_);
}


//条款22：将成员变量声明为private
//Declare data members private
//
//为什么不采用public变量
// 语法一致性上，调用者访问对象的方法是通过成员函数；这样访问成员时，都加小括号就完事了
// 声明为public的话所有人对该成员变量的权限就都一样了，而使用private并做封装可以控制各种只读/只写/读写权限
// 封装性，对于调用者来说，是个黑盒，当class内部改动时，不需要关心内部实现的变化，使用方法不变
// 
//总结
//切记将成员变量声明为private，可赋予客户访问数据的一致性，可细微划分访问控制，允诺约束条件获得保证，提供设计者充分的实现弹性
//另外，protected并不比public更具封装性
//


//条款23：以non-member、non-friend替换member函数
//Prefer non-member non-friend functions to member functions
//
//以一个浏览器功能为例
class y_browser {
public:
	void clear_cache() {};
	void clear_history() {};
	void clear_cookies() {};
};
//


int main()
{
	std::cout << "Hello World!\n";
}

