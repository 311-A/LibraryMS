#pragma once
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

/* ==========================================================================
 *                        【实验五】模板与 vector 容器
 * --------------------------------------------------------------------------
 *  一、为什么要有模板？
 *    Resource、Student 两类对象都要"放进一个容器里，按编号查找、删除、遍历、
 *    清空"。逻辑完全一样，只是元素类型不同。没有模板就得把同样的代码抄两遍；
 *    有了模板，把"类型"也当作参数传进去，一份代码服务所有类型 —— 这就是
 *    泛型（类属）程序设计：一个类模板 = 一组类的抽象。
 *
 *  二、为什么用 vector 而不用对象数组？
 *    对象数组 Resource* arr[100] 的弊端：
 *      1) 长度写死，超了就越界；
 *      2) 要自己维护"当前有多少个"；
 *      3) 删除中间元素要手动把后面的往前挪。
 *    vector 把这些都封装好了：push_back / emplace_back 尾部增加、
 *    erase 删除、size 取长度、begin/end 拿迭代器遍历、clear 清空。
 *
 *  三、vector 里放【对象】还是放【对象指针】？（实验五思考题1）
 *    放对象  = 值语义：内存自动管理，不用 delete；
 *              但会发生"对象切片"——把派生类对象塞进去时，派生类特有的成员
 *              会被切掉，而且抽象类根本不能实例化，也就放不进去。
 *    放指针 = 引用语义：保留完整对象、支持多态（一个容器装下所有派生类）；
 *              代价是必须自己 delete（本类在析构里统一回收）。
 *    => 需要多态就必须放基类指针，本程序选的就是这种。
 *
 *  四、基类和派生类对象能放进同一个 vector 吗？（实验五思考题2）
 *    不能直接放对象（会切片）；只能放【基类指针】vector<Resource*>，
 *    这正是多态容器的做法，见 Library 里的 Repository<Resource>。
 * ========================================================================== */

/* ---------------------- 函数模板：与类无关的通用算法 ---------------------- */

// 在"存放指针的 vector"里按编号查找，返回下标；找不到返回 -1。
// 要求：类型 T 必须重载了 operator==(const std::string&)。
template <typename T>
int findIndexById(const std::vector<T*>& vec, const std::string& id)
{
    for (std::size_t i = 0; i < vec.size(); ++i)
        if (vec[i] != nullptr && *vec[i] == id)
            return static_cast<int>(i);
    return -1;
}

// 用迭代器遍历并输出。要求：类型 T 必须重载了 operator<<(ostream&, const T&)。
template <typename T>
void printAll(const std::vector<T*>& vec, std::ostream& os = std::cout)
{
    typename std::vector<T*>::const_iterator it;
    for (it = vec.begin(); it != vec.end(); ++it)     // begin() / end() 拿到迭代器
        os << **it;
}

// 释放容器中所有对象并清空（vector 只负责存指针，对象本身要手动 delete）
template <typename T>
void deleteAll(std::vector<T*>& vec)
{
    for (std::size_t i = 0; i < vec.size(); ++i)
        delete vec[i];
    vec.clear();                                       // clear() 清空所有元素
}

/* -------------------- 类模板：把 vector 包装成"对象仓库" -------------------- */
// 对 T 的要求：① 提供 getId()；② 重载 operator==(const std::string&)；
//             ③ 析构函数可访问（Resource 是虚析构，delete 基类指针才安全）。
template <typename T>
class Repository
{
private:
    std::vector<T*> items;        // 真正干活的是这个 vector

public:
    typedef typename std::vector<T*>::iterator       iterator;
    typedef typename std::vector<T*>::const_iterator const_iterator;

    Repository() {}
    ~Repository() { clear(); }    // 析构时统一回收，避免内存泄漏

    // 仓库里装的是裸指针，默认拷贝是浅拷贝 -> 会 delete 两次，必须禁止
    Repository(const Repository&)            = delete;
    Repository& operator=(const Repository&) = delete;

    // ---------- 增 ----------
    void push(T* p)    { if (p) items.push_back(p); }      // push_back：尾部放入
    void emplace(T* p) { if (p) items.emplace_back(p); }   // emplace_back：尾部就地放入

    // ---------- 查 ----------
    std::size_t size()  const { return items.size(); }     // size()：元素个数
    bool        empty() const { return items.empty(); }

    T* at(std::size_t i) const { return (i < items.size()) ? items[i] : nullptr; }

    // 用 begin()/end() 迭代器遍历查找
    T* findById(const std::string& id) const
    {
        for (const_iterator it = items.begin(); it != items.end(); ++it)
            if (**it == id)
                return *it;
        return nullptr;
    }

    // 复用上面的【函数模板】做查找 —— 类模板内部调用函数模板
    int indexOf(const std::string& id) const { return findIndexById<T>(items, id); }

    bool contains(const std::string& id) const { return indexOf(id) >= 0; }

    // ---------- 删 ----------
    // erase() 会返回"被删元素的下一个元素"的迭代器，所以循环中要用它接住返回值，
    // 否则迭代器失效。这也是实验步骤里要求的 erase 用法。
    bool eraseById(const std::string& id)
    {
        for (iterator it = items.begin(); it != items.end(); )
        {
            if (**it == id)
            {
                delete *it;                 // 先释放对象
                it = items.erase(it);       // 再从 vector 里移除，并接住返回的迭代器
                return true;
            }
            else
            {
                ++it;
            }
        }
        return false;
    }

    void eraseAt(std::size_t i)
    {
        if (i >= items.size()) return;
        delete items[i];
        items.erase(items.begin() + static_cast<std::ptrdiff_t>(i));
    }

    // ---------- 清空 ----------
    // 调用【函数模板】deleteAll：delete 所有对象 + clear() 清空容器
    void clear() { deleteAll<T>(items); }

    // ---------- 遍历接口：把迭代器暴露出去，外面就能用范围 for ----------
    iterator       begin()       { return items.begin(); }
    iterator       end()         { return items.end();   }
    const_iterator begin() const { return items.begin(); }
    const_iterator end()   const { return items.end();   }
};
