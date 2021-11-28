// effective_modern_cpp_02.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <vector>
#include <type_traits>

//
//右值引用、移动语义、完美转发
//

//
//牢记
// 参数永远是左值
class widget {};
void
f(widget&& w)
{
	//即使类型是右值引用，参数w也是左值
}
// 
//

//
//理解std::move和std::forward
// 运行期间，它们不做任何事情，不产生任何可执行代码
// 仅仅是执行转换函数（模板）
void
test_move()
{
	std::move(widget());
	//std::forward();
	/* 源码部分

	template <class _Ty>
	_NODISCARD constexpr _Ty&& forward(remove_reference_t<_Ty>&& _Arg) noexcept { // forward an rvalue as an rvalue
		static_assert(!is_lvalue_reference_v<_Ty>, "bad forward call");
		return static_cast<_Ty&&>(_Arg);
	}

	// FUNCTION TEMPLATE move
	template <class _Ty>
	_NODISCARD constexpr remove_reference_t<_Ty>&& move(_Ty&& _Arg) noexcept { // forward _Arg as movable
		return static_cast<remove_reference_t<_Ty>&&>(_Arg);
	}

	template <class _Ty>
	using remove_reference_t = typename remove_reference<_Ty>::type;

	*/
}
// std::move接收一个通用引用，返回一个指向同对象的引用
// 函数返回类型&&部分表明返回一个右值引用
// 如果T是一个左值引用，T&&会是一个左值引用，为了避免这个情况，类型萃取器remove_reference应用到T，确保&&应用于一个不是引用的类型上
// 保证了返回的是右值的引用。所以，函数作用就是把参数转为右值
// 并不存在函数名move所谓的移动操作，只是告诉编译期，这个对象容易被移动
// 
// 看一个例子
class TestMove final {
public:
	explicit TestMove(const std::string text)
		:value(std::move(text))
	{
		//vs2019直接给出警告，不要对常量使用std::move
		//这里代码text不是移动到value，而是复制过去的
		/*
		string的拷贝构造和移动构造类似如下代码

		class string{
		public:
			string(const string& rhs);
			string(string&& rhs);
		}

		*/
		//移动构造接收的是一个右值引用(non-const)
		//一个const std::string的右值会被拷贝函数接收，即使text被转换成了右值
	}
private:
	std::string value;
};
// 总结
//	在希望移动对象时，通常代表修改该对象，语言不允许常量对象被传递给可修改它们的函数（移动构造）
//	move不移动任何东西，不保证执行转换的对象可以被移动；只能确定的是，会得到一个右值
// 
// forward 类似于move
// 区别在于
//	move无条件转换参数为右值
//	forward只在满足一定条件时才转换
//	forward只是传递一个对象到另一个函数，保留它原有的左值或右值属性（也就是说，条件是当它的参数绑定右值时，才转换为右值）
//

//
//区分通用引用和右值引用
// 
// 即T&&并非就是右值引用（之前老是习惯性的这么想）
// 
// 从一个例子开始
void
test_rvalue_ref()
{
	widget&& w1 = widget();			//w1为右值
	auto&& w2 = w1;					//w2为widget&；auto&&为通用引用，之后类型推导出w2
	//decltype(auto) w3 = w1;		//直接err，无法将右值引用绑定到左值
}
template<typename T>
void f(std::vector<T>&& param) {}	//右值引用
template<typename T>
void f(T&& param) {}				//不是右值引用，是通用引用
// 
// T&& 有两种含义
//	其一就是右值引用
//	其二是，它既可以是右值引用，也可以是左值引用T&
//	可以绑定在左值或右值上
//	也可以绑定在const和non-const上，volatile和non-volatile上，几乎可以绑定在任何东西
// 称为通用引用或转发引用
// 
// 
//总结
// T&& 称为通用引用
// static_cast<remove_reference_t<T>&&>(arg) 才确保为右值引用
// 

//
//需要记住的事
// 不要对通用引用参数的函数进行重载
// 完美转发构造函数是糟糕的实现
// 
//例子，通用引用参数的函数重载代替方案
//
template<typename T>
void func_impl(T&& name, std::false_type)
{
	//真正的逻辑
}
//封装一层
template<typename T>
void func(T&& name)
{
	func_impl(std::forward<T>(name), std::is_integral<std::remove_reference_t<T>>());
	//注意remove_reference_t的使用场景；如果不使用其将T转为右值，即使传递了整型，例如int&
}
//这样就可以重载其他版本
void func_impl(int num, std::true_type)
{
	//int版本
}
// 
//

//
//理解引用折叠
template<typename T>
void test_uni_ref(T&& param) {}
void
test_something()
{
	widget w;
	test_uni_ref(w);			//以左值传递，T被推导为widget&
	test_uni_ref(widget());		//以右值传递，T推导为widget
}
// 其中，以左值传递时，实际上调用的函数形如 void test_uni_ref(widget& && param)
// 而引用的引用在C++中是非法的
// 这种情况下，编译器进行引用折叠，最终的函数签名其实是 void test_uni_ref(widget&)
// 
// 所以，前面 auto&& 对另一个引用进行推导得到的是左值引用时，也是因为推导成了 widget& && 之后，进行了引用折叠
//

//
//理智看待移动语义
// 认为移动开销都很小是错误的（string中小长度字符串存储在缓冲区，并没有在堆内存，移动存储的字符串并不比复制快）
// 类不提供移动操作时，写了move也会变成复制（move只是尝试move）
// 进行移动的上下文要求移动操作不会抛异常
//

int main()
{
	std::cout << "Hello World!\n";
}
