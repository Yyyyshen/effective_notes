// effective_cpp_01.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>

/**
 * 参考《Effective C++》
 */

using namespace std;

//首先，一些术语

//声明（declaration），告诉编译器某个东西的名称和类型（type），但略去细节
extern int x;//对象声明（虽然是内置类型，但对象并不单指用户自定义类型的变量）
std::size_t numDigits(int num);//函数声明，返回值size_t为一个typedef
class Widget;//类声明
template<typename T> class GraphNode;//模板声明

//签名式（signature），每个函数的参数及返回类型
//例如上述numDigits，函数签名就是std::size_t (int)，表示函数获取一个int并返回size_t

//定义式（definition），提供编译器一些声明式所遗漏的细节
//对于对象而言，定义式是编译器为此对象拨发内存的地点；对于函数或函数模板，定义式提供了代码本体；对class和类模板，定义式列出了成员
int x;//对象定义
std::size_t numDigits(int num) {//函数定义
	std::size_t digitsSoFar = 1;
	while ((num /= 10) != 0) ++digitsSoFar;
	return digitsSoFar;//返回其参数的数字个数
}
class Widget {//class定义
public:
	Widget();
	~Widget();
	//...
};
template<typename T>
class GraphNode {//template定义
public:
	GraphNode();
	~GraphNode();
	//构造和析构可缩写为 ctor和dtor
};

//初始化（initialization），给与对象初值的过程
//对于用户自定义类型的对象，初始化由构造函数执行，default构造是一个可被调用的不带任何实参者，这样的构造或是没有参数，或是每个参数都有缺省值
class A {
public:
	A();//default构造函数
};
class B {
public:
	explicit B(int x = 0, bool b = true) {};//也是default构造函数
};
class C {
public:
	explicit C(int x);//非default构造
};

//explicit，会阻止执行隐式类型转换（implicit type conversions），但仍可以被用来及逆行显示类型转换（explicit type conversions）
//声明为explicit的构造函数禁止编译器执行非预期（往往也不被期望）的类型转换
//除非有很好的理由允许构造函数被用于隐式类型转换，否则最好声明为explicit
void doSomething(B objB) {};//接受类型B对象的函数
void test_explicit() {
	B objB1;
	doSomething(objB1);//没问题
	B objB2(28);//根据in28构建一个B对象，bool参数缺省为true
	//doSomething(28);//error，int到B之间没有隐式转换
	doSomething(B(28));//没问题，使用B构造，显示转换int为一个B对象
}

//copy构造，以同型对象初始化自我对象
//copy assignment操作符，从另一个同型对象中拷贝其值到自我对象
class Widget1 {
public:
	Widget1() {};//default构造
	Widget1(const Widget1& rhs) {};//copy构造
	Widget1& operator=(const Widget1& rhs) { return *this; };//copy assignment操作符
};
void test_copy() {
	Widget1 w1;//调用default构造
	Widget1 w2(w1);//调用copy构造
	w1 = w2;//调用copy assignment操作符
	//要注意赋值符号，= 语法也可以用来调用copy构造
	Widget1 w3 = w2;
	//而copy构造和copy赋值较容易区分
	//如果一个新对象被定义（例如上面w3）一定是一个构造函数被调用，不可能去选用赋值操作
	//反之，如果没有新对象被定义（如上面w1=w2），就不会有构造被调用，自然选择赋值操作
}
//copy构造很重要，因为它定义一个对象如何以值传递（passed by value）
bool hasAcceptableQuality(Widget1 w) { return true; };//参数w以by value方式传递
void test_pass_by_value() {
	Widget1 aWidget;
	if (hasAcceptableQuality(aWidget))//以值传递，所以会通过copy构造，将aWidget复制到w内；而这样传值并不好，pass-by-reference-to-const比较好
	{
		//...
	}
}

//不明确行为
void test_something() {
	int* p = 0;//p是一个null指针
	cout << *p << endl;//warning，对null指针取值（dereference），会导致不明确行为

	char name[] = "Darla";//name是一个大小为6的数组（结尾有null）
	char c = name[10];//warning，取了一个无效的数组索引
}

//C++没有接口（interface），这与java和.net语言是一个区分
//接口一般就是指函数签名或class的成员函数

//考虑到使用自己代码的人，客户（client），包括使用者可能也是自己，所以，尽量让代码更标准是个好习惯

//命名习惯
//参数名称 lhs（left-hand side)和rhs（right-hand side） 分别代表左手端和右手端，常用于操作符==和*的参数名称
//例如，a*b 等价于 operator*(a,b)
const Widget1 operator*(const Widget1& lhs, const Widget1& rhs);
//在成员函数中，左侧实参是由this指针表现得，所以有时只需要使用rhs
//命名指针时，通常将指向T型对象的指针命名为pt
Widget1* p_global_widget;//见名知意
//命名引用也类似
//命名成员时，m作为前缀，函数以f作为缩写等也是常用习惯

//TR1-Technical report 1，是一份规范，描述加入c++标准库的功能，例如 hash tables、reference-counting smart pointers等，被置于tr1命名空间，tr1又嵌套命名于std内
//boost-提供可以指的开源c++程序库

int main()
{
	std::cout << "Hello World!\n";
}

