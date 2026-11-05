#pragma once
#include <iostream>
#include <string>
#include <vector>

/* ==========================================================================
 *  Student —— 学生读者（与 Resource 平行的独立类，被 Library 组合使用）
 * --------------------------------------------------------------------------
 *  属性：学号、姓名、班级、已借资源编号列表。
 *  说明：这里保存的是"资源编号"，而不是资源对象本身，
 *        这样学生和资源之间只用一个字符串联系起来，耦合度低。
 *  实验五：为了让 Student 也能放进 Repository<T> 类模板，这里补上了
 *          operator==(const std::string&) —— 模板对类型参数的要求之一。
 *  实验六：新增 save() / load()，把学生对象也存进文件。
 * ========================================================================== */
class Student
{
private:
    std::string              id;          // 学号
    std::string              name;        // 姓名
    std::string              className;   // 班级
    std::vector<std::string> borrowedIds; // 当前借在手上、尚未归还的资源编号

public:
    Student();
    Student(const std::string& id, const std::string& name, const std::string& className);

    const std::string& getId()   const { return id; }
    const std::string& getName() const { return name; }

    void input();
    void display(std::ostream& os = std::cout) const;

    // ---------------- 运算符重载 ----------------
    friend std::ostream& operator<<(std::ostream& os, const Student& s);
    Student& operator+=(const std::string& resourceId);  // stu += "B001" 表示借入
    Student& operator-=(const std::string& resourceId);  // stu -= "B001" 表示归还
    bool operator==(const std::string& rhsId) const { return id == rhsId; } // 按学号比较

    void addBorrow(const std::string& resourceId);
    bool removeBorrow(const std::string& resourceId);
    bool hasBorrowed(const std::string& resourceId) const;
    int  borrowCount() const { return static_cast<int>(borrowedIds.size()); }

    // ---------------- 实验六：对象的存储与读取 ----------------
    void save(std::ostream& os) const;    // 写一行：学号|姓名|班级|已借编号|…
    bool load(std::istream& is);
    bool loadFromLine(const std::string& line);
};
