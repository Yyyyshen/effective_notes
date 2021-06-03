// effective_stl_05.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include <list>
#include <set>

/**
 * 5 算法
 */

using namespace std;

//第30条：确保目标区间足够大
// 
//一个常见想法
int trans_num(int x) { return 1; }
void test_transform() {
	vector<int> vec_i;
	vector<int> vec_ret;
	//...存一些值
	transform(vec_i.begin(), vec_i.end(), vec_ret.end(), trans_num);
	//将trans_num作用于vec_i每个元素，并把返回值追加在vec_ret结尾
	//transform通过赋值操作将结果写到目标区间，也就是先以vec_i[0]为参数调用trans_num，并将结果赋给*vec_ret.end()
	//实际上，vec_ret.end()并没有指向任何对象，对无效对象的赋值可能引起灾难性后果
	transform(vec_i.begin(), vec_i.end(), back_inserter(vec_ret), trans_num);
	//back_inserter生成了一个迭代器，指向目标区间起始位置，并且将赋值操作转换为push_back，以下是源码摘录
	/*
		_CONSTEXPR20 back_insert_iterator& operator=(const typename _Container::value_type& _Val) {
			container->push_back(_Val);
			return *this;
		}
	*/
}
//这类方法也可以在调用前搭配reserve或resize使用，提前给好所需的空间
//


//第31条：了解各种与排序有关的选择
// 
//最容易想到的排序方法为sort，另一种只排序一部分的算法为partial_sort
class widget {};
bool quality_compare(const widget& lhs, const widget& rhs) { return true; }
bool has_quality(const widget& w) { return true; }
void test_sort() {
	vector<widget> vec_w;
	//一次性排序所有元素
	sort(vec_w.begin(), vec_w.end());//或stable_sort
	//将质量最好的20个元素按顺序放在vector的前20个位置上
	partial_sort(vec_w.begin(), vec_w.begin() + 20, vec_w.end(), quality_compare);
	//若只需要挑出前n个元素，但不在乎排序，则可以选择nth_element
	nth_element(vec_w.begin(), vec_w.begin() + 19, vec_w.end(), quality_compare);
	//要将满足某条件的元素放到vector前部，可以使用以下方法
	partition(vec_w.begin(), vec_w.end(), has_quality);//或stable_partition
}
//


//第32条：如果确实需要删除元素，则需要在remove这一类算法之后调用erase
// 
//先要了解remove，需要一对迭代器来指定操作区间，而并没接受容器
// remove不可能推断出是什么容器，也并不知道它操作的元素所在容器
// 所以，remove并不是真正意义的删除，因为它做不到
// 它所做的事情，是移动区间中的元素，将不删除的元素移到了区间前部（并不意味着将需要删除的都移到后部，后部可能只是保持原状，前部需要被删除的元素可能已经被覆盖掉）
// （|1|2|3|99|5|99|7|8|9|99|  -----remove(99)----->  |1|2|3|5|7|8|9|8|9|99|）
// 返回值为指向最后一个不用被删除的元素之后的迭代器
//
//若要真的删除元素，还需要搭配erase
void test_erase_remove() {
	vector<int> vec_i;
	vec_i.erase(remove(vec_i.begin(), vec_i.end(), 99), vec_i.end());
}
//另外需要注意的是，STL中位移一个名为remove并且确实删除了容器中元素的函数是
// list的remove成员函数
//
//（unique行为类似，包括作为成员函数时）
//


//第33条：对包含指针的容器使用remove这一类算法时要特别小心
// 
//看一个用vector存放指针的情况
// 
// 		+---+												   +---+
// 		| 0 |  ------>  widget a							   | 0 |  ------>  widget a
// 		|---|												   |---|
// 		| 1 |  ------>  widget b （需要被删除） ---remove--->  | 1 |  ----	   widget b 
// 		|---|												   |---|	  ↘
// 		| 2 |  ------>  widget c							   | 2 |  ------>  widget c
//		+---+												   +---+
// 
// 要被删除的指针（widget b）已经被不会被删除的指针覆盖了
// 没有任何指针再指向它，这就发生了资源泄漏
// 如果使用的是有引用计数功能的智能指针，可以直接使用erase-remove的习惯用法
//


//第34条：了解哪些算法要求使用排序的区间作为参数
// 
//罗列要求排序区间的STL算法
// binary_search、lower_bound、upper_bound、equal_range、
// set_union、set_intersection、set_difference、set_symmetric_difference
// merge、inplace_merge、includes
//另外，不一定要求排序区间，但通常情况下会与排序区间一起用的算法
// unique、unique_copy
// 
//之所以要求排序区间，目的是为了提供更好的性能
//只要确保提供排序区间，并保证这些算法使用的比较函数与排序使用的比较函数一致，就可以有效的执行操作
//


//第35条：通过mismatch或lexicographical_compare实现简单的忽略大小写的字符串比较
// 
//之前条例中提到的忽略大小写的方法
int ci_char_compare(char c1, char c2) {
	//tolower参数和返回值都是int，除非int是EOF，否则它的值必须可以用unsigned char表示，所以需要先强转
	int ic1 = tolower(static_cast<unsigned char>(c1));
	int ic2 = tolower(static_cast<unsigned char>(c2));
	//都转换为小写后进行比较
	if (ic1 < ic2) return -1;
	if (ic1 > ic2) return 1;
	return 0;
}
//可以根据mismatch算法，编写比较字符串的函数，但一般两字符串长度不一样，必须把短的字符串作为第一区间传入
int ci_string_compare_impl(const string& s1, const string& s2);
int ci_string_compare(const string& s1, const string& s2) {
	if (s1.size() <= s2.size()) return ci_string_compare_impl(s1, s2);
	else return -ci_string_compare_impl(s2, s1);
}
int ci_string_compare_impl(const string& s1, const string& s2) {
	typedef pair<string::const_iterator, string::const_iterator> psci;
	//mismatch返回一对迭代器，指示两个区间中对应字符第一次比较失败的位置
	psci p = mismatch(s1.begin(), s1.end(), s2.begin(), not2(ptr_fun(ci_char_compare)));
	if (p.first == s1.end())
	{
		//比较失败的位置在第一个区间结尾，要么是字符串相同，要么是s1更短
		if (p.second == s2.end()) return 0;
		else return -1;
	}
	return ci_char_compare(*p.first, *p.second);//如果不匹配出现在中间，则字符串之间关系和两个不匹配字符之间的关系相同
}
//第二种实现方式
bool ci_char_less(char c1, char c2) {
	return tolower(static_cast<unsigned char>(c1)) < tolower(static_cast<unsigned char>(c2));
}
bool ci_string_compare2(const string& s1, const string& s2) {
	return lexicographical_compare(s1.begin(), s1.end(), s2.begin(), s2.end(), ci_char_less);
}
//还可以使用 strcmp
int ci_string_compare3(const string& s1, const string& s2) {
	return strcmp(s1.c_str(), s2.c_str());
}
//


//第36条：理解copy_if算法的正确实现
// 
//在STL中，有11个名字中包含copy的算法
// copy、copy_backward、replace_copy、reverse_copy、
// replace_copy_if、unique_copy、remove_copy、rotate_copy
// remove_copy_if、partial_sort_copy、uninitialized_copy
//但却没有copy_if，所以，如果想复制区间中符合某条件的元素，则需要自己实现
// 
//一个看似合理的copy_if
template<typename input_iterator, typename output_iterator, typename predicate>
output_iterator copy_if(input_iterator begin, input_iterator end, output_iterator dest_begin, predicate p) {
	return remove_copy_if(begin, end, dest_begin, not1(p));
}
//这种做法是以这样的事实为基础的：
// 虽然STL不允许“复制所有使判别式条件为真的元素”
// 但它允许“复制所有使判别式条件不为真的元素”
// 所以，把判别式加上not1修饰后传递给remove_copy_if，就实现了copy_if
// 然而，not1不能直接应用到一个函数指针上，必须先用ptr_fun进行转换
// 为了调用copy_if这个实现，传入的不仅是一个函数对象，且应该是一个可配接的函数对象
// 可以这样进行完善
template<typename input_iterator, typename output_iterator, typename predicate>
output_iterator copy_if(input_iterator begin, input_iterator end, output_iterator dest_begin, predicate p) {
	while (begin != end) {
		if (p(*begin)) *dest_begin++ = *begin;//手动遍历调用
		++begin;
	}
	return dest_begin;
}
//


//第37条：使用accumulate或者for_each进行区间统计
// 
//一些常见算法
// count可以得出一个区间有多少元素，count_if则统计满足某个判别式的元素个数
// min_element、max_element可以获取区间中的最大和最小值
// 有时，需要自定义方式对区间进行统计处理
// 例如计算一个容器中字符串长度综合、一个区间中数值乘积、平均值等
// 可以使用accumulate
#include <numeric> //不像其他算法那样存在于algorithm中
void test_accumulate() {
	list<double> list_d;
	//...插入一些元素
	double num = accumulate(list_d.begin(), list_d.end(), 0.0);
	//这里0.0很重要，代表初始值为double，如果使用0，则最终计算结果会是int型，即使通过编译，结果也是错误的
}
//除了默认用法，还能自定义统计函数
string::size_type string_length_sum(string::size_type sum_so_far, const string& s) {
	return sum_so_far + s.size();
}
void test_accumulate2() {
	set<string> set_str;
	string::size_type len_sum = accumulate(set_str.begin(), set_str.end(), static_cast<string::size_type>(0), string_length_sum);
}
//计算乘积，直接用标准函数
void test_accumulate3() {
	vector<float> vec_f;
	float ret = accumulate(vec_f.begin(), vec_f.end(), 1.0f, multiplies<float>());
}
//
//for_each有相似的作用，但不同于accumulate直接返回统计结果，它返回一个函数对象，必须从中提取出所要的结果
// 这也意味着，我们需要在函数子中加一个成员函数，来返回结果
// 
//（for_each函数参数允许有副作用，而accumulate不允许）
//


int main()
{
	std::cout << "Hello World!\n";
}
