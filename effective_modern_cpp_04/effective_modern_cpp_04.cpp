// effective_modern_cpp_04.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <future>
#include <thread>

//
//并发
//

//
//优先基于任务编程，而不是基于线程
// 
int do_async_work() {}
void
test_async_task()
{
	//异步执行函数的两个方法

	//1，线程
	std::thread t(do_async_work);

	//2，任务
	auto fu = std::async(do_async_work);
	//这种方式中，函数对象作为任务传递给std::async
	//通常优于线程方式
	// 首先，有返回值的方法，使用thread无法获取，而future提供get获取结果
	// 如果有异常，get函数可以提供抛出异常的访问，如果基于thread，则直接终止
	//
}
// 
// 基于线程和基于任务本质区别在于抽象层次高低
// 并发中
//	硬件线程是真实执行计算的线程，现代计算机体系结构为每个CPU核心提供一个或多个硬件线程
//	软件线程（系统线程）是操作系统管理的在硬件线程上执行的线程，通常可以存在更多数量，因为软件线程被阻塞时，操作系统可以调度其他线程提供吞吐量
//	std::threads是C++执行过程的对象，并作为软件线程的handle，也就是句柄，存在多种状态
//		null，表示空句柄，处于默认构造，没有函数执行，不对应任何软件线程
//		moved from，对应软件进程开始执行
//		joined，连接唤醒与被唤醒的两个线程
//		detached，将两个连接线程分离
// 软件线程是有限的，如果开发者创建了大于其支持数量的线程，即使在noexcept的函数中，依然会抛出system_error异常
// 使用async将使用thread时需要处理的很多线程管理职责交给了C++标准库开发者，减少错误情况
//

//
//确保在异步是必须时，才指定任务的启动为std::launch::async
// 
// std::launch::deferred意味着目标函数仅在调用get或wait时才执行
// 如果不指定，默认状态下，是两种lauch策略共存的
//

//
//make std::threads unjoinable on all paths
// 在所有路径上保证thread最终是unjoinable
//

//
//关注不同线程句柄的析构行为
// future的正常析构行为就是销毁future本身的成员数据
// 最后一个引用std::async创建共享状态的future析构会在任务结束前block
//

//
//对于一次性事件通信考虑使用无返回futures
//

//
//并发中的单一基础变量使用std::atomic，而不是互斥量；特定内存使用volatile（C++中volatile其实是不具有什么并发性质的）
//

int main()
{
	std::cout << "Hello World!\n";
}
