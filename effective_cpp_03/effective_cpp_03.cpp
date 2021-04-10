﻿// effective_cpp_03.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>

/**
 * 2.构造/析构/赋值运算
 * Constructors,Destructors,and Assignment Operators
 */

using namespace std;

//条款05：了解C++默认编写和调用的函数
//Know what functions C++ silently writes and calls
//
//一个空类
class Empty {};
//编译器处理后，会声明一个copy构造、一个copy assignment操作符函数、一个析构函数
//如果没有声明任何构造函数，还会声明一个default构造函数
//所有这些函数都是public且inline
class EmptyAfterCompile {
public:
	EmptyAfterCompile();
	EmptyAfterCompile(const EmptyAfterCompile& rhs);
	~EmptyAfterCompile();

	EmptyAfterCompile& operator=(const EmptyAfterCompile& rhs);
};
//唯有当这些函数被调用，它们才会被编译器创建出来
//编译器帮我们写的函数都做了哪些事：
//default构造和析构函数主要给编译器一个地方放置藏身幕后的代码，像是调用base-classes和non-static成员变量的构造和析构
//编译器产出的析构函数是一个non-virtual函数，除非这个class的base class自身声明virtual析构函数
//copy构造和copy assignment操作符，编译器创建的版本只是单纯将源对象的每个non-static成员变量拷贝到目标对象
template<typename T>
class NamedObject {
public:
	NamedObject(const char* name, const T& value) {};
	NamedObject(const std::string& name, const T& value) {};
private:
	std::string nameValue;
	T objectValue;
};
//上面类既没有声明copy构造函数和copy assignment操作符，所以编译器会为其创建（如果被调用），但不会创建default构造（只要有一个构造函数，编译器就不会自动创建默认构造）
void test_copy_ctor() {
	NamedObject<int> no1("Smallest Prime Number", 2);
	NamedObject<int> no2(no1);//调用编译器生成的copy构造
	//编译器生成的copy构造是以no1.nameValue和no1.objectValue为初值设定no2.nameValue和no2.objectValue
	//两者中，nameValue类型为string，有copy构造函数，所以no2.nameValue初始化方式是调用string的copy构造并以no1.nameValue为实参
	//另一个成员objectValue类型是int，是一个内置类型，所以no2.objectValue会以拷贝no1.objectValue内每一个bits来完成初始化
	//内置类型没有拷贝构造一说，基础类型在内存中结构布局比较简单，通常直接从底层拷贝变量所占的整个内存单元即可
}
//编译器生成的copy assignment操作符，行为与copy构造函数基本一样；但不满足条件的话，编译器会拒绝为class生成operator=
template<typename T>
class NamedObject2 {
private:
	std::string& nameValue;//与上面不同，是一个引用
	const T objectValue;//与上面不同，是一个const变量
public:
	//以下构造函数的nameValue不再接收一个const，因为类成员是个string引用
	NamedObject2(std::string& name, const T& value) :nameValue(name), objectValue(value) {};
	//同样没有operator=
};
void test_copy_assign() {
	std::string newDog("Moka");
	std::string oldDog("Apollo");
	NamedObject<int> mk(newDog, 2);
	NamedObject<int> apl(oldDog, 3);
	mk = apl;//可以操作
	NamedObject2<int> mk2(newDog, 2);
	NamedObject2<int> apl2(oldDog, 3);
	//mk2 = apl2;//error，不支持赋值操作
	//两个对象的nameValue都指向string对象，赋值操作如果是mk2.nameValue指向apl2.nameValue所指的string，就改变了引用自身
	//C++不允许让reference改指向不同对象
}
//如果打算在内含reference成员的class内支持赋值操作，必须自己定义copy assignment操作符
//内含const成员的class也是一样的，改动const成员是不合法的，编译器不知道如何生成operator=
//还有一种情况，就是如果某个base class将copy assignment操作符声明为private，编译器也将拒绝为其derived class生成operator=


//条款06：若不想使用编译器自动生成的函数，要明确拒绝
//Explicitly disallow the use of compiler-generated functions you do not want
//
//有时候，并不想使用编译器自动生成的copy构造和copy assignment操作符
//其中一个办法是，将两个函数手动声明为private，阻止了编译器生成和使用者调用
//但并不绝对安全，因为成员函数和friend函数可以调用，除非不去定义它们
//将成员函数声明为private并故意不去实现是一种常见做法，C++iostream库中ios_base,basic_ios中阻止copying行为
class Home {
private:
	Home(const Home&);//copy构造，只有声明，也就没有必要写参数名称（在传值只需要其类型时也经常这样写）
	Home& operator=(const Home&);//copy assignment，只有声明
	//...
};
//此方法会在使用者试图拷贝对象时被编译器阻止；但如果自己在成员函数或friend函数之内无意中调用了，会在链接时报错
//将链接期错误转移至编译期通常是更好的，可以定义一个专门的base class
class Uncopyable {
protected:
	Uncopyable() {};
	~Uncopyable() {};//允许构造和析构
private:
	Uncopyable(const Uncopyable&);
	Uncopyable& operator=(const Uncopyable&);//阻止copying
};
//当有类想要达到上面说的效果，就可以继承该类（不一定以public继承）
//
//对于这个条款，应该是C++98的做法，C++11的delete应该才是最佳答案，他明确表示了不可拷贝
class UncopyableCpp11 {
protected:
	UncopyableCpp11() {};
	~UncopyableCpp11() {};
	UncopyableCpp11(const UncopyableCpp11&) = delete;
	UncopyableCpp11& operator=(const UncopyableCpp11&) = delete;
};


//条款07：为多态基类声明virtual析构函数
//Declare destructors virtual in polymorphic base classes


int main()
{
	std::cout << "Hello World!\n";
}