// effective_stl_06.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>

/**
 * 6 函数子、函数子类、函数及其他
 */

using namespace std;

//第38条：遵循按值传递的原则来设计函数子类
// 
//C和C++的标准库函数都遵循这一规则，函数指针是按值传递的
//
//由于这个原因，为了确保正常工作
// 函数对象必须尽可能的小，否则会开销很大
// 函数对象必须不是多态的，不得使用虚函数，避免传递过程中产生剥离问题
//
//然而并不是所有函数子都小巧和单态，需要一个两全其美的办法
// 将所需的数据和虚函数从函数子中分离出来，放到一个新类中
// 然后在函数子的类中包含一个指针指向这个新类对象（pimpl手法）
// 另外，可以使用智能指针这样的辅助类
// 
//本条款的意图，主要在于，在STL中传参或返回值总是按值传递，所以需要谨慎处理拷贝构造函数，就意味着小巧和非多态
//


//第39条：确保判别式是“纯函数”
// 
//一个判别式是返回值为bool（或可以隐式转换为bool）类型的函数
//一个纯函数是指返回值仅仅依赖于其参数的函数
// 例如，func为一个纯函数，参数为x，y两对象
// 只有当x，y发生变化时f(x,y)返回值才发生变化
//判别式类时一个函数子类，它的operator()函数是一个判别式，返回bool类型
// STL中凡是能接受判别式的地方，也可以接受一个判别式类的对象
//接受函数子的STL算法可能会先创建函数子的副本，存放起来待以后使用，所以要求必须是纯函数
// 如果违反这个约束，可能会产生的后果如以下例子
class widget {};
class bad_predicate :public unary_function<widget, bool> {
public:
	bad_predicate() :times_called(0) {} //初始化为0
	bool operator()(const widget&) { return ++times_called == 3; } //忽略参数，单纯以调用次数来判别，不符合纯函数约束
private:
	size_t times_called;
};
void test_bad_predicate() {
	vector<widget> vec_w;
	vec_w.erase(remove_if(vec_w.begin(), vec_w.end(), bad_predicate()), vec_w.end());
	//表面上，是删除第三个元素，然而同时会删除第6个
}
//从remove_if通常的实现方式看这个问题
template<typename forward_iterator, typename predicate>
forward_iterator y_remove_if(forward_iterator begin, forward_iterator end, predicate p) {
	begin = find_if(begin, end, p);
	if (begin == end) return begin;
	else {
		forward_iterator next = begin;
		return remove_copy_if(++next, end, begin, p);
	}
	//p被在两个地方使用，两次传递中都是按值传递，所以产生了两个内部成员为0的对象
	//即使find_if调用3次之后，到remove_copy_if，还是从0开始计数三次才生效
}
//最简单的约束方法，是将operator()函数声明为const，确保不改变数据成员（其实还能改变mutable成员和非const局部static对象等）
// 
//所以，判别式应该是纯函数
//


//第40条：若一个类是函数子，则应使它可以配接
//(std::unary_function和std::binary_function不建议使用，并且在新标准中已经删除，理解即可）
//
//
//


//第41条：理解ptr_fun、mem_fun和mem_fun_ref的来由
// 
//一个对象x，和一个函数f，想要在x上调用f，有三种方式
// f(x);	//非成员函数
// x.f();	//成员函数
// px->f(); //pf为指向x的指针，f为成员函数
void test_widget(widget& w) {}
void test_for_each() {
	//现假设有一个存放widget容器，需要逐个检查
	vector<widget> vec_w;
	for_each(vec_w.begin(), vec_w.end(), test_widget);//第一种形式函数调用，可以编译；这是传入了一个真正的函数，所以没必要调整语法形式
	//是否使用ptr_fun，在这里不影响，加不加都可以编译
	for_each(vec_w.begin(), vec_w.end(), ptr_fun(test_widget));//如果无法确定该不该加，可以总是使用它，性能也没有太大损失，或者是，编译器提醒时才使用它
}
//按照上面三种想法，如果widget本身支持自检，也就是有一个成员函数，应该支持如下写法
class widget2 {
public:
	void test() {}
};
void test_for_each2() {
	vector<widget2> vec_w2;
	//for_each(vec_w2.begin(), vec_w2.end(), &widget2::test);//第二种形式的函数调用，无法通过编译
	/* for_each摘录
		template <class _InIt, class _Fn>
		_CONSTEXPR20 _Fn for_each(_InIt _First, _InIt _Last, _Fn _Func) { // perform function for each element [_First, _Last)
			_Adl_verify_range(_First, _Last);
			auto _UFirst      = _Get_unwrapped(_First);
			const auto _ULast = _Get_unwrapped(_Last);
			for (; _UFirst != _ULast; ++_UFirst) {
				_Func(*_UFirst); //这里可以看出，就是非成员函数类型调用
			}
			return _Func;
		}
	*/
	for_each(vec_w2.begin(), vec_w2.end(), mem_fun_ref(&widget2::test));//转换成for_each适用的调用形式
	/* mem_fun_ref_t摘录
		template <class _Result, class _Ty>
		class mem_fun_ref_t : public unary_function<_Ty, _Result> { // functor adapter (*left.*pfunc)(), non-const *pfunc
		public:
			explicit mem_fun_ref_t(_Result (_Ty::*_Pm)()) : _Pmemfun(_Pm) {}

			_Result operator()(_Ty& _Left) const {
				return (_Left.*_Pmemfun)();//转换成 . 成员函数的调用形式
			}

		private:
			_Result (_Ty::*_Pmemfun)(); // the member function pointer
		};
	*/


	vector<widget2*> vec_pw2;
	//for_each(vec_pw2.begin(), vec_pw2.end(), &widget2::test);//第三种形式，也无法通过编译
	for_each(vec_pw2.begin(), vec_pw2.end(), mem_fun(&widget2::test));//mem_fun将第三种转换成for_each适配的形式，可以编译
	/* mem_fun_t摘录
		template <class _Result, class _Ty>
		class mem_fun_t : public unary_function<_Ty*, _Result> { // functor adapter (*p->*pfunc)(), non-const *pfunc
		public:
			explicit mem_fun_t(_Result (_Ty::*_Pm)()) : _Pmemfun(_Pm) {}

			_Result operator()(_Ty* _Pleft) const {
				return (_Pleft->*_Pmemfun)(); //当调用到这个对象的operator()时，会把参数作为左值来调用构造该对象的成员函数指针，也就转换成了for_each适用的调用形式
			}

		private:
			_Result (_Ty::*_Pmemfun)(); // the member function pointer
		};
	*/
}
//


//第42条：确保less<T>与operator<具有相同的语义
// 
//假设一个对象，有两个成员变量
class widget3 {
public:
	size_t a() const { return a_; }
	size_t b() const { return b_; }
private:
	size_t a_;
	size_t b_;
};
bool operator<(const widget3& lhs, const widget3& rhs) {
	return lhs.a() < rhs.a();
}
//在正常排序时，按照a大小进行排序，也就是上面operator<的重载
//在特殊情况下，想要根据b大小进行排序
// 默认比较函数一般时less<T>函数，而less<T>默认是用operator<完成
// 一个有问题的想法是，特化less<widget>，切断其与operator<之间的关系
template<>
struct std::less<widget3> :public std::binary_function<widget3, widget3, bool> {
	bool operator()(const widget3& lhs, const widget3& rhs) const {
		return lhs.b() < rhs.b();
	}
};
//这其实会违背大部分人的认知常理，并且也并不需要这么做
// 完全可以自定义函数子，替换默认比较函数即可
//


int main()
{
	std::cout << "Hello World!\n";
}
