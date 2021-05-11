// more_effective_cpp_05.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>

/**
 * 技术
 */

using namespace std;

//条款25：将constructor和non-member functions虚化
// 
//构造无法真正被虚化，需要间接处理
//有一种特别的virtual copy constructor被广泛运用
// 返回一个指针，指向其调用者的一个新副本
// 通常命名为copySelf或cloneSelf
class component {
public:
	//virtual copy ctor
	virtual component* clone() const = 0;
	//virtual non-member functions
	virtual ostream& print(ostream& s) const = 0;
};
class text_block :public component {
public:
	virtual text_block* clone() const { return new text_block(*this); }
	virtual ostream& print(ostream& s) const { return s; }
};
//clone本质上就是通过*this对象调用了真正的copy constructor
// 
//non-member functions也同样，间接调用
inline ostream& operator<<(ostream& s, const component& c) {
	return c.print(s);
}
//


//条款26：限制某个class所能产生的对象数量
// 
//限制在0或1个对象
// private ctor
// 单例模式
// 
//限制有限个对象
// 在产生对象的地方计数，达到计数时抛异常
// 设计一个用来计算对象个数的base class
template<class being_counted>
class counted {
public:
	class too_many_objects {};//用于抛出异常
	static int object_count() { return num_objects; }
protected:
	counted();
	counted(const counted& rhs);
	~counted() { --num_objects; }

private:
	static int num_objects;
	static const size_t max_objects;
	void init();//封装，避免两个构造中出现重复代码
};
template<class being_counted>
counted<being_counted>::counted() { init(); }
template<class being_counted>
counted<being_counted>::counted(const counted<being_counted>&) { init(); }
template<class being_counted>
void counted<being_counted>::init() {
	if (num_objects >= max_objects) throw too_many_objects();
	++num_objects;
}
//使用时以private方式继承
//


//条款27：要求（或禁止）对象产生于heap之中
// 
//有时想要让对象必须产生于heap之中
// 也就是让对象必须通过new产生
// 简单处理可以将ctor和dtor置为private
// 比较好的办法是将dtor声明为private，然后提供伪dtor函数，构造仍为public
class widget {
public:
	widget() {};
	//pseudo dtor
	void destroy() const { delete this; }
private:
	~widget() {};
};
void test_widget() {
	//widget w;//error,无法自动析构
	widget* pw = new widget;//迫使调用者这样来用
	//...
	//delete pw;//error,企图调用private的dtor
	pw->destroy();
}
//想要禁止产生于heap的话，就将new和delete声明为private
//


//条款28：Smart Pointers（智能指针）
// 
template<class T>
class smart_ptr {
public:
	smart_ptr(T* real_ptr = 0) :pointee(real_ptr) {};
	smart_ptr(const smart_ptr& rhs);
	~smart_ptr();

	smart_ptr& operator=(const smart_ptr& rhs);
	T* operator->() const;
	T& operator*() const;
private:
	T* pointee;
};
template<class T>
smart_ptr<T>::~smart_ptr() {
	//if (*this owns * pointee)//伪代码
	delete pointee;
}
template<class T>
T& smart_ptr<T>::operator*() const {
	//perform "smart pointer" processing;//伪代码，确保pointee有效
	return *pointee;
}
template<class T>
T* smart_ptr<T>::operator->() const {
	//perform "smart pointer" processing;//伪代码，确保pointee有效
	return pointee;
}
//比较需要注意的是复制和赋值，不应当产生多个智能指针指向同一个对象
// 除了简单的禁止赋值和复制，可以处理这两个操作，令其转移对象拥有权
template<class T>
smart_ptr<T>& smart_ptr<T>::operator=(const smart_ptr<T>& rhs) {
	if (this == &rhs)
		return *this;//自我赋值，不做任何处理
	delete pointee;
	pointee = rhs.pointee;//转移拥有权
	rhs.pointee = 0;//rhs不再拥有任何东西
	return *this;
}
//


//条款29：Reference counting（引用计数）
// 
//这项技术允许多个等值对象共享同一实值
// 简化heap objects周边工作
// 类似auto_ptr的类可以协助此类工作
// 可以消除记录对象拥有权的复合
// 另一个动机是实现一种常识
// 许多对象若拥有相同值，存储多次是不合理的，共享即可
// 
//一个引用计数基类
class rc_object {
public:
	rc_object();
	rc_object(const rc_object& rhs);
	rc_object& operator=(const rc_object& rhs);
	virtual ~rc_object() = 0;
	void add_reference();
	void remove_reference();
	void mark_unshareable();
	bool is_shareable() const;
	bool is_shared() const;
private:
	int ref_count;
	bool shareable;
};
rc_object::rc_object() :ref_count(0), shareable(true) {}
rc_object::rc_object(const rc_object& rhs) : ref_count(0), shareable(true) {}
rc_object& rc_object::operator=(const rc_object& rhs) { return *this; }
rc_object::~rc_object() {}//虚析构也必须实现出来，但什么都不做
void rc_object::add_reference() { ++ref_count; }
void rc_object::remove_reference() { if (--ref_count == 0) delete this; }
void rc_object::mark_unshareable() { shareable = false; }
bool rc_object::is_shareable() const { return shareable; }
bool rc_object::is_shared() const { return ref_count > 1; }
//
//自动操作引用次数
// 上面的类还是需要手动操作add或remove操作
// 我们希望把这些动作移至可复用的class内
// 使用者就不需要考虑细节
template<class T>
class rc_ptr {
public:
	rc_ptr(T* real_ptr = 0);
	rc_ptr(const rc_ptr& rhs);
	~rc_ptr();
	rc_ptr& operator=(const rc_ptr& rhs);
	T* operator->() const;
	T& operator*() const;
private:
	T* pointee;
	void init();
};
template<class T>
rc_ptr<T>::rc_ptr(T* real_ptr /* = 0 */) :pointee(real_ptr) { init(); }
template<class T>
rc_ptr<T>::rc_ptr(const rc_ptr& rhs) : pointee(rhs.pointee) { init(); }
template<class T>
void rc_ptr<T>::init() {
	if (pointee == 0) return;
	if (pointee->is_shareable() == false) pointee = new T(*pointee);//不可共享则复制一份
	pointee->add_reference();
}
template<class T>
rc_ptr<T>::~rc_ptr() {
	if (pointee) pointee->remove_reference();
}
template<class T>
rc_ptr<T>& rc_ptr<T>::operator=(const rc_ptr& rhs) {
	if (pointee != rhs.pointee) {
		if (pointee)
			pointee->remove_reference();//移除当前实值引用次数
		pointee = rhs.pointee;
		init();
	}
	return *this;
}
template<class T>
T* rc_ptr<T>::operator->() const { return pointee; }
template<class T>
T& rc_ptr<T>::operator*() const { return *pointee; }
//
//可结合两个类实现一个带引用计数的应用类
//


//条款30：Proxy classes（替身类、代理类）
// 
//例如二维数组，可以分解为嵌套的一维数组
template<class T>
class array_2d {
public:
	class array_1d {
	public:
		T& operator[](int index);
	};
	array_1d operator[](int index);
};
//


//条款31：让函数根据一个以上的对象类型来决定如何虚化
// 
//
//


int main()
{
	std::cout << "Hello World!\n";
}
