// effective_stl_05.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <vector>
#include <algorithm>

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
//
//


int main()
{
	std::cout << "Hello World!\n";
}
