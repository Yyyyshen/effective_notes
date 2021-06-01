// effective_stl_02.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <vector>

/**
 * 2 vector和string
 */

using namespace std;

//第13条：vector和string优先于动态分配的数组
// 
//当使用new来动态分配内存，意味着需要
// 确保内存被delete，否则是资源泄漏
// 确保使用了正确形式的delete，例如分配数组须使用delete[]
// 确保只delete了一次，多次调用的结果同样是不确定的
// 
//每当需要动态分配数组时，都可以或应该考虑用vector和string来代替（例如vector<char>/string代替char[]等）
// 他们自己管理内存
// 适用于STL序列容器的算法都可以使用
// 
//大部分情况，使用string代替char数组是比较合理的，除非在多线程中使用了引用计数的string
// 由避免内存分配和字符拷贝所节省下来的时间还比不上花在背后同步控制上的时间
// 如果发生了这个情况，可以选择三种方式
// 首先，检查库的实现，看看是否有可能禁止引用计数
// 其次，寻找或开发另一个不使用引用计数的string实现（或部分实现）
// 第三，考虑使用vector<char>而不是string
//


//第14条：使用reserve来避免不必要的重新分配
// 
//STL容器很方便的一点就是会自动增长来容纳数据，每当需要更多空间，会进行类似realloc的如下操作
// 分配一块大小为当前容器某个倍数的新内存（一般在vector和string中是2倍）
// 把容器的所有元素从旧内存复制到新内存中
// 析构旧内存中的对象
// 释放旧内存
//STL中有关容量和大小的函数以及区别
// size()返回容器中现有多少个元素
// capacity()返回容器已经分配的内存可以容纳多少个元素
// resize()强迫容器改变到包含n个元素的状态
// reserve()强迫容器把容量变为至少是n（如果n比当前容量小，则vector忽略该调用，string则可能把自己容量减为size和n中的最大值）
// 再具体些
//  resize既分配了空间，也创建了对象，可以通过下标访问
//  reserve只修改capacity大小，不修改size大小，resize既修改capacity大小，也修改size大小
//  reserve是容器预留空间，但并不真正创建元素对象，在创建对象之前，不能引用容器内的元素，因此当加入新的元素时，需要用push_back()/insert()函数
//  resize是改变容器的大小，并且创建对象，因此，调用这个函数之后，就可以引用容器内的对象了，因此当加入新的元素时，用operator[]操作符，或者用迭代器来引用元素对象
//  两个函数的形式是有区别的，reserve函数之后一个参数，即需要预留的容器的空间
//  resize函数可以有两个参数，第一个参数是容器新的大小，第二个参数是要加入容器中的新元素，如果这个参数被省略，那么就调用元素对象的默认构造函数
// 
//所以，如果想创建一个包含大量元素的vector，可以先调用一次reserve，直接分配足够多的空间。这样就避免了内部机制一次次的扩容
//


//第15条：注意string实现的多样性
// 
//对于string对象的大小（size）
// 在一些实现中，sizeof(string)和char*指针大小相同（并不常见）
// 还有一些实现，每个string大小是前者七倍
//理解string实现，可能会存储哪些数据
// 字符串大小，size
// 用于存储该字符串字符内存的容量，capacity
// 字符串的值，即构成该字符串的字符
// alloctor的一份拷贝
// 对值的引用计数
// 
//实现A
// allocator
// size
// capacity
// pointer 指向动态分配内存，包含引用计数和具体的值  -> | ref_count | value... |
//实现B
// pointer 指向一个结构，包含 size
//							  capacity
//							  ref_cnt
//							  pointer 指向一块动态分配的内存，保存具体字符值value
//实现C
// pointer 指向所有需要的内容 -> | size | capacity | ref_cnt | value... |
//实现D
// 每个string内部包含一块内存，最大容纳15个字符，小的字符串可以完整直接存放于对象中
// 当容量超过15，该内存起始部分被当作一个指向动态分配内存的指针，指向存放的值
// allocator				 allocator
// value					 pointer (| unused buffer space ) -> | value... |
// size						 size
// capacity					 capacity
// (capacity<=15)			 (capacity>15)
//


//第16条：了解如何把vector和string数据传给旧的API
//（C++11标准为vector和string新增一个名为data的无参函数，返回底层成员指针，兼容旧API）
//  
//很多旧的C API在使用，可能接受数组或char*指针，这其实很容易用容器兼容
// 例如有一个vector，而需要一个指向vec中数据的指针，把数据当作数组看待，则可以直接使用&vec[0]这样的格式即可
// 对于string，则是使用str.c_str()来返回一个char*
void do_something(const int* p_int, size_t num_int) {}
void test_old_api() {
	vector<int> vec_i;
	if (!vec_i.empty())//做判空，放置size为0时产生空指针异常
		do_something(&vec_i[0], vec_i.size());
	//注意，这与vec.begin()并不相同，如果使用了迭代器，等价的应该是 &*vec.begin()
}
//得到容器中数据指针的方式对于vector是适用的，但string不行
// string数据不一定连续
// string内部表示不一定以空字符结尾
//
void do_something(const char* p_str) {}
void test_old_api2() {
	string str;
	do_something(str.c_str());//c_str产生的指针并不一定直接指向字符串数据的内部表示，可能是一个不可修改的拷贝，并且进行了格式化以适配C API要求
}
//


//第17条：使用“swap技巧”除去多余的容量
// 
//当使用一个容器包含了大量元素，并筛选出了部分元素时，为避免其继续占用大量内存，应当缩减到当前需要的数量
//通常称为shrink to fit。有一种利用swap方法的技巧，形式如下
// vector<Contestant>(contestants).swap(contestants);
// vector<Contestant>(contestants)创建一个临时向量，是由vector的拷贝构造函数完成的一个复制品
// 而拷贝构造函数只为所拷贝的元素分配所需要的内存，所以这个临时vector没有多余的容量
// 之后，将临时的vector与contestants中数据进行swap操作
// 这样一来，contestants便具有了被去除之后的容量，即临时vector的容量
// 而临时vector容量变成了之前contestants臃肿的容量
// 到语句结尾时，临时vector被析构，从而释放了先前被contestants所占用的内存
//同样的技巧对于string也适用
// string(str).swap(str);
//另外，swap技巧同样可以用来清除一个容器，并让容器变为最小值，只要用一个默认构造创建的临时对象做交换即可
// vector<Contestant>().swap(vec); //清除vec
// string().swap(str); //清除str
// swap时，两个容器不仅内容被交换，同时，迭代器、指针和引用也将被交换（string除外）
//


//第18条：避免使用vector<bool>
// 
//作为STL容器，vector<bool>有两点不对
// 不是STL容器
// 并不存储bool
// 
//一个对象要称为STL容器，必须满足C++标准，其中一个条件是
// 如果用operator[]取得了Container<T>中的一个T对象
// 那么你可以通过取它的地址得到一个指向该对象的指针
// 则下面代码必须可以被编译
void test_vector_bool() {
	vector<bool> vec_b;
	//bool* pb = &vec_b[0]; //error
}
//vector<bool>是一个假容器，并不真的存储bool
// 为了节省空间，存储的是bool的紧凑表示，每个仅占用1个二进制位
// 一个8位字节可以容纳8个bool
// 在内部，vector<bool>使用了类似与位域一样的思想
// 虽然可以创建指向bool的指针，但指向单个位的指针是不允许的，引用也是一样
// 所以，设计operator[]接口被设计时产生了一个问题，返回值本应是T&
// 由于vector<bool>并不真的直接存储bool，则需要一种特殊的设计来处理这个问题
// 方式
// vector<bool>::operator[]返回一个对象，这个对象表现得像是一个指向单个位的引用，即所谓的代理对象
/*
template<typename allocator>
y_vector<bool, allocator>{
public:
	class reference {} //用来指向单个位的引用而产生的代理类
	reference opertor[](size_t n) {} //返回一个代理对象

}
*/
// 由此可见，上述test_vector_bool中代码不能通过的原因是
// 表达式右边返回的是代理类对象指针，而不是bool*类型
// 
//如果真的需要一个存放bool类型的容器，有两种选择
// 使用deque<bool>，提供了几乎所有vector的功能，它确实的存储了bool，但内存并不连续，所以不能传递给参数为bool数组的C API
// 另一种为使用bitset，虽不是STL容器，但也是标准C++库的一部分，与STL不同的是，它大小确定，不支持插入和删除，与vector<bool>一样使用了紧凑表示
//



int main()
{
	std::cout << "Hello World!\n";
}
