// effective_modern_cpp_05.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <vector>

//
//两个受多因素影响的场景
//

//
//如果参数可拷贝且移动操作开销很低，考虑直接按值传递
// 
class widget {
public:
	void add_name(const std::string& name)
	{
		names.push_back(name);
	}
	void add_name(std::string&& name)
	{
		names.push_back(std::move(name));
	}
private:
	std::vector<std::string> names;
};
// 对于拷贝自己参数到私有容器，为了提高效率，应该拷贝左值，移动右值
// 但需要维护两个同名参数
// 按之前想法，使用通用引用的函数模板
class widget2 {
public:
	template<typename T>
	void add_name(T&& name)
	{
		names.push_back(std::forward<T>(name));
	}
private:
	std::vector<std::string> names;
};
// 但作为模板，会增加复杂度，展开时可能也会实例化多个函数，还要注意传参类型
// 而下面方法，可以兼顾只编写一个函数并且按两个方式处理（左值拷贝，右值移动）
class widget3 {
public:
	void add_name(std::string name)
	{
		names.push_back(std::move(name));
	}
private:
	std::vector<std::string> names;
};
// 看上去name时拷贝的传递进来的对象，move不影响什么
// 在C++98中，无论调用者如何调用，name都是拷贝传递，开销比较大
// 而C++11中，就是左值拷贝，右值移动（有点颠覆，因为之前学的时候，都是推荐用const&方式设计参数的，可能是因为老的书都以C++98为标准？）
void
test_widget3()
{
	widget w;
	std::string name("hello");
	w.add_name(name);//以左值方式调用，调用的函数签名为 void widget::add_name(const std::string&)
	w.add_name(name + "world");//以右值方式调用，签名为 void widget::add_name(std::string&&)，hin优雅
}
// 
//

//
//考虑用emplacement代替insertion
// 
// 容器中，插入函数insert，push_front，push_back添加新元素时
// 一般会按照左值和右值进行重载
void
test_emplacement()
{
	std::vector<std::string> vec;
	vec.push_back("hello");//这个调用下，由const char[6]创建一个std::string临时变量并传递给push_back
	//性能上讲
	// 一个临时对象，通过std::string构造器生成
	// 临时对象是右值，被传递给push_back的右值重载，vector内存中一个副本被创建，又调用一次构造器（移动构造）
	// 临时变量最终被销毁，调用一次析构
	//

	//而以下方式
	vec.emplace_back("world");
	//会把传递的参数（无论是否是string）直接传递给vector内部构造器，没有临时变量生成
	// 使用了完美转发，只要没有遇到forward限制，就可以传递任何参数
	//

	//********************************************************************************************
	//事实上，从我vs2019默认的C++14标准跟进源码去看，push_back内部其实也是默认调用了emplace_back的
	// （编译器越来越智能，标准库越来越完善，那么学习这些effective条例是否应该用新的眼光）
	//********************************************************************************************
}
// 
//

int main()
{
	std::cout << "Hello World!\n";
}
