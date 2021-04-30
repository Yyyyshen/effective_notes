// more_effective_cpp_02.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>

/**
 * 操作符
 */

using namespace std;

//条款5：对定制的类型转换函数保持警觉
// 
//C++中有两种函数会被编译器执行类型转换
// 单自变量constructors
// 隐式类型转换操作符
class y_rational {
public:
	y_rational(int numerator = 0, int denominator = 1) {}//int可以隐式转为此类对象，属于单自变量constructors

	operator double() const {} //隐式类型转换操作符，没有返回值类型

};
//隐式类型转换函数经常会导致未预期的结果，并且也不直观，所以最好不要提供
// 比如，使用cout<<输出一个y_rational对象，本来对象内没有实现operator<<应该无法使用
// 但此时编译器想尽办法，发现有类型转换成double，可以调用operator<<double，这虽然不会引起严重后果，但也并不合理
//
//可以使用其他成员函数来实现类型转换，比类型转换操作更直白
// double as_double() const;//定义这样的函数在y_rational内部
// 
//对于单自变量构造，可以使用explicit关键字
// 


//条款6：区别increment/decrement操作符的前置（prefix）和后置（postfix）形式
// 
//早期C++还没有分别前置后置，为了补充这个语言漏洞，定义了后置式有一个int自变量
class y_int {
public:
	y_int& operator++() {
		//累加后取出
		return *this;
	}//前置++
	const y_int operator++(int) {
		//取出后累加
		y_int temp = *this;//记录原值
		++(*this);//累加，依赖于前置式，这样就只需要维护前置版本操作
		return temp;//返回原值
	}//后置++

	//--同理
};
void test_y_int() {
	y_int i;
	++i;//调用operator++()
	i++;//调用operator++(0)，编译器自动将int解释为0，所以定义是也不需要设置形参名
}
//对于返回值
// 前置返回一个引用是由于返回的是累加后的本身
// 后置返回一个对象是由于返回的是累加前的旧值
// 后置返回的对象为const的原因是，如果返回的不是const，这样的语句被允许	i++++;//连续两次后置++，i.operator++(0).operator++(0);
// 而两次++即使合法也不合理，因为第二次++作用于第一次返回的temp对象上，i本身最终只被累加了一次，会引起混淆
// 
//处理用户定制类型时，应该尽可能使用前置式，因为通过上面例子可以看出，后置往往产生了临时对象，对效率有影响
//


//条款7：千万不要重载&&，||和,（逗号）操作符
// 
//在大多数人的习惯里，真假表达式采用的都是骤死式评估方式，一旦表达式真假确定，即使还有部分尚未检查，整个评估工作就结束了
//所以重载后，可能与这样的惯用思维有冲突
// 
//逗号操作符
// 比较常见的是for循环中的使用 for(int i = 0,j = 100;i<j;++i,--j)
// 逗号与逻辑操作符类似，先评估逗号左侧，再评估右侧
// 


//条款8：了解不同意义的new和delete
// 
//使用new来创建对象时，是所谓的new operator
//void* operator new(size_t size);这样的定义是operator new，返回一个指针，指向一块原始内存，函数任务类似malloc，不涉及构造方法，就只是分配内存，
// 
//特殊版本的operator new成为placement new，即在一个指针指向的内存上构造一个对象
// 
//delete也有对应的 delete operator和operator delete（相当于free，不涉及析构）
//


int main()
{
	std::cout << "Hello World!\n";
}
