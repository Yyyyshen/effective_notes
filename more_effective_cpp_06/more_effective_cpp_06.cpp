// more_effective_cpp_06.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>

/**
 * 杂项
 */

using namespace std;

//条款32：在未来时态下发展程序
// 
//提高可扩展、可维护性，使程序更健壮
// 提供完整的classes体系，即使某些部分暂时用不到
// 设计接口，使有利于共同的操作行为，阻止共同的错误
// 尽量使代码一般化（泛化）
//


//条款33：将非尾端类（non-leaf classes）设计为抽象类（abstract classes）
// 
//将你的具体类派生自既存的具体类
//试着在程序库继承体系中找一个更高层的抽象类，其中有需要的大部分功能，然后继承它
//以你所希望继承的哪个程序库类来实现新类
//


//条款34：如何在同一个程序中结合C++和C
// 
//确定C++和C编译器产出兼容的目标文件（object file）
//将双方都使用的函数声明为extern "C"
//如果可能，尽量在C++中写main
//总是以delete删除new的内存；以free释放malloc的内存
//将两个语言间的数据结构传递限制于C所能了解的形式
//


//条款35：让自己习惯于标准C++语言
// 
//一些特性：RTTI、namespace、bool、关键词mutable和explicit、enums
//扩充了templates弹性：允许member template
//强化了异常处理机制
//修改了内存分配例程：加入operator new[]/delete[]
//增加了新的转型形式
// 
//支持C标准库、strings、本地化、I/O、数值应用、容器和算法
//
//熟悉STL
//

int main()
{
    std::cout << "Hello World!\n";
}
