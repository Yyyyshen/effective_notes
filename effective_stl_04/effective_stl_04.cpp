// effective_stl_04.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <deque>
#include <fstream>

/**
 * 4 迭代器
 */

using namespace std;

//第26条：iterator优先于const_iterator、reverse_iterator及const_reverse_iterator
// 
//STL中所有标准容器都提供了四种迭代器类型，对于container<T>而言
// iterator类型相当于T*
// const_iterator相当于const T* (T const*，也是一样)
// reverse类型同理，只不过是反向遍历的迭代器
// 转换关系
// 				 const_iterator
//				↗				↖base()
//		iterator				const_reverse_iterator
//		base()↖↘				↗
//				reverse_iterator
// 
//


//第27条：使用distance和advance将容器的const_iterator转换成iterator
// 
//有些容器成员函数只支持iterator作为参数，而const_iterator不行
//而如果手上有一个const_iterator，在该位置插入新值，则需要转换
void test_const_to_iterator() {
	typedef deque<int> int_deque;
	typedef int_deque::iterator iter;
	typedef int_deque::const_iterator const_iter;

	int_deque d;
	const_iter ci;
	iter i(d.begin());//创建一个容器起始位置的迭代器，然后通过偏移量计算
	advance(i, distance<const_iter>(i, ci));//移动i，使其指向ci所指位置；另外需要显式指明distance所使用类型参数
}
//本质上还是使用了一个新的iterator，也正是26条所说的原则
//


//第28条：正确理解由reverse_iterator的base()成员函数所产生的iterator的用法
// 
//reverse_iterator的base成员函数可以得到与之相对应的iterator
// 如果想要在reverse_iterator指定位置上插入元素，只需要在base位置处插入元素即可
// 如果要在reverse_iterator指定位置删除一个元素，则需要在base位置前面的位置上执行删除
// 而表达式 --ri.base()往往不能通过编译，考虑通用性和移植性，一般先递增一次reverse_iterator再调用base： v.erase((++ri).base());
//


//第29条：对于逐个字符的输入请考虑使用istreambuf_iterator
// 
void test_istreambuf() {
	ifstream input_file("111.txt");
	input_file.unsetf(ios::skipws);//清除输入流标志，否则会跳过空白字符
	string file_data((istream_iterator<char>(input_file)), istream_iterator<char>());//这里第一个参数为什么加括号第6条有解释
	//若使用istreambuf_iterator，则不会跳过任何字符，并且速度会更快
	string file_data_buf((istreambuf_iterator<char>(input_file)), istreambuf_iterator<char>());
}
//同理，对于输出流，也可以考虑使用ostreambuf_iterator
//


int main()
{
	std::cout << "Hello World!\n";
}
