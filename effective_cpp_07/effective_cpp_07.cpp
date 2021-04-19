// effective_cpp_07.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>

/**
 * 6.继承与面向对象设计
 * Inheritance and Object-Oriented Dsign
 */

using namespace std;

//条款32：确定你的public继承塑模出is-a关系
//Make sure public inheritance models "is-a"
// 
//面向对象中，最重要的是：public inheritance 意味"is-a"的关系
//public继承意味着子类是一种父类，父类身上的每一件事也一定适用于子类，父类更加一般化
//如果子类中有一些父类中有但子类不该有的功能，就不一定适合使用public继承，例如鸟类作为基类定义了飞的动作，而企鹅继承鸟类但不会飞
//但可以通过设计来区分，例如不将飞行的动作定义于鸟类，而再定义一个“会飞的鸟”类去定义飞行
//


//条款33：避免遮掩继承而来的名称
//Avoid hiding inherited names
//
//Derived classes内的名称会遮掩base classes内的名称
//想要使用base中被遮掩的名称，可以使用using声明式或转交函数
//


//条款34：区分接口继承和实现继承
//Differentiate between inheritance of interface and inheritance of implementation
// 
//有时，需要派生类只继承成员函数的接口（声明）；有时又需要同时继承函数的接口和实现（定义），又希望能override他们锁继承的实现（或不希望override）
// 
//成员函数接口总是会被继承
//声明一个纯虚函数的目的是为了让派生类只继承函数接口
// （但纯虚函数也是可以提供定义的，调用时的唯一途径是明确指出class名称。例如：Derived* p; p->Base::f();）
//声明一个非纯虚函数的目的是让派生类继承该函数的接口和缺省实现
//声明一个non-virtual函数目的是为了让派生类继承函数的接口及一份强制性实现
//


//条款35：考虑virtual函数以外的其他选择
//Consider alternatives to virtual functions
// 
//使用non-virtual interface（NVI）手法，是template method设计模式的一种特殊形式；它以public non-virtual成员函数包覆较低访问性的virtual函数
//将virtual函数替换为函数指针成员变量
//以std::function成员变量替换virtual函数，比函数指针更灵活
//将继承体系内的virtual函数替换为另一个继承体系内的virtual函数
//


//条款36：绝不重新定义继承而来的non-virtual函数
//Never redefine an inherited non-virtual function
// 
//基类有一个non-virtual成员函数mf，其派生类一定会继承mf的接口和实现，如果派生类重新定义mf，设计便有矛盾
//如果真的有必要重新定义，那其实就该把mf声明为virtual函数
//所以，如题
//


//条款37：绝不重新定义继承而来的缺省参数值
//Never redefine a function's inherited default parameter value
// 
//在继承一个带有缺省参数值的virtual函数的条件下讨论一个问题
//virtual函数是动态绑定，而缺省参数值却是静态绑定
class shape {
public:
	enum shape_color { white, black };
	virtual void draw(shape_color color = white) const = 0;
};
class rectangle :public shape {
public:
	virtual void draw(shape_color color = black) const {}//使用不同的缺省参数值
};
void test_default() {
	shape* pr = new rectangle;
	pr->draw();//相当于调用rectangle::draw(shape::white);
}
//C++为了执行效率，放弃了将缺省参数值动态绑定
//也是为了编译器实现上的简易度
//


//条款38：通过复合塑模出has-a或“根据某物实现出”
//Model "has-a" or "is-implemented-in-terms-of" through composition
// 
//某种类型的对象内含其他类型的对象，就是复合关系
class A {};
class B {
private:
	A a;//符合关系（分层、内嵌、聚合、内嵌等说法），意味"has-a"
};
//
//另外如set可基于list等基础容器实现出来，那么set中有一个list对象这样的设计是"is-implemented-in-terms-of"
// 
//在应用于复合意味"has-a"（有一个
//在实现域复合意味"is-implemented-in-terms-of"（根据某物实现出）
//


//条款39：明智而谨慎的使用private继承
//Use private inheritance judiciously
// 
//在public继承中，有时为了函数调用，编译器会在必要时刻将派生类暗自转为基类
//若使用private继承，就并不意味这is-a关系了
class person {};
class student : private person {};
void eat(const person& p) {}
void study(const student& s) {}
void test_private() {
	person p;
	student s;
	eat(p);
	//eat(s);//error，不允许对不可访问基类转换
}
//也就是说，如果classes之间为private继承关系，编译器不会自动将derived class对象转为base classes对象
//还有一个规则就是，由private base class继承而来的所有成员，在derived class中都会变成private属性（即使在base中原本是public或protected）
//
//private继承意味着"is-implemented-in-terms-of"（根据某物实现出）
//但尽可能的使用复合（上一条款提到）来实现这种关系
//当protected成员和virtual函数牵扯进来时，才可能使private成为必要选择
// 
//例如STL中，一些做类型定义用的empty classes，虽然没有non-static成员变量，但有很多typedefs、enums、static成员变量或non-virtual函数
//这样的private继承很少增加derived classes大小
//这种方式叫做EBO（empty base optimization）空白基类最优化
//


//条款40：明智而谨慎的使用多重继承
//Use multiple inheritance judiciously
// 
//多继承造成的问题
// 一个以上的base classes中有同名函数，会导致歧义（可明确指出是哪个类的函数）
// 钻石型多重继承，一个类的多个父类又拥有同一个父类，中间层的类在继承父类时应该使用virtual继承（但要付出一些效率代价）
// 
//多重继承的正当用途
// public继承某个interface class
// private继承某个协助实现的class
// 两相组合
//


int main()
{
	std::cout << "Hello World!\n";
}
