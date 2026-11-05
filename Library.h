#pragma once
#include <string>
#include "Repository.h"
#include "Resource.h"
#include "Student.h"

/* ==========================================================================
 *  Library —— 图书馆【组合类】
 * --------------------------------------------------------------------------
 *  【实验五】组合关系升级：图书馆"含有"若干资源对象与若干学生对象，
 *           现在不再用对象数组，而是用【类模板】Repository<T> 组织：
 *               Repository<Resource> resources;   // 基类指针容器 -> 支持多态
 *               Repository<Student>  students;
 *           Repository 内部就是 std::vector<T*>，把 push_back / emplace_back /
 *           begin / end / erase / size / clear 全部封装成了成员函数，
 *           对象的 new / delete 也由它统一负责。
 *
 *  为什么容器里存的是 Resource*（基类指针）而不是 Book / Magazine / EResource？
 *    因为基类指针可以指向任意派生类对象，一个容器就能装下所有类型的资源，
 *    遍历时统一写代码，具体行为由虚函数在运行时决定 —— 这就是【多态】。
 *
 *  【实验六】新增文件存取：saveToFile() / loadFromFile()。
 *           启动时自动读档，退出时自动存档，对象不再"用完就没了"。
 *
 *  本类还负责：菜单显示、八大功能的实现，以及所有输入提示的输出。
 * ========================================================================== */
class Library
{
private:
    Repository<Resource> resources;   // 资源对象仓库（类模板实例化，多态容器）
    Repository<Student>  students;    // 学生对象仓库（同一个类模板的另一次实例化）

    std::string dataFile;             // 数据文件名（实验六）
    static const int MAX_BORROW = 5;  // 每名学生最多同时借阅的件数

    // ---------------- 内部辅助函数 ----------------
    void loadSampleData();                            // 载入示例数据
    static void printMenu();

    // ---------------- 八大功能 + 附加功能 ----------------
    void addResource();        // 1. 增加新的图书或资源
    void increaseCount();      // 2. 增加已有图书或资源的数量
    void showOne() const;      // 3. 显示某一图书或资源情况
    void showAll() const;      // 4. 显示所有图书或资源情况
    void decreaseCount();      // 5. 减少已有图书或资源的数量
    void removeResource();     // 6. 删除某一图书或资源
    void borrow();             // 7. 学生借阅
    void giveBack();           // 8. 学生归还
    void registerStudent();    // 附加1：注册学生读者
    void showStudents() const; // 附加2：显示学生借阅情况
    void demoPolymorphism() const;    // 附加3：虚函数多态演示
    void demoVectorAndTemplate() const; // 实验五：vector 与模板演示
    void demoFileStream() const;        // 实验六：文件流读写演示

public:
    explicit Library(const std::string& dataFile = "library_data.txt");
    ~Library();

    // 禁止拷贝：仓库里是裸指针，浅拷贝会导致 delete 两次
    Library(const Library&)            = delete;
    Library& operator=(const Library&) = delete;

    // ---------------- 实验六：对象的存储与读取 ----------------
    bool saveToFile(const std::string& path) const;   // 对象 -> 文件
    bool loadFromFile(const std::string& path);       // 文件 -> 对象
    bool saveToFile()  const { return saveToFile(dataFile); }
    bool loadFromFile()      { return loadFromFile(dataFile); }
    const std::string& dataFilePath() const { return dataFile; }

    void run();                 // 启动菜单循环
};
