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
//
//


int main()
{
	std::cout << "Hello World!\n";
}
