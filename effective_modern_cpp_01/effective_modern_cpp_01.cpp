// effective_modern_cpp_01.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <vector>

//
//《effective modern c++》学习
//

//
//类型推导
void foo(int, double) {}
template<typename T>
void f1(T param) {}
template<typename T>
void f2(T& param) {}
void
test_type()
{
	f1(foo);//param推导为函数指针，类型为void(*)(int,double)
	f2(foo);//推导为函数引用
	//与数组退化一样，函数也会退化为指针

	//f1({ 1,2,3 });//err；对于模板类型无法推导std::initializer

	//decltype
	int x = 0;
	using type1 = decltype(x);//推导为int
	using type2 = decltype((x));//(x)作为一个表达式，是一个左值，推导为int&
}
// 
//decltype和auto
template<typename Container, typename Index>
auto auth_access(Container& c, Index i) -> decltype(c[i])
{
	return c[i];
}
//auto不进行推导工作，只是类似一个类型占位符，指示使用后置返回类型语法-> ; （c++14可忽略后置）
//更好的写法是 auto 改为组合使用 decltype(auto)
//另外，最好还支持右值，避免不必要的拷贝
template<typename Container, typename Index>
decltype(auto) auth_access(Container&& c, Index i)
{
	return std::forward<Container>(c)[i]; //使用forward完美转发
}
// 
// 可视IDE可以直接查看自动推导类型信息，善于利用
// 
// 
//尽量使用auto
void
test_auto()
{
	int x;//潜在未初始化变量
	auto x1 = 0;//必须初始化

	//C++14中，lambda表达式形参也可以用auto
	auto lam_func = [](const auto& p1) { return; };

	//节约复杂类型编码时间，和一些跨平台类型区分时间
}
// 
//

//
//初始化
// 
//区别()和{}初始化
class widget {};
void
test_init()
{
	double x = 1.5, y = 1.2, z = 1.0;
	//int sum1{ x + y + z };//编译期错误，不能用double初始化int
	int sum2(x + y + z);	//可以，窄化为int
	int sum3 = x + y + z;	//同上不进行窄化检查

	//调用没有参数的构造函数
	widget w1;
	widget w2();//不明确行为，会被视为一个函数w2，返回类型为widget
	widget w3{};//调用无参构造

	//对于容器
	std::vector<int> vec1(10, 20);//表示构造一个10个元素的容器，每个元素值为20
	std::vector<int> vec2{ 10,20 };//表示构造一个2个元素的容器，值分别       为10和20
}
// 
//

//
//需要表示空指针时，优先使用nullptr代替0和NULL
//

//
//优先使用别名声明代替typedefs
// 
void
test_using()
{
	//typedef void(*fp)(int, const std::string&);
	using fp = void(*)(int, const std::string&);//更容易理解
	//在模板中，using可以模板化，而typedef不能
}
// 
//

//
//优先考虑class限制域的枚举类型
// 
enum class WEEK : std::uint8_t	//可指定基础（默认int）
{
	Mon, Tue, Wed
};
// 避免命名空间污染，防止不经意使用隐式转换
//

//
//显式delete不需要或禁止调用的函数 而非在private中使用未定义的声明
// 
// 且不限于成员函数；非成员函数和模板实例也可以
void some(int num) {}
void some(char) = delete;//拒绝char，类似explicit避免隐式转换
// 
//

//
//使用overrid关键字
//

//
//优先考虑const_iterator代替普通迭代器
// 
// 在只读的操作中，使用const的迭代器
// 
//优先考虑非成员函数版本的begin、end等代替同名成员函数（感觉并不方便）
//

//
//使用noexcept标示不抛异常的函数
// 
// 优化编译，避免不必要的栈展开
//

//
//尽可能使用constexpr
// 
void
test_constexpr()
{
	int num;
	const auto num1 = num;	//可以，const表示不可更改，相当于复制num的值
	//constexpr auto num2 = num;//err，num在编译期不可知
}
// 
//

//
//若const成员函数改变了mutable的成员，应在内部保证线程安全
// 
// 因为在外界来看，const成员函数只读，不需要处理同步问题
// 
// （处理同步若只有单一基础变量，尽量使用原子变量，比互斥锁性能更好）
//

//
//理解特殊成员函数的自动生成
// 
// 默认的构造、析构、拷贝构造、拷贝赋值、移动构造、移动赋值
// 
//记得显式用default声明（即使编译期本身就会自动生成，也应明确意图）
//

// （大部分内容其实都是重复的了，过一遍就好）
// 
//智能指针
//	优先考虑智能指针代替new
//

//
//pImpl法
// 类中数据成员替换为指向其的指针（智能指针）
//

int main()
{
	std::cout << "Hello World!\n";
}
