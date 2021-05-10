// more_effective_cpp_04.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>

/**
 * 效率
 */

using namespace std;

//条款16：谨记80-20法则
// 
//一个程序80%的资源用于20%的代码上
// 资源指的是执行时间、内存、磁盘访问、维护成本等
// 也有人赞同90-10法则，但宗旨就是软件整体性能取决于一小部分代码
//


//条款17：考虑使用lazy evaluation（缓式评估）
// 
//最具效率的就是未被执行的运算
// 直到某种运算结果必须立马被需要之前，都应尽量拖延
// 
//常用四种情况：
// 在真正需要之前，不必急于给某物做一个副本
// 延缓决定读和写
// 缓式取出，在需要时才在member functions内赋值（包括const成员函数），所以可以将data members声明为mutable 指针字段
// 表达式缓评估
//


//条款18：分期摊还预计的计算成本
// 
//如果预期程序常常会用到某个计算，可以降低每次计算的平均成本，办法就是设计一份数据结构以便有效率地处理需求
//可以成为超前评估——over-eager evaluation
template<class numerical_type>
class data_collection {
public:
	numerical_type min() const {};
	numerical_type max() const {};
	//...
};
//当这样地调用很频繁时，可以随时记录程序执行过程中数据集的各种操作
//一旦函数被调用，便可以立即返回正确的值，而无须再计算，这样分次摊还的成本会更低
//（例如vector中，当容量即将不够用时，进行双倍扩容，提前分配更多内存，这样就避免了每次添加都再次扩容的资源消耗）
//这与条例17并不矛盾
// 当必须支持某些运算而结果并不总是需要时，缓式评估可以改善效率
// 当必须支持某些运算并且结果几乎总是被需要时，超前评估可以改善效率
//


//条款19：了解临时对象来源
// 
template<class T>
void y_swap(T& obj1, T& obj2) {
	T temp = obj1;
	obj1 = obj2;
	obj2 = temp;
}
//人常将temp称为一个临时对象，但C++中，它只是函数中的局部对象，并不能说是临时对象
//真正的临时对象是不可见的，没有在代码中出现的
// 只要产生一个non-heap object而没有命名时，便产生了临时对象
// 也可以说是匿名对象
// 通常发生于，隐式类型转换、函数返回对象时
// 需要考虑这些对象的产生和销毁对性能的影响
// 
size_t count_char(const string& str, char ch) { return 0; }
void test_count_char() {
	char buffer[] = "Hello,world.";
	char c = 'l';

	cout << count_char(buffer, c) << endl;
}
//上例中，会产生一个以buffer作为自变量，调用string ctor初始化出的临时对象
// str参数会被绑定于此string临时对象上
// 当count_char返回时，此临时对象会被销毁
// 
class Widget {};
const Widget operator+(const Widget& lhs, const Widget& rhs) { return Widget(); };
//这样的函数返回值就是临时对象
// 没有名称，就是返回值
// 
//总结，
//临时对象可能比较消耗成本，所以应当尽量消除
// 需要训练眼力，根据经验看出临时对象产生的地方
// 当看到一个reference-to-const参数，就极有可能产生临时对象
// 当看到函数返回一个对象，就会产生临时对象
//


//条款20：协助完成“返回值优化（RVO）”
// 
//如条例19所说，函数以by-value方式返回对象，其临时对象的产生和析构则无法消除
// 当一个函数必须返回一个对象时，不应该以指针（容易忘记回收资源）或引用（容易指向不存在的对象）方式去对抗
// 应该在乎的是对象的成本，而不是消除对象本身
class y_rational {
public:
	y_rational(int numerator = 0, int denominator = 1) :numerator_(numerator), denominator_(denominator) {}
	int numerator() const { return numerator_; }
	int denominator() const { return denominator_; }
private:
	int numerator_;
	int denominator_;
};
/* inline */const y_rational operator*(const y_rational& lhs, const y_rational& rhs) {
	return y_rational(lhs.numerator() * rhs.numerator(), lhs.denominator() * rhs.denominator());
}
//这个例子，调用了一个构造，产生了临时对象，函数复制这个对象当作返回值
// 这种方式取代局部对象，似乎没有什么改变，因为还是付出了构造和析构
// 实际上，在C++编译器对临时对象优化时，产生了一些优势
// y_rational c = a*b;
// 若这样来使用到上面的函数，编译器能够消除operator*内的临时对象及被operator*返回的临时对象
// 它可以将return表达式所定义的对象构造于c的内存内，那么如果这样被优化了，operator*中临时对象的总成本就为0，只是付出了一个用于产生c的ctor代价
// 
//某些时候，采用inline也可以消除额外开销（前提是了解inline，上一本书中有讲过）
//


//条款21：利用重载技术（overload）避免隐式类型转换（implicit type conversions）
// 
class y_int {
public:
	y_int() {}
	y_int(int value) {}
};
const y_int operator+(const y_int& lhs, const y_int& rhs) { return y_int(); }
void test_y_int() {
	y_int i1, i2;
	y_int i3 = i1 + i2;//没什么问题
	y_int i4 = i1 + 10;//能成功，但发生了隐式转换，产生了临时对象
	y_int i5 = 10 + i2;//同上
}
//如果加上下面几个重载，则不会隐式转换
const y_int operator+(const y_int& lhs, int rhs) { return y_int(); }
const y_int operator+(int lhs, const y_int& rhs) { return y_int(); }
//const y_int operator+(int lhs, int rhs) { return y_int(); }//error
// 若重载了只有int变量的操作符，那么预先定义好的操作符意义就改变了，导致各种各样的问题
// 
//此条例不止适用于操作符函数
// 例如传参中，如果接受char*，那么应该希望也接受string对象
// 同时也应兼顾80-20法则，大量重载函数不一定是好事
//


//条款22：考虑以操作符复合形式（op=）取代其独身形式（op）
// 
//一般来说，如果能够写如下语句
// x = x + y;
//应当能够写成
// x += y;
//其他操作符-、*、/也一样
//
//可基于+=这样的方法来实现+这样的操作
// 
//效率和便利性
// result = a + b + c + d;//产生三个临时对象，每个对应于一次+的调用
// result = a; result += b; ... //不产生临时对象，但写起来麻烦
//


//条款23：考虑使用其他程序库
// 
//当程序出现性能瓶颈，尝试对一些操作使用另一个程序库
// 比如遇到了I/O瓶颈，考虑用stdio取代iostreams
// 如果动态分配/释放内存较多，可以考虑使用提供了优化operator new/delete的程序库
// 网络遇到瓶颈，考虑各种第三方如asio、libevent等网络库
//


//条款24：了解virtual functions、multiple inheritance、virtual base classes、runtime type identification的成本
// 
//了解编译器以什么样的方式来实现某些语言特性是重要的，其中最重要的就是虚函数
// 当一个虚函数被调用，执行代码必须对应于对象的动态类型，大部分编译器使用vtbls和vptrs
// vtbl通常是一个由函数指针架构而成的数组，某些编译器使用链表，但基本策略相同
class C1 {
public:
	C1();
	virtual ~C1();
	virtual void f1();
	virtual int f2(const char* c);
	void f3() const;
};
// C1's vtbl
// 	+----+
// 	|	-|->Implementation of C1::~C1()
// 	|----|
// 	|	-|->Implementation of C1::f1()
// 	|----|
// 	|	-|->Implementation of C1::f2()
// 	+----+
// 非虚函数构造及f3()不在表中
// 如果有继承关系，派生类虚函数表内同时有派生类内虚函数以及未被重新定义的C1虚函数
class C2 :public C1 {
public:
	C2();
	virtual ~C2();
	virtual void f1();
	virtual void f4();//新的虚函数
};
// C2's vtbl
// 	+----+
// 	|	-|->Implementation of C2::~C2()
// 	|----|
// 	|	-|->Implementation of C2::f1()
// 	|----|
// 	|	-|->Implementation of C1::f2()
// 	|----|
//  |	-|->Implementation of C2::f4()
//  +----+
// 
//凡是声明有虚函数的class，对象内都含有一个隐藏的data member，用来指向该class的vtbl，即所谓的vptr
// 如果对象不大，这份额外的开销是值得注意的
// 在内存不充裕的系统中，意味着程序产生的对象个数减少了
//	


int main()
{
	std::cout << "Hello World!\n";
}
