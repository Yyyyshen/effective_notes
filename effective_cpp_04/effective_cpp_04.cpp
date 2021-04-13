// effective_cpp_04.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>

/**
 * 3.资源管理
 * Resource Management
 */

using namespace std;

//条款13：以对象管理资源
//Use objects to manage resources
// 
//手动管理资源时
class A {};
void test_manage_A() {
	A* pa = new A();
	//...一顿操作
	delete pa;
}
//上面例子中，如果...一系列操作中，出现提前return、抛出异常等情况，跳出了当前作用域，则没有执行到delete语句
//尽管可以通过合理的代码来避免，但实际中转交给其他人维护则不一定能保证
//为确保资源总是被释放，我们需要将资源放进对象内，当控制流离开函数，对象的析构函数会自动释放那些资源（RAII，利用栈+析构特性自动管理资源）
//许多资源动态分配于heap后用于单一区块内，应该在控制流离开区块时释放，可以使用auto_ptr，智能指针（现在一般用shared_ptr）
//智能指针是一种类指针对象，析构函数自动对其所指对象调用delete
void test_auto_ptr() {
	std::auto_ptr<A> pa(new A());
	//...一顿操作
	//跳出区块后经由auto_ptr的析构函数自动删除pa
}
//
//两个关键想法：
//获得资源后立刻放进管理对象内——资源取得时机便是初始化时机（Resource Acquisition Is Initialization；RAII）我们几乎总在获得一笔资源后在同一语句以其初始化管理对象
//管理对象运用析构函数确保资源被释放——不管控制流如何离开区块，对象离开作用域时就会被销毁，其析构函数就会自动调用
//
//智能指针被销毁时会自动删除所指之物，所以一定别让多个auto_ptr同时指向同一个对象
//所以，智能指针使用copy构造和copy assignment操作符时，为了使指针取得资源的唯一拥有权，他们会变成null
void test_auto_ptr_copy() {
	std::auto_ptr<A> pa1(new A());
	std::auto_ptr<A> pa2(pa1);//copy构造，pa1此时设为null
	pa1 = pa2;//copy assignment，pa2此时设为null
}
//由于这个约定，STL容器要求其元素发挥正常复制行为，所以这些容器不能使用auto_ptr
//代替方案使引用计数型智能指针（RCSP）例如shared_ptr
//
//智能指针在析构函数内做的是delete而不是delete[]，就意味着不能使用其管理array（虽然能通过编译）
//


//条款14：在资源管理类中小心copying行为
//Think carefully about copying behavior in resource-managing classes
//
//对于堆内资源，使用条款13的RAII观念较好，而并非所有资源都是heap-based；有时还是需要自己来管理资源
//例如，管理互斥量，自动锁
#include <mutex>
std::mutex g_mutex;
void test_mutex() {
	std::lock_guard<std::mutex> lock(g_mutex);
	//一系列需要加锁操作
	//出作用域之后，自动调用lock析构函数解锁
}
//内部实现
template <class _Mutex>
class y_lock_guard { // class with destructor that unlocks a mutex
public:

	explicit y_lock_guard(_Mutex& _Mtx) : _MyMutex(_Mtx) { // construct and lock
		_MyMutex.lock();
	}

	y_lock_guard(_Mutex& _Mtx, adopt_lock_t) : _MyMutex(_Mtx) {} // construct but don't lock

	~y_lock_guard() noexcept {
		_MyMutex.unlock();
	}
	//去掉copying操作
	y_lock_guard(const y_lock_guard&) = delete;
	y_lock_guard& operator=(const y_lock_guard&) = delete;

private:
	_Mutex& _MyMutex;
};
//对于复制操作，有两种选择：
//禁止复制
// 许多时候，允许RAII对象被复制并不合理
//对底层资源使用引用计数法
// 复制时，被引用数递增，反之递减；直到持有该资源的最后一个对象被销毁


//条款15：在资源管理类中提供对原始资源的访问
//Provide access to raw resources in resource-managing classes
//
//在资源管理中，有了管理类的包装后，有时也需要直接取用其包装的资源本身
//这就需要将RAII class对象转换为所含原始资源，分为显示和隐式转换
//
//显示转换
//auto_ptr和shared_ptr中提供的get成员函数，会返回原始指针
//隐式转换
//智能指针重载了指针取值操作符operator->和operator*，它们会隐式转换为底部原始指针


//条款16：成对使用new和delete时要采取相同形式
//Use the same form in corresponding uses of new and delete
// 
//new时使用[],delete表达式也要使用[]，如果new没有使用，也不能在delete中相应的使用[]
void test_new_and_delete() {
	int* arr = new int[5];
	delete[] arr;
	A* a = new A();
	delete a;
}


//条款17：以独立语句将newed对象置入智能指针
//Store newed objects in smart pointers in standalone statements
//
//假设有这样一个函数
int func() {}
void process_to_A(std::shared_ptr<A> pa, int func) {}
void test_process() {
	process_to_A(std::shared_ptr<A>(new A), func());
}
//上面调用可能的执行顺序是：new A ， 调用func ， 调用shared_ptr构造
//如果func函数发生异常，new A则是资源泄漏
//处理方法很简单，就是分离语句，将newed对象置入智能指针单独列出
void test_process2() {
	std::shared_ptr<A> pa(new A);
	process_to_A(pa, func());
}



int main()
{
	std::cout << "Hello World!\n";
}
