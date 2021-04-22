// effective_cpp_09.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <new>

/**
 * 8.定制new和delete
 * Customizing new and delete
 */

using namespace std;

//条款49：了解new-handle的行为
//Understand the behavior of the new-handler
// 
//当operator new无法满足某一内存分配需求时，会抛出异常
//产生异常后会调用一个指定的错误处理函数，我们可以使用set_new_handler自定义该处理函数
namespace yshen {
	typedef void (*new_handler) ();
	new_handler set_new_handler(new_handler p) throw();
}
//参数为将被调用的指定函数，返回的是原本的new-handler函数
//假定使用如下函数作为处理函数
void y_oom() {
	std::cerr << "no memory" << std::endl;
	std::abort();
}
//如果在没有足够空间的情况下， 此函数被调用（如果cerr期间需要动态分配内存，会无限递归直到栈内存溢出）
// 
//C++不支持class专属new-handlers，但可以自己实现operator new并提供set_new_handler
//调用者就可以通过set_new_handler指定，而new操作则确保在分配对象内存过程中用class专属的new-handler替换global new-handler
class Widget {
public:
	static std::new_handler set_new_handler(std::new_handler p) throw();
	static void* operator new(std::size_t size) throw(std::bad_alloc);
private:
	static std::new_handler currentHandler;
};
std::new_handler Widget::currentHandler = 0;//static成员必须在class定义式之外被定义（除非是const整数型）
std::new_handler Widget::set_new_handler(std::new_handler p) throw() {
	std::new_handler old_handler = currentHandler;
	currentHandler = p;
	return old_handler;
}
//operator new应该做以下事情
// 调用标准set_new_handler，告知Widget的错误处理函数，将Widget的new-handler安装为global new-handler
// 调用global operator new，执行实际之内存分配，若分配失败，global operator new调用Widget的new-handler
// 如果global operator new最终无法分配内存，会抛出bad_alloc异常（此情况下，Widget的new必须恢复原本的global new-handler，传播异常）
// 为了确保原本的new-handler总是能够被重新安装回去，Widget将global new-handler视为资源并遵守条款13，用资源管理对象防止泄漏
// 如果global operator new能够分配一个Widget对象所用内存，Widget的new会返回一个指针，指向分配所得
// Widget析构函数会管理global new-handler，自动将原本的new-handler恢复回来
class NewHandlerHolder {
public:
	//资源处理类
	explicit NewHandlerHolder(std::new_handler nh) :handler(nh) {}//取得当前global new-handler
	~NewHandlerHolder() { std::set_new_handler(handler); }//还原
private:
	std::new_handler handler;
	//阻止copying
	NewHandlerHolder(const NewHandlerHolder&) = delete;
	NewHandlerHolder& operator=(const NewHandlerHolder&) = delete;
};
void* Widget::operator new(std::size_t size) throw(std::bad_alloc) {
	NewHandlerHolder h(std::set_new_handler(currentHandler));//安装class专属new-handler，并记录当前全局new-handler
	return ::operator new(size);//分配内存或抛出异常
	//跳出控制流后，自动调用h对象的析构函数，恢复global new-handler
}
//使用时
void test_new_handler() {
	Widget::set_new_handler(y_oom);//设定专属new-handler
	Widget* pw1 = new Widget;//若分配内存失败，则调用y_oom
	//此时handler已经恢复为原来的
	std::string* ps = new std::string;//若分配失败，调用global new-handler
	Widget::set_new_handler(0);//设定专属handler为null
	Widget* pw2 = new Widget;//内存分配失败立刻抛出异常，因为没有专属new-handler函数
}
//这个方案可以通用于所有class，可定义一个mixin风格的base class，用来允许derived classes继承单一特定能力
template<typename T>
class new_handler_supooert {
public:
	static std::new_handler set_new_handler(std::new_handler p) throw();
	static void* operator new(std::size_t size) throw(std::bad_alloc);
private:
	static std::new_handler current_handler;
};
template<typename T>
std::new_handler new_handler_supooert<T>::set_new_handler(std::new_handler p) throw() {
	std::new_handler old_handler = current_handler;
	current_handler = p;
	return old_handler;
}
template<typename T>
void* new_handler_supooert<T>::operator new(std::size_t size) throw(std::bad_alloc) {
	new_handler_supooert h(std::set_new_handler(current_handler));
	return ::operator new(size);
}
template<typename T>
std::new_handler new_handler_supooert<T>::current_handler = 0;
class A :public new_handler_supooert<A> {
	//不必声明set_new_handler或operator new
};
//类型T并没有实际作用，只是用来区分不同的Derived class
//对于每一个T，都会具象化一个base class，每种base class都会拥有一个static成员变量current_handler被子类继承
// 
//早期C++要求在无法分配足够内存时返回null，新一代的new则是抛出bad_alloc异常
// 为了兼容，提供了另一种形式，负责对应传统的返回null行为，例如：Widget* pw = new (std::nothrow) Widget;//分配失败返回0
//


//条款50：了解new和delete的合理替换时机
//Understand when it makes sense to replace new and delete
// 
//可能需要自定义operator new/delete的情况：
// 检测运用上的错误。new所得内存delete时失败，会内存泄漏，多次delete会导致不明确行为；编程错误导致的越界，可自定义new超额分配空间，并在额外空间放置签名，delete时检查
// 强化效能。定制版的new和delete通常可以更快并节约内存
// 收集使用上的统计数据。收集软件如何使用其动态内存
// 
//一个并不完善的例子，但体现了检测越界问题的情况
namespace yshen {
	static const int signature = 0xDEADBEEF;
	typedef unsigned char Byte;
	/*
	void* operator new(std::size_t size) throw(std::bad_alloc) {
		size_t realSize = size + 2 * sizeof(int);//增加大小放入两个signatures
		void* pMem = malloc(realSize);
		if (!pMem) throw bad_alloc();
		//将signatur写入内存前后端
		*(static_cast<int*>(pMem)) = signature;
		*(reinterpret_cast<int*>(static_cast<Byte*>(pMem) + realSize - sizeof(int))) = signature;
		//返回指针，指向第一个签名后面的位置
		return static_cast<Byte*>(pMem) + sizeof(int);
	}
	*/
}
//指针操作中，对齐是很重要的，例如要求特定类型必须放在特定内存地址，可能要求指针地址必须是4倍数，doubles地址必须是8倍数
//malloc是按照这些要求工作的，所以返回一个直接来自malloc的指针是安全的，但上面例子是返回偏移一个int后的指针，就不保证安全
// 
//Boost的Pool程序库就是一个可参考的分配器
// 


//条款51：编写new和delete时需固守常规
//Adhere to convention when writing new and delete
// 
//C++中，即使要求申请0bytes，operator new也得返回一个合法指针（视为1-byte申请）
// 
//operator new应该内含一个无穷循环，并在其中尝试分配内存，如果无法满足内存需求，就该调用handler，也应有处理0bytes申请的能力
// class专属版本应该还能处理“比正确大小更大的申请”
//operator delete应该在收到null指针时不做任何事
// class专属版本应该还能处理“比正确大小更大的申请”
//


//条款52：写了placement new也要写placement delete
//Write placement delete if you write placement new
// 
//一个new表达式 Widget* pw = new Widget;
//共有两个函数被调用，一个是operator new，一个是Widget的default构造函数
// 
//当你声明placement new和placement delete，请确定不要无意识掩盖正常版本
//


/**
 * 9.杂项讨论
 * Miscellany
 */

void* foo = nullptr;

//条款53：不要忽视编译器的警告
//Pay attention to compiler warnings
// 
//其他语言中警告可能相对无害，但C++中有时却会引起严重问题
// 严肃对待编译器发出的警告信息，努力在编译器的最高警告级别下争取无任何警告
// 不要过度依赖编译器报警能力，不同编译器对待同一段代码的态度并不一定相同
//


//条款54：熟悉包括tr1在内的标准程序库
//Familiarize yourself with the standard library,including tr1
// 
//标准库主要内容：
// STL
// iostreams
// 国际化支持
// 数值处理
// 异常阶层体系
// 
//TR1组件：
// 智能指针
// function
// bind
// hashtables
// 正则
// tuples
// array
// mem_fn
// reference_wrapper
// 随机数
// 数学特殊函数
// type traits
// result_of
//


//条款55：熟悉Boost
//Familiarize yourself with Boost
// 
//高质量源码开放跨平台跨编译器的程序库
// https://boost.org
// 
//常用asio等
//


int main()
{
	std::cout << "Hello World!\n";
}

