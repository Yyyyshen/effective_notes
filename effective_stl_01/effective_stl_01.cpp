// effective_stl_01.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <vector>
#include <list>

/**
 * 参考《Effective STL》
 */

using namespace std;

/**
 * 1容器
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


//
//


int main()
{
	std::cout << "Hello World!\n";
}
