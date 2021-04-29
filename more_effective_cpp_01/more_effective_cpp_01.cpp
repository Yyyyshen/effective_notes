// more_effective_cpp_01.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>

/**
 * 参考《More Effective C++》
 */

using namespace std;

/**
 * 基础议题
 */

void* foo = nullptr;

//条款1：仔细区别pointers和references
// 
//指针和引用看起来不一样，但似乎做类似的事，何时使用哪一个
// 
//对引用的认知
// 没有所谓的null reference，一个引用总是代表了某个对象
//所以，
//如果一个变量，目的是指向或代表一个对象，但也可能不指向或代表任何对象，那么该使用pointer
//反之，
//如果变量不允许为null那么该用引用
void test_pointer_and_reference() {
	char* pc = 0;//指针定为null
	char& rc = *pc;//引用代表上面空指针的解引用，这并不合理

	//C++要求引用必须由初值
	//string& rs;//error
	string s("yshen");
	string& rs = s;//ok

	//指针可以不初始化，但也有风险
	string* ps;
}
//
//效率上
// 使用reference之前不需要验证
// 使用pointers之前通常需要判断其是否为null
// 
//重要差异
// pointers可以被重新赋值，指向另一个对象
// reference则总是指向最初的对象
void test_p_and_r_diff() {
	string s1("name1");
	string s2("name2");

	string& rs = s1;//rs为s1引用
	string* ps = &s1;//ps指向s1

	ps = &s2;//ps改为指向s2，不会改变s1
	std::cout << s1 << std::endl;//name1
	rs = s2;//rs仍然代表s1，但s1的值变为了s2的值
	std::cout << s1 << std::endl;//name2
}
//
//其他使用reference的情况
// 实现操作符时，最常见的是operator[]，返回引用
// 
//总结
// 当需要指向一个东西，而不会改变指向时；或实现操作符而语法无法用pointer达成时。选择引用
// 其他时候，尽量使用pointers
//


//条款2：最好使用C++转型操作符
// 
//C++中4种新型转型符
// static_cast
// const_cast
// dynamic_cast
// reinterpret_cast
// 
//这在第一本书中也提到了，更容易辨识这是一条转型语句
//并且每个操作符提供了各种不同功能的转型，更由严谨性
//


//条款3：绝对不要以多态方式处理数组
// 
//一个例子，继承关系中处理数组
class Base {};
class Derived :public Base {};
void print_base_arr(ostream& os, const Base arr[], int arr_size) {
	for (int i = 0; i < arr_size; ++i) {
		//os << arr[i];//假装定义了operator<<
	}
}
void test_print_base_arr() {
	Base base_arr[10];
	print_base_arr(cout, base_arr, 10);//没问题
	//如果使用了多态特性
	Derived derived_arr[10];
	print_base_arr(cout, derived_arr, 10);//编译器不会觉得有问题，而实际不正确
	//数组其实就是指针表达式
	//arr[i] 其实是 *(arr+i)
	//arr是一个指针指向数组起始处，arr+i与起始处距离为i*sizeof(arr_obj)
	//当把派生类数组传递给上述函数，编译器还会认为每个元素是Base类对象大小
	//此时如果派生类比基类含有更多成员变量，那么编译器产生的指针算数表达式就是不正确的
	//会发生不可预期行为
}
//


//条款4：非必要不提供default constructor
// 
//当类生成对象需要某些外界信息时，这样的类则不必拥有缺省构造
//毫无意义的default constructors会影响classes效率
//


int main()
{
	test_p_and_r_diff();
}