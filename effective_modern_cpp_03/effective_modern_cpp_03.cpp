// effective_modern_cpp_03.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <vector>

//
//lambda
//

//
//避免使用默认捕获模式
// 按引用捕获可能带来悬空引用问题
// 按值引用可能会诱导你以为能解决悬空引用问题（按值捕获指针）
// 捕获成员函数可以使用copy to closure的方式
// void widget::func() const
// {
//      [data = m_data](){};
// }
//

//
//使用初始化捕获移动对象到闭包中
// 如果有一个只能被移动的对象，例如unique_ptr或future要进入闭包
// C++14支持对象移动到闭包，被称为初始化捕获
// 初始化捕获可以让你指定从lambda生成的闭包类中的数据成员名称和初始化该成员的表达式
// 
class widget {};
void
test_init_capture()
{
	//移动捕获
	auto pw = std::make_unique<widget>();
	auto func = [pw = std::move(pw)]		//等号左侧表示闭包内的数据成员名称，右侧为初始化表达式
	{										//而左右两侧pw并不是同一个，而是意为在闭包中创建一个成员pw，并通过move上方局部pw初始化该成员
		return;
	};
}
// C++11有近似的实现方案模拟该方法
#include <functional>
void
test_capture_cpp11()
{
	std::vector<int>data;
	auto func = std::bind(
		[](const std::vector<int>& data)
		{
			//use data
		},
		std::move(data));
	//std::bind生成函数对象，第一个参数为可调用对象，后续的表示传递给该对象的值
	//（这在之前看boost样例代码时c++11和c++14的区别中经常碰到，算是弄懂了原理）
}
// 
//

//
//在使用auto&&形参的lambda表达式中
// 对于std::forward的auto&&形参使用decltype
void
test_forward()
{
	auto f = [](auto&& x)
	{
		std::forward<decltype(x)>(x);
	};
}
// 
//

//
//优先考虑lambda而非std::bind
//

int main()
{
	std::cout << "Hello World!\n";
}
