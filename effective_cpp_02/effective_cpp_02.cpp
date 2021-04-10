// effective_cpp_02.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <vector>
#include <list>

/**
 * 1.让自己习惯C++
 * Accustoming Yourself to C++
 */

using namespace std;

//条款01：将C++视为一个语言联邦
//View C++ as a federation of languages
// 
//可将C++视为以下四个次语言，每个次语言都有自己的规约：
//C，很多基础的内容，blocks、statements、preprocessor、built-in data types、arrays pointers等统统来自C语言
//Object-Oriented C++，C with Classes，使C具有面向对象能力
//Template C++，泛型编程
//STL，是个template程序库
//
//对于内置类型，pass-by-value比pass-by-reference高效
//对于自定义的类，有构造函数和析构函数，pass-by-reference-to-const往往更好，尤其是在泛型编程中，传进来的数据类型都不确定，所以经常需要加上const
//在STL中，迭代器和函数对象都是在C指针基础上塑造的，所以pass-by-value适用
//
//C++高效编程守则视状况而变化，取决于使用C++的哪个部分


//条款02：尽量适用const，enum，inline替换#define
//Perfer consts,enums,and inlines to #define
//
//宁可以编译器替换预处理器
#define ASPECT_RATIO 1.653
//宏定义不被视为语言的一部分，编译器处理源码之前可能就被预处理器移走，于是它可能没进入记号表（symbol table）
//所以当bug出现在#define中，调试起来就会麻烦，可以用常量替换上述宏
const double AspectRatio = 1.653;
//
//定义常量指针，常量定义式通常放在头文件，有必要将指针（不只是所指内容）声明为const，这就需要用两次const
const char* const authorName = "Yshen";
//同时 string 对象通常比char*-based合宜
const std::string authorName1 = "Yshen";
//
//class专属常量，为了将常量作用域限制于class内，必须让它成为class的一个成员，为确保只有一份实体，必须让它成为static成员
class GamePlayer {
private:
	static const int Num = 5;//常量声明
	int scores[Num];//使用常量
	static const double Num1;//常量声明，in-class初值设定只允许对整数常量进行
};
//上面例子中，Num为声明而非定义，通常c++要求对使用的任何东西提供定义，如果它是class专属常量又是static且为整数类型，则特殊处理
//只要不取地址，可以声明并使用他们而无须提供定义
//但如果要取某个class专属常量地址，或编译器坚持看到定义式，就要提供
const int GamePlayer::Num;//定义式，但没有给予数值，因为class常量在声明时获得了初值，定义时不可再设初值
const double GamePlayer::Num1 = 1.3;//常量定义，当编译器不允许在static成员在声明式上给初值时，只能将初值设定放在定义上
//并且，#define无法创建一个class专属常量，因为#define并不重视作用域
//另外，有些编译器不支持static整数型class常量完成in-class初值设定，可以换用the enum hack，
class Widget {
private:
	enum { NUM = 5 };//枚举令NUM作为5的记号
	int arr[NUM];
};
//枚举在行为上类似#define，例如取const变量的地址是合法的，而取enum的地址不合法，取#define的地址通常也不合法
//如果不想让别人获得一个指针或者引用指向你的某个常量，可以使用enum来实现约束
//
//另一个误用情况，用#define实现宏（macros），看起来像函数，但不会招致函数调用带来的额外开销
#define CALL_WITH_MAX(a,b) f((a)>(b)?(a):(b))
//这样的表达式要为所有实参加上括号，并且即使加了小括号，也可能有问题，例如
void f(int x) {}
void test_define() {
	int a = 5, b = 0;
	CALL_WITH_MAX(++a, b);//a被累加两次
	CALL_WITH_MAX(++a, b + 10);//a被累加一次
	//因为根据上述宏定义，a大于b，所以相当于比较时累加一次，决定选用a时又累加了一次，所以这样的宏定义并不好
}
//对于此问题，如果既想获得宏带来的效率以及一般函数的可预料行为和类型安全，可以使用template inline函数
template<typename T>
inline void callWithMax(const T& a, const T& b) {//无法预知T是什么，所以采用pass by reference-to-const
	f(a > b ? a : b);
}
//
//尽量多的使用consts、enums、inlines代替#define
//其他很必要的预处理器还有#include、#ifdef/#ifndef等


//条款03：尽可能使用const
//Use const whenever possible
//
//const，指定一个语义约束——不该被改动的对象，编译器会强制实施
//可以修饰的有，classes外部global作用域或namespace中的常量，或修饰文件、函数、区块中被声明为static的对象
//还可以修饰classes内部的static和non-static成员变量，对于指针可以修饰指针自身和指针所指内容
void test_const() {
	char greeting[] = "Hello";
	char* p1 = greeting;//non-const pointer,non-const data
	const char* p2 = greeting;//non-const pointer,const data
	char* const p3 = greeting;//const pointer,non-const data
	const char* const p4 = greeting;//const pointer,const data
	//简单的说就是将const右边的东西修饰为不可改变
	//写在星号左边，可看成修饰*p，也就是所指内容，表示值不可变
	//写在星号右边，则修饰的是p，也就是指针本身，表示指向地址不变
	//以下两种写法是一样的
	const char* p5 = greeting;
	char const* p6 = greeting;
}
//STL的迭代器相当于T*指针，声明迭代器为const就像声明指针为const（T*const），表示迭代器不得指向不同东西，但所指东西的值可以改动
//如果需要的是所指东西不可改动（即const T*)，需要的是const_iterator
void test_iter() {
	std::vector<int> vec;
	vec.push_back(0);
	const std::vector<int>::iterator iter = vec.begin();//iter是一个T* const
	*iter = 10;//没问题，改变的是iter所指内容
	//++iter;//error,iter是const，所以不能操作
	std::vector<int>::const_iterator cIter = vec.begin();//cIter是一个const T*
	//*cIter = 10;//error,*cIter是const的，不能改变所指内容
	++cIter;//没问题
}
//const还常用于函数声明，可以关联返回值、各参数；如果是成员函数，还可以修饰函数自身，表示该函数不能改变成员
//令函数返回一个常量值，往往可以降低调用者错误造成意外，例如
class Rational {};
const Rational operator*(const Rational& lhs, const Rational& rhs) {
	return Rational();
}
bool operator==(const Rational& lhs, const Rational& rhs) { return true; }
void test_return_const() {
	Rational a, b, c;
	//可避免以下情况
	//if ((a * b) = c) return;
	if ((a * b) == c) return;//有时手误会在判断中少打一个=，会编译通过，但逻辑就不正常了；使用了const修饰返回值后，就可以在编译期报错
}
//const成员函数，目的是确认该成员函数可作用于const对象上，使调用者容易得知哪个函数可以改动对象内容而哪个不行
//很多人忽视一件事：两个成员函数如果只是常量性不同，可以被重载。这其实很重要，例如
class TextBlock {
private:
	std::string text;
public:
	const char& operator[](std::size_t position) const { return text[position]; }
	char& operator[](std::size_t position) { return text[position]; }//如果函数返回类型为内置类型，那么改动返回值不合法，tb[0] = 'x';就会编译报错
	//就算返回char合法，tb.text[0]改动的也只是一个拷贝，不是tb[0]本身，也是没有意义的
};
void test_const_mf(const TextBlock& ctb) {
	TextBlock tb;
	std::cout << tb[0] << std::endl;//调用的是non-const的operator[]
	//对于const对象，常用传值方式 passed by reference-to-const
	std::cout << ctb[0] << std::endl;//调用const operator[]
	//ctb[0] = 'x';//error,const型不能修改
}
//成员函数是const，有两个流行概念
//bitwise constness(physical constness)：成员函数只有在不更改对象任何成员变量（static除外）时才说是const。即const成员函数不改变对象内任何non-static成员变量
//但这个const性质能够被绕过：一个更改了指针所指物的成员函数虽然不能算是const，但如果只有指针（非所指物）隶属于对象，那么声明为const并不会引起编译器异议
//这是有些矛盾的，举个不够恰当但足以说明问题的例子
class CTextBlock {
public:
	char& operator[](std::size_t position) const { return pText[position]; }
private:
	char* pText;
};
void test_bitwise_const() {
	const CTextBlock cctb;//声明常量对象
	char* pc = &cctb[0];//调用const operator[]取得指针
	*pc = 'Y';//这样是可行的，但似乎违背了const性质
}
//据此，有另一个概念被支持
//logical constness：一个成员函数可以修改他所处理对象内的某些bits，但只有客户端侦测不出时才如此
//继续看这个类
class CTextBlock_1 {
private:
	char* pText;
	std::size_t textLength;
	bool lengthIsValid;
public:
	std::size_t length() const {
		if (!lengthIsValid) {
			//textLength = std::strlen(pText);//error
			//lengthIsValid = true;//error，编译器不同意在const中修改这两个数据，但实际上可以有这样的需求
		}
		return textLength;
	}
};
//针对这样的需求，可以做这样的修改，使用关键字mutable，释放掉non-static成员变量的bitwise constness约束
class CTextBlock_2 {
private:
	char* pText;
	mutable std::size_t textLength;//mutable修饰的成员变量总是可能会被更改，即使是在const成员函数内
	mutable bool lengthIsValid;//
public:
	std::size_t length() const {
		if (!lengthIsValid) {
			textLength = std::strlen(pText);//这样就没问题
			lengthIsValid = true;//
		}
		return textLength;
	}
};
//
//在const和non-const成员函数中避免重复
//如果const和non-const两个函数都需要做很多事情，如下
class TextBlock_1 {
private:
	std::string text;
public:
	const char& operator[](std::size_t position) const {
		//边界校验、日志记录、数据完整性
		return text[position];
	}
	char& operator[](std::size_t position) {
		//边界校验、日志记录、数据完整性
		return text[position];
	}
};
//这会导致大量的代码重复，维护时会很麻烦；正常的做法通常是实现一次并复用，如下
class TextBlock_2 {
private:
	std::string text;
public:
	const char& operator[](std::size_t position) const {
		//边界校验、日志记录、数据完整性
		return text[position];
	}
	char& operator[](std::size_t position) {
		//转型调用
		return const_cast<char&>(static_cast<const TextBlock_2&>(*this)[position]);//复用const operator[]
	}
};
//转型（cast）通常是不建议做的，但本例中其实现是一样的，只是多了一个const修饰，这种情况将返回值const转除是安全的
//这其中有两次转型动作，第一次是将*this转为const型，否则会一直递归调用自身造成死循环
//第二次是将复用了const operator[]后的返回值移除const
//另外，推荐做法就是用非const调用const，反过来则不好，因为const成员函数承诺不改变对象逻辑状态，所以在其中调用non-const函数可能会破坏这个约束
//
//总结
//编译器遵循bitwise constness，而编程中应该遵循logical constness
//意味着你不该将一个返回数据为非常量引用的成员函数声明为const，即使是函数本身并没有修改对象，但调用者可以拿到这个引用来修改对象内部数据
//当const和non-const成员函数有等价实现，令non-const版本调用const版本可避免代码重复


//条款04：确定对象被使用前已被初始化
//Make sure that objects are initialized before they're used
// 
//将对象初始化
int x;
//某些语境下，x被初始化为0，其他语境不保证，例如
class Point {
private:
	int x, y;
};
Point p;
//p的成员有时被初始化，有时则不会
//读取未初始化值会导致不明确行为，某些平台读到未初始化值会终之程序，还有一些会读取一些半随机bits，污染数据，导致不可预知的结果
//有一些规则，描述对象初始化动作何时一定发生
//通常如果使用C part of C++并且初始化可能招致运行期成本，那么就不保证发生初始化，而non-C parts of C++不太一样
//例如array不保证内容初始化，而vector却有此保证
//这样的规则其实并不容易记，所以最好的办法就是：永远在使用对象之前将其手动初始化
void test_init() {
	int x = 0;//手动初始化为0
	const char* text = "A C-style string";//对指针手动初始化
	double d;
	std::cin >> d;//使用输入流读取方式完成初始化
}
//如果是内置类型意外的对象，初始化任务则在构造函数中进行：确保每个构造函数都将对象的每个成员初始化
//但要注意，不要混淆赋值和初始化
class PhoneNumber {};
class ABEntry {
public:
	ABEntry();
	ABEntry(const std::string& name, const std::string& address, const std::list<PhoneNumber>& phones);
private:
	std::string theName;
	std::string theAddress;
	std::list<PhoneNumber> thePhones;
	int numTimesConsulted;
};
//ABEntry::ABEntry(const std::string& name, const std::string& address, const std::list<PhoneNumber>& phones) {
//	theName = name;//这都是赋值，而非初始化
//	theAddress = address;
//	thePhones = phones;
//	numTimesConsulted = 0;
//}
//这虽然能让对象带有期望值，但并不是最佳做法
//C++规定，对象成员变量的初始化动作发生在进入构造函数本体之前
//而进入构造函数本体之后，就都不是初始化，而是被赋值
//一个比较好的写法是，使用member initializtion list
ABEntry::ABEntry(const std::string& name, const std::string& address, const std::list<PhoneNumber>& phones)
	:theName(name),
	theAddress(address),
	thePhones(phones),
	numTimesConsulted(0)
{
	//这样做的结果与上面版本相同，但通常效率更高
	//上面的版本先调用了default构造函数为成员设初值，紧接着对他们赋予新值，构造做的事就浪费了
	//这个版本就避免了这个问题，每个实参都被拿去作为各成员的构造实参（copy构造）
	//所以，比起先调用default构造再调用copy assignment操作符，往往比单调用一次copy构造效率低
	//对于内置类型int，则相同，但为了写法一致，最好都同样使用成员初值列来写
}
//无参构造同样适用这样的写法
ABEntry::ABEntry()
	:theName(),
	theAddress(),
	thePhones(),
	numTimesConsulted(0)
{
	//调用各成员的default构造
}
//编译器会为用户自定义类型的成员变量自动调用default构造函数
//可以规定自己，总是在初值列中列出所有成员变量，以免需要区分那些成员可以无需初值
//另外，有些情况，即使是内置类型，也必须适用初值列
//即如果变量是const或references，就一定需要初值，不能被赋值，这也是需要区分的一个点
//所以，为了不去记忆这么多情况，最简单的办法就是，总是使用成员初值列
//如果class存在多个构造函数，每个构造函数都有自己的成员初值列，会导致很多重复性工作
//此时则可以合理地将一些赋值表现与初始化表现一样好地成员变量去掉，改用赋值操作；或者根据c++11标准，委托构造地特性，使用一个构造函数给另一个构造函数初始化
//
//C++中成员初始化次序是固定的
//base classes更早于其derived classes被初始化；class内成员变量总是以其声明次序被初始化，不受其在初值列中次序影响
//所以，为了避免一些错误，例如初始化array时需要指定大小，代表大小的那个变量必须先有初值
//
//不同编译单元内定义的non-local static对象的初始化次序
//static对象的寿命是从被构造之后直到程序结束
//这种对象包括global对象、定义于namespace作用域内对象、在class内、在函数内、以及在file作用域内被声明为static的对象
//其中函数内的static对象是local static对象（相对于函数是本地的，虽然生命周期与程序相同，但可访问范围仅限于函数内），其他static对象成为non-local static对象
//编译单元指产出单一目标文件的源码，基本上指的是单一源码文件加上其所含头文件
//在两个源码文件中，都存在non-local static对象，则有一个问题产生：
//某个编译单元内的某个non-local static对象初始化使用了另一个编译单元内的某个non-local static对象，而这个对象可能并未初始化
//C++对于定义于不同编译单元的non-local static对象的初始化次序并无明确定义
//这并不只是因为难以确定，而是在一些情况，甚至没有正确次序可言
//比如，多个编译单元内的non-local static对象经“模板隐式具现化（implicit template instantiations）”形成
//因此，将每个non-local static对象搬到专属函数内，在由函数返回一个reference指向其所指对象，这也是单例模式常用手法
//基于：C++保证函数内local static对象会在函数被调用时、首次遇到该对象定义式时被初始化
//更好的是，如果从未调用这个函数，则不会引发构造和析构成本（可以说是懒加载）
//例子
class FileSystem {
private:
	int numDisks_;
public:
	std::size_t numDisks() const { return numDisks_; }
};
//FileSystem tfs;//避免使用这样的一个全局对象
FileSystem& tfs() {
	//用这样一个函数代替一个直接使用的FileSystem对象
	static FileSystem fs;//定义并初始化一个local static对象
	return fs;//返回一个引用指向该对象
}
class Directory {
public:
	Directory();
	Directory& tempDidr();
};
Directory::Directory() {
	std::size_t disks = tfs().numDisks();
}
Directory& tempDir() {
	static Directory td;
	return td;
}
//在多线程环境下，内含static对象的函数被调用时具有不确定性，有一种做法是：
//在程序单线程启动阶段，手动调用所有reference-returning函数，将所有引用取出，可消除与初始化相关的竞速形式（race conditions）
//
//综合起来说
//手动初始化内置型non-member对象
//使用成员初值列
//加强设计，避免一些初始化次序不确定的情况（使用local staic对象代替non-local static对象


int main()
{
	std::cout << "Hello World!\n";
}
