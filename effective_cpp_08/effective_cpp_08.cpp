// effective_cpp_08.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>

/**
 * 7.模板与泛型编程
 * Templates and Generic Programming
 */

using namespace std;

//条款41：了解隐式接口和编译期多态
//Understand implicit interfaces and compile-time polymorphism
// 
//当把只支持一种类型的实现泛化为template时
// 类型T所使用的功能应当是传入类型必须支持的隐式接口
// 使这些template具象化的行为发生在编译期，以不同template参数具象化function templates会导致调用不同的函数，这就是所谓的编译期多态
// 
//总的来说就是template与class的区别
//


//条款42：了解typename的双重意义
//Understand the two meanings of typename
// 
//template声明中，class和typename
// 意义相同，只是习惯不同
// typename可以暗示该类型不一定非要是一个class
// 
//C++也并不总是把class和typename视为等价，有时一定需要用typename
// 例如STL设计中，很多定义在template中的typedefs
template<typename Container>
void print_something(const Container& container) {
	//Container::iterator* iter;//编译器不明白
	typename Container::iterator* iter;
}
//这里的Container::iterator是我们知道容器中会有相应的类型定义，但编译器并不知道，又或者这个iterator刚好与一个全局变量名，就会出问题
//所以这种情况，需要主动告诉编译器，它是一个类型，使用关键字typename即可
//一般性规则：
// 任何时候当你想要在template中指涉一个嵌套从属类型名称，就必须在它前面使用关键字typename修饰
//例外：
// 在继承时父类列表中不加修饰
// 在成员初值列作为基类初始化时不加修饰
// template<typename T>
// class Derived: public Base<T>::Nested { //base class list，不适用typename
// public:
//		explicit Derived(int x): Base<T>::Nested(x) { //mem init list，不适用typename
//			typename Base<T>::Nested temp;//不属于以上两种情况，使用typename
//		}
// }
// 
//typedef typename 连用很常见于STL
//


//条款43：学习处理模板化基类内的名称
//Know how to access names in templatized base classes
// 
//可以在derived class template内通过 this-> 指涉base class templates内的成员名称，或明确写出 base class资格修饰符 来调用（using或者Base<T>::func()）
//


//条款44：将与参数无关的代码抽离template
//Factor parameter-independent code out of templates
// 
//使用template会导致代码膨胀，因为目标码中会产生不同类型的重复代码
//所以任何template代码都不该与某个造成膨胀的template参数产生依赖关系
//因非类型模板参数造成的代码膨胀通常可以避免，做法是以函数参数或成员变量去代替
//因类型参数造成的代码膨胀往往可以降低，做法是让带有完全相同二进制表述的具象类型共享实现码
// （例如int和long在一些平台上就是相同的，还有指针在大多数平台有相同的二进制表示）
//


//条款45：运用成员函数模板接受所有兼容类型
//Use member function templates to accept "all compatible types"
// 
//如果声明member templates用于泛化copy构造或泛化assignment操作，还是需要声明正常的copy构造和copy assignment
//


//条款46：需要类型转换时请为模板定义非成员函数
//Define non-member functions inside template when type conversions are desired
// 
//当编写一个class template，而他所提供的“与此tmeplate相关的”函数支持“所有参数之隐式类型转换”时
// 请将那些函数定义为class template内部的friend函数
//


//条款47：请使用traits classes表现类型信息
//Use traits classes for information about types
// 
//traits class使类型相关信息在编译器可用，以template及templates特化完成
//整合重载计数后，traits classes有可能在编译期对类型执行if else测试
// 
//主要应用为STL中判断迭代器类型
//


//条款48:认识template元编程
//Be aware of template metaprogramming
// 
//TMP模板元编程可将工作由运行期移往编译期，因而得以实现早期错误侦测和更高的执行效率
//TMP可被用来生成“基于政策选择组合”的定制代码，也可用来避免生成对某些特殊类型并不适合的代码
//


//本节很多内容都在学习STL中体会过


int main()
{
	std::cout << "Hello World!\n";
}
