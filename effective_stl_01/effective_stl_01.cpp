// effective_stl_01.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <algorithm>
#include <set>

/**
 * 参考《Effective STL》
 */

using namespace std;

/**
 * 1 容器
 */

void* foo = nullptr;

//第1条：慎重选择容器类型
// 
//标准STL序列容器：vector、string、deque、list
//标准STL关联容器：set、map、multiset、multimap
//非标准序列容器：slist、rope
//非标准关联容器：hash_set、hash_map、hash_multiset、hash_multimap
//标准非STL容器：数组、bitset、valarray、stack、queue、priority_queue
// 
//在vector、deque和list中做选择
// vector使默认应使用的类型
// 需要频繁在序列中插入和删除，应使用list
// 当大多数插入删除操作在序列头尾，考虑使用deque
// 
//另一种分类方式
// 连续内存容器，也就是基于数组，将元素存放在一块或多块内存，每块内存多个元素，插入删除时，其他元素需要移动，影响效率和异常安全
// （标准，vector、string、deque；非标准，rope）
// 基于节点的容器，每个动态分配的内存块中只存放一个元素，元素插入删除只影响指向节点的指针，不需要元素移动
// （链表容器，list、slist；所有标准关联容器，通常实现方式为平衡树）
// 
//考虑条件
// 是否需要任意位置插入新元素，需要则选择序列容器，关联容器不行
// 是否关心元素排序，不关心则可以使用哈希容器
// 是否必须为C++标准，如果是则排除哈希容器、slist、rope
// 需要什么类型迭代器。随机访问则选择vector、deque和string，双向迭代器则要避免slist及哈希
// 发生元素插入删除时，效率是否关键，是的话需要避免连续内存容器
// 容器中数据布局是否需要与C兼容，需要兼容则只能选vector
// 元素查找速度是否关键，是则考虑哈希容器、排序的vector和标准关联容器
// 是否介意容器内使用了引用计数，如果是则避免使用string（可用vector<char>代替）、rope
// 插入删除操作在失败时是否需要回滚，需要可选择list，提供了事务语义
// 是否介意插入删除时，迭代器、指针或引用会变为无效。是则应选择基于节点的容器，连续内存容器的插入删除操作一般会导致其失效
// 是否介意使用swap时，迭代器、指针或引用无效，介意则应避免使用string
// 当插入操作仅在容器末尾发生时，deque是唯一迭代器可能变为无效而指针和引用不会变为无效的STL标准容器
//


//第2条：不要试图编写独立于容器类型的代码
// 
//编写适用于所有类型容器的代码是不太可能的，他们有不同的特征和优缺点
// 
//考虑到有时不可避免的会在开发中改变容器类型，可以使用类型定义来避免改动过多的代码
class widget {};
typedef vector<widget> widget_container;
void test_widget() {
	widget_container cw;
	widget best_widget;
	//widget_container::iterator it = find(cw.begin(), cw.end(), best_widget);
}
//或者封装进一个类
class widget_list {
private:
	typedef list<widget> widget_container;
	typedef widget_container::iterator c_itor;
private:
	widget_container widgets;
public:
	//...
};
//


//第3条：确保容器中的对象拷贝正确而高效
// 
//容器中保存的对象并不是提供给容器的对象，当通过insert等操作插入对象时，存入的是其拷贝
//（新的C++标准已经支持了移动语义，也就是基于右值引用的移动构造函数，书里已经比较落后）
//拷贝时依赖的是copying成员函数，拷贝构造（copy ctor）和赋值操作符（copy assignment operator）
// 如果没有自己声明，编译器会自动生成他们（比如，内置类型实现是按位拷贝）
//
//之前了解到在继承关系下拷贝时，可能会发生slicing（创建存放基类对象的容器后加入派生类对象，会导致子类特性丢失）
// 
//使拷贝动作高效、正确，并避免剥离，简单的办法是使容器包含指针而不是对象（智能指针更好）
//


//第4条：调用empty而不是检查size()是否为0
// 
//对于所有标准容器，empty使常数时间操作；而对于一些list实现，size消耗线性时间
// 由于list的链接操作，size操作一般是遍历一遍来计数的
//


//第5条：区间成员函数优先于与之对应的单元素成员函数
// 
//两个vector，要使v1内容与v2后半部分内容相同，需要怎样的赋值操作
// 最佳答案 v1.assign(v2.begin()+v2.size()/2,v2.end())
// 
//也许copy也能采用一条语句完成这个任务，但内部也是使用了循环的
//
//选择区间成员函数的充分理由
// 代码更简洁
// 清楚的表达意图
// 效率更高
//


//第6条；当心C++编译器最烦人的分析机制
//
//从一个存有整数的文件中读取数据存放到list
void test_read_file() {
	ifstream data_file("ints.dat");
	list<int> data_list(istream_iterator<int>(data_file), istream_iterator<int>());
}
//这种做法看起来是可以的，能通过编译，但运行时什么都不会做
// 没有读取任何数据，也不会创建list
// 第二条语句并没有声明一个list，也没有调用构造函数
//具体做了什么需要分析一哈
int f(double d);//声明了一个返回int和带double参数的函数
int f(double(d));//意义同上，d两边的括号会被忽略
int f(double);//意义相同，省略了参数名称
int g(double(*pf)());//以函数指针为参数
int g(double pf());//同上，pf为隐式指针
int g(double());//同上，pf被省略
//所以，上面第二句代码的含义是，声明了一个函数叫data_list
// 返回值为list<int>
// 第一个参数为data_file，两边括号被忽略，类型为istream_iterator<int>
// 第二个参数为一个函数指针，忽略了参数名，该函数指针返回一个istream_iterator<int>
// （这在vs这样的ide中就比较容易发现，因为颜色的区分就能看出是一个函数声明）
//


//第7条：如果容器中包含了通过new操作创建的指针，切记在容器对象析构前将指针delete掉
//
void cause_memory_leak() {
	vector<widget*> vec_pw;
	vec_pw.push_back(new widget);
	//...
}//跳出作用域，vector析构，发生widget泄漏
//在设定上，通过new创建的对象只有编写者才直到是否该被释放
// 最简单的做法，一般是在析构前遍历delete指针
// 还可以使用for_each，编写仿函数操作
template<typename T>
struct delete_obj : public unary_function<const T*, void> {
	void operator()(const T* ptr) const {
		delete ptr;
	}
};
void deal_memory_leak() {
	vector<widget*> vec_pw;
	//简单处理
	for (auto i = vec_pw.begin(); i != vec_pw.end(); ++i)
		delete* i;
	//for_each
	for_each(vec_pw.begin(), vec_pw.end(), delete_obj<widget*>());
}
//但仍然存在异常安全和类型安全的问题
//最简单的是用智能指针代替指针
//


//第8条：切勿创建包含auto_ptr的容器对象
// （C++11已经弃用，了解理论即可，没弃用之前也不推荐使用）
// 
//可能导致排序等操作中，元素内容会被改变
//


//第9条：慎重选择删除元素的方法
// 
//不同的容器类型，在删除元素时采取不同办法
// 对于连续内存（vector、deque或string）容器，最好的办法是erase-remove惯用方法
void test_delete() {
	vector<int> vec_i;
	//此remove位于algorithm，不会删除元素，只是把符合条件的元素移动到末尾，并返回指向移动后元素起始位置的迭代器
	//再利用erase方法删除指定范围元素，这样可以减少每次删除元素时的位移操作
	vec_i.erase(remove(vec_i.begin(), vec_i.end(), 1), vec_i.end());
	//对于list，则可以使用成员函数remove
	list<int> list_i;
	list_i.remove(1);
}
// 对于标准关联容器（set、multiset、map、multimap），任何remove操作都是错误的，可能会覆盖容器值甚至破坏容器
void test_delete2() {
	set<int> set_i;
	//正确做法是使用erase
	set_i.erase(1);
}
//满足特定条件进行删除操作时
// 对于vector、string、deque，使用erase-remove_if
// 对于list，使用list::remove_if
// 标准关联容器，使用remove_copy_if和swap，或写循环来遍历元素，并且erase时，要进行后缀递增
bool bad_value(int) { return true; }
void test_delete3() {
	set<int> set_i;
	for (auto iter = set_i.begin(); iter != set_i.end();/* 处理在后面进行 */) {
		if (bad_value(*iter)) set_i.erase(iter++);//使用后置++，确保删除当前迭代器，而后iter为下一个有效值
		else ++iter;//不需删除则正常递增
	}
}
//时刻记得评估这些操作的效率和是否会产生不确定行为
//


//第10条：了解分配子（allocator）的约定和限制
// 
//如果编写自定义内存管理器，需要记住
// 你的allocator是一个模板，参数T代表你为它分配内存的对象类型
// 提供类型定义pointer和reference，但是始终让pointer为T*，reference为T&
// 千万别让你的allocator拥有随对象而不同的状态。通常，分配子不该有非静态的数据成员
// 传给分配子的allocate成员函数的是那些要求内存的对象的个数，而不是所需字节数；同时，这些函数返回T*指针，即使尚未有T对象被构造出来
// 一定要提供嵌套的rebind模板，因为标准容器依赖该模板
//


//第11条：理解自定义分配子的合理用法
// 
//只要遵守了同一类型的分配子必须是等价的这一限制要求
// 当使用自定义分配子来控制通用的内存管理策略
// 或者聚集成员关系的时候
// 或着在使用共享内存和其他特殊堆的时候
// 就不会陷入麻烦
//


//第12条：切勿对STL容器的线程安全性有不切实际的依赖
// 
//最多只能期望
// 多个线程读是安全的
// 多个线程对不同容器的写是安全的
//实现线程安全的方案
// 对容器成员函数的每次调用，都锁住容器直至结束
// 在容器返回的每个迭代器生存周期结束前，都锁住容器（begin或end等）
// 对作用于容器的每个算法，都锁住该容器，直到算法结束（算法无法知道他们所操作的容器，所以不一定有意义）
//锁的实现方案
// 构造函数中获得一个互斥体，析构中释放，尽可能减少忘记解锁的可能
template<typename container>
class y_lock { //概念版
public:
	y_lock(const container& c) : c_(c) {
		/* 获取锁 */
		//get_mutex();
	}
	~y_lock() {
		/* 释放锁 */
		//release_mutex();
	}
private:
	const container& c_;
};
//不能指望STL，安心手工同步
//


int main()
{
	std::cout << "Hello World!\n";
}
