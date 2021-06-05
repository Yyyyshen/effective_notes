// effective_stl_07.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>

/**
 * 7 在程序中使用STL
 */

using namespace std;

//第43条：算法调用优先于手写的循环
// 
//尽量先选择以一对迭代器为参数的算法，而不是手写循环，主要有几个理由
// 效率上，算法通常比自己写的循环效率更高
// 正确性，自己的循环比算法更易出错
// 可维护性上，算法的代码更简洁易读
class widget {
public:
	void test() {}
};
void test_loop_and_algorithm() {
	//效率对比
	vector<widget> vec_w;
	for (auto iter = vec_w.begin(); iter != vec_w.end(); ++iter)
	{
		iter->test();
	}
	//使用算法
	for_each(vec_w.begin(), vec_w.end(), mem_fun_ref(&widget::test));
	//自己写循环每次都要调用end()方法
	//当然这是影响很小的性能问题，更主要的是，STL的实现者一定是比我们更加熟悉容器的，所以根据了解程度，他们会对遍历过程进行优化，而我们很难做到
}
// 
//正确性上，需要保证迭代器都是有效的，并且指向你所希望的地方；自己写循环可能会出现并不是想要的结果
//算法名称往往能够告诉读代码的人它的功能，而自己写循环则没那么明确含义
// 
//所以，尽量遵循此建议
//


//第44条：容器的成员函数优先于同名的算法
// 
//一些STL容器中提供了与算法同名的成员函数
// 关联容器提供，count、find、lower_bound、upper_bound、equal_range
// list提供，remove、remove_if、unique、sort、merge、reverse
//他们往往有一些优势
// 成员函数往往速度更快
// 成员函数通常与容器结合更加紧密
//同名但做的事不一定完全相同
// 例如set中使用find
// 在百万数据量下执行
// find成员函数以对数时间运行，最坏情况大约40次找到结果，平均20次
// find算法直接遍历，最坏情况时百万次，平均50w次
//


//第45条：正确区分count、find、binary_search、lower_bound、upper_bound和equal_range
// 
//选择查找策略时，先看区间是否排序
// 如果是，那么通过binary_search、lower_bound、upper_bound和equal_range会有更快的速度
// 如果没有排序，则只能选择count、find或其带if条件的算法
// 
//在count和find之间
// 判断count的数量来确定存在性是习惯用法
// 判断find返回迭代器是否为end也是习惯用法
// 前者相对代码更容易写，但效率较差，因为find找到即返回，count一定会到达末尾
// 
//在排序区间中测试是否存在某特定值
// 可以使用binary_search，但只返回bool类型
// 如果要确定这个值具体在哪，需要使用equal_range，下面说为什么不用lower、upper
// 而lower_bound的意义不太一样，值找到时，会返回位置，如果不在，会返回一个它应该插入哪里的位置（upper同理）
// equal_range返回一对迭代器，第一个迭代器为lower_bound返回、第二个等于upper_bound返回
//


//第46条：考虑使用函数对象而不是函数作为STL算法的参数
// 
//函数对象比函数在STL算法使用中速度更快，主要在于
// 函数内联
// 如果一个函数对象的operator()函数已经被声明为内联（显式inline声明、或定义在类定义内部隐式内联）
// 那么其函数体可直接被编译器使用
// 但如果将一个函数作为参数传递时，编译器会隐式转换为一个指向该函数的指针并传递
// 内部使用时，编译器会产生一个通过指针发出的间接调用，大多数编译器不会试图对函数指针执行内联优化
// 所以性能的差异就产生了
//
//另一个理由是，函数不一定能完全合法的通过编译
//还能避免一些微妙的语言陷阱
//


//第47条：避免产生“直写型”（write-only）的代码
// 
//当一条代码有过多的函数嵌套和动态创建的函数对象时，是不利于维护的
//最好将其分开，理清思路
//


//第48条：总是包含（#include）正确的头文件
// 
//不同的C++实现可能有不同的相互包含关系
// 有的vector头包含了string
// 有的algorithm包含了string
// 有的iostream包含了iterator
// 有的iostream包含了string和vector
// 等等
// 
//所以，至少包含与所用内容同名的头文件
// vector、list、set、map
// algorithm、numeric
// iterator
// functional
// 等等
// 
//这会提高可移植性
//


//第49条：学会分析与STL相关的编译器诊断信息
// 
//一些技巧
// vector和string迭代器通常就是指针，错误使用iterator时，诊断信息可能会提到指针类型
// 信息中提到back_insert_iterator、front_insert_iterator时，几乎意味着错误的使用方式，或者某个函数间接使用了他们
// 类似的，提到binder1st或者binder2nd也是如此
// 输出迭代器使用错误时，可能在错误信息中提到与赋值操作符有关的内容
// 如果错误消息来源于某个STL算法内部实现，那可能是使用了错误的类型
// 没有识别到某组件或算法，可能是没包含对应的头文件
//


//第50条：熟悉STL相关的Web站点
//
//SGI
//STLport
//Boost
//


int main()
{
	std::cout << "Hello World!\n";
}
