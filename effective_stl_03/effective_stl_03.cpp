// effective_stl_03.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <set>
#include <vector>
#include <algorithm>
#include <map>

/**
 * 3 关联容器
 */

using namespace std;

//第19条：理解相等（equality）和等价（equivalence）的区别
// 
//在STL中，例如find或insert这类的函数，必须能够比较两个对象是否相同
// find对相同的定义是相等，以operator==为基础
// set::insert对相同的定义是等价，以operator<为基础
// 所以，“相同”并不一定会有相同的断定结果，要理解等价和相等
// 
//一般情形下，一个关联容器比较函数并不是operator<，甚至不是less
// 是用户定义的判别式
// 每个标准关联容器都通过key_comp成员函数使排序判别式可被外部使用，常用下面格式表示x，y等价
// !c.key_comp()(x,y) && !c.key_comp()(y,x)
// 其中，c.key_comp()返回一个函数（对象），并以x和y作为传入参数
//
//另外，也要注意成员函数和非成员函数find的区分
// 例如有一个存放不区分大小写的set
struct string_compare :public binary_function<string, string, bool> {
	bool operator()(const string& lhs, const string& rhs) const {
		//return ci_string_compare(lhs, rhs);//略，后面条例中实现
		return true;//假定比较规则为不区分大小写
	}
};
void test_compare() {
	set<string, string_compare> set_str;
	set_str.insert("Yshen");
	set_str.insert("yshen");//根据比较规则，这条不会被插入，因为跟上面是等价的
	set_str.find("yshen");//根据比较规则，能够查找成功
	find(set_str.begin(), set_str.end(), "yshen");//无法查找成功，因为find中使用相等判定，而两字符串并不相等
}
// 一般来说，都是尽量先选择容器内成员函数
//


//第20条：为包含指针的关联容器指定比较类型
// 
//假定有一个包含string*指针的set
void test_set_with_pointer() {
	set<string*> set_str;
	//实际是以下的缩写
	set<string*, less<string*>, allocator<string*>> set_str1;
	//如果想要让集合内元素按字符串内容的值来排序，就需要自定义比较类型，因为less比较的是指针而不是其内容值
}
struct string_ptr_less :public binary_function<const string*, const string*, bool> {
	//template<typename ptr_type>//或使用模板来实现
	bool operator() (const string* ps1, const string* ps2)const {
		return *ps1 < *ps2;
	}
};
//对于容器内包含指向T对象的迭代器或智能指针的情况，也可以同样处理
//


//第21条：总是让比较函数在等值情况下返回false
// 
//如果使用less_equal作为比较类型，那么创建一个set
void test_less_equal() {
	set<int, less_equal<int>> set_i;
	set_i.insert(10);
	set_i.insert(10);//依然能够插入
	//判断条件类似这样
	//!(10<=10) && !(10<=10)
	//!(true) && !(true)
	//false && false
	//也就是说，结论是10和10不等价，从而被插入
}
//这样的结果甚至破坏了set，存在两个相等的值，则意味着它不再是一个集合
// 
//所以，任何定义了“严格的弱序化”的函数必须对相同值的两个拷贝返回false
//


//第22条：切勿直接修改set或multiset中的键
// 
//对于map容器，修改键将直接不能通过编译，所以不需要刻意记住
//而set是可以的，但这会破坏容器
// 
//安全的修改方式
// 找到想修改的元素，通常用
// 为将被修改的元素做一份拷贝
// 修改该拷贝
// 把元素中容器删除，通常使用erase
// 把新的值插入（可以使用第一步得到的迭代器位置，调用hint形式的insert，提高插入效率）
//


//第23条；考虑用排序的vector代替关联容器
// 
//当需要快速查找时，大多会想到用set、map这样的标准关联容器
//标准关联容器实现一般为平衡二叉搜索树，对于插入、删除和查找做了很多混合
// 
//而日常使用一个数据结构的方式通常有三个过程
// 设置阶段，创建新数据结构，插入大量元素
// 查找阶段，查询数据结构找到特定信息
// 重组阶段，改变数据结构内容
// 每个阶段都是很独立的操作，不需要混合插入删除和查找
//这种方式使用数据结构的话，树那样复杂的操作可能并不一定真的效率高，vector可能提供了更好的性能，前提是排序的vector
//对排序过的vector使用二分查找，比二叉搜索树执行二分查找具有更好的性能
// 首先，空间上，存储每个元素，树节点都需要额外的至少3个指针开销（左儿子、右儿子、父节点）；而vector的开销通常总共只有三个机器字，平均到每个元素是微不足道的
// 其次，假定数据结构足够大，分割后跨越多个内存页面，而vector相比之下需要更少的页面（开销小，并且是连续的）；关联容器则有可能散布在各个地址空间
// 当然，对于vector也有不足，每次插入新元素，都需要继续保证有序，所以可能每次插入元素后都需要位移很多元素，尤其是需要重新分配内存（扩容）时
void test_sort_vector() {
	vector<int> vec_i;
	//...填充元素
	sort(vec_i.begin(), vec_i.end());//排序
	int i = 666;
	binary_search(vec_i.begin(), vec_i.end(), i);
}
//当想要用vector代替map时
// 存储的数据是pair<K,V>，而不是完全模仿map使用pair<const K,V>
// 自定义比较函数，来确定pair的哪些部分进行比较
//


//第24条：当效率至关重要时，请在map::operator[]与map::insert之间谨慎做出选择
// 
//例子
class widget {
public:
	widget() {}
	widget(double num) {}//有一个double隐式构造
	widget& operator=(double num) {}
};
void test_map() {
	map<int, widget> map_w;
	map_w[1] = 1.5;
	map_w[2] = 3.6;
	//map[k] = v
	//这样的表达式代表检查k是否已经存在于map，若没有则加入v，若存在则将值更新为v
	//意义为添加和更新，operator[]的调用返回一个指向widget的引用
	//如果当前位置还没有元素，则会先默认构造一个widget，然后返回了该对象引用，之后才把其赋值为我们自己构造的对象
	//以这种方式添加一个元素，代码过程相当于
	typedef map<int, widget> iw_map;
	pair<iw_map::iterator, bool> result = map_w.insert(iw_map::value_type(3, widget()));
	result.first->second = 4.7;
	//而实际上，如果“直接使用所需值构造一个widget”比“先默认构造一个widget后再赋值”效率更高，我们其实只需要一步操作
	map_w.insert(iw_map::value_type(4, 5.9));//节省三个函数调用（临时对象的构造和析构，以及一个赋值操作）
}
//所以，在添加的情况下，insert比operator[]节省了更多的开销
// 
//更新操作时，情形就相反了
void test_map2() {
	map<int, widget> map_w;
	map_w[1] = 1.5;
	typedef map<int, widget> iw_map;
	map_w[1] = 1.6;//operator[]方式修改
	map_w.insert(iw_map::value_type(1, 1.7));//insert方式更新（insert对于重复key应该是直接忽略，而不是替换，一般是erase后再insert）
}
//所以，更新操作时，从代码简洁度和效率上，都是operator[]更好
//


//第25条：熟悉非标准的散列容器
// 
//标准STL中没有散列容器
// hash_set
// hash_multiset
// hash_map
// hash_multimap
//最常见实现来自于SGI，是开源的
//


int main()
{
	std::cout << "Hello World!\n";
}
