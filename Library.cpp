#include "Library.h"
#include "Book.h"
#include "Magazine.h"
#include "EResource.h"
#include "FileUtil.h"
#include "InputUtil.h"
#include "ResourceFactory.h"

#include <fstream>

/* ==========================================================================
 *  演示"对象切片"用的两个小类（只在本文件里可见）
 * --------------------------------------------------------------------------
 *  为什么要单独造这两个类？
 *    因为 Resource 是抽象类，没法直接放进 vector<Resource> 里做实验；
 *    用一对最简单的基类/派生类，就能亲眼看到"放对象会切片"的后果。
 * ========================================================================== */
namespace
{
    struct SliceBase
    {
        int         a;
        std::string tag;
        SliceBase() : a(1), tag("基类自己的成员") {}
        virtual ~SliceBase() {}
        virtual const char* who() const { return "SliceBase（基类）"; }
    };

    struct SliceDerived : public SliceBase
    {
        int         b;
        std::string extra;
        SliceDerived() : b(99), extra("派生类特有的成员") {}
        const char* who() const override { return "SliceDerived（派生类）"; }
    };
}

/* ========================= 构造 / 析构：new 与 delete ========================= */
// 实验六：构造时先试着读档；读不到（第一次运行）就载入示例数据并立刻存档。
Library::Library(const std::string& file)
    : dataFile(file)
{
    if (!loadFromFile(dataFile))
    {
        std::cout << "  未发现数据文件「" << dataFile << "」，本次载入示例数据。\n";
        loadSampleData();
        saveToFile(dataFile);
    }
}

// 析构函数：真正的 delete 由 Repository<T> 的析构函数完成（clear -> deleteAll）
Library::~Library()
{
    std::cout << "\n系统已退出，vector 已清空，所有资源对象与学生对象内存均已释放。\n";
}

// 示例数据：让程序第一次运行就有东西可看
void Library::loadSampleData()
{
    resources.push(new Book("B001", "C++ Primer（第5版）", "人民邮电出版社", 2013, 8, 1,
                            "Stanley B. Lippman", "978-7-115-35389-1", "TP312C++"));
    resources.push(new Book("B002", "数据结构（C语言版）", "清华大学出版社", 2021, 12, 0,
                            "严蔚敏", "978-7-302-14751-0", "TP311.12"));
    resources.push(new Magazine("M001", "计算机学报", "科学出版社", 2026, 5, 1,
                                "0254-4164", "2026年第10期", "月刊"));
    resources.push(new Magazine("M002", "软件学报", "科学出版社", 2026, 3, 0,
                                "1000-9825", "2026年第9期", "月刊"));
    resources.push(new EResource("E001", "C++ 标准库参考手册", "电子工业出版社", 2024, 10, 1,
                                 "PDF", 12.5, "https://cppreference.cn/"));
    resources.push(new EResource("E002", "中国知网·计算机专辑", "同方知网", 2025, 30, 5,
                                 "在线数据库", 0.0, "https://www.cnki.net/"));

    Student* s1 = new Student("S202301", "张三", "计科2301");
    s1->addBorrow("B001");
    students.push(s1);

    Student* s2 = new Student("S202302", "李四", "计科2301");
    s2->addBorrow("M001");
    s2->addBorrow("E001");
    students.push(s2);
}

/* ====================== 通过【基类引用】调用虚函数 ====================== */
static void showByReference(const Resource& r)
{
    r.display();
}

/* ============================ 功能1：新增资源 ============================ */
void Library::addResource()
{
    std::cout << "\n================ 功能1：增加新的图书 / 资源 ================\n"
              << "  请选择要新增的资源类型：\n"
              << "    1. 一般图书\n"
              << "    2. 杂志\n"
              << "    3. 电子资源\n"
              << "    0. 取消，返回上级菜单\n";

    int type = InputUtil::readInt("  请选择（0-3）：", 0, 3);
    if (type == 0)
    {
        std::cout << "  已取消。\n";
        return;
    }

    Resource* r = nullptr;                 // 基类指针 -> 指向派生类对象
    switch (type)
    {
    case 1: r = new Book();     break;
    case 2: r = new Magazine(); break;
    case 3: r = new EResource(); break;
    }

    std::cout << "\n  请依次录入资源信息（先公共信息，再该类特有信息）：\n";
    r->input();                            // 一次读完：基类公共字段 + 子类特有字段

    if (resources.contains(r->getId()))    // 编号重复，拒绝添加
    {
        std::cout << "  编号为 " << r->getId() << " 的资源已存在，添加失败！\n";
        delete r;                          // 用 delete 释放刚刚 new 出来的对象
        return;
    }

    resources.push(r);                     // push_back 放入仓库，生命周期交给 Repository
    std::cout << "  添加成功！当前资源总数：" << resources.size() << " 项。\n";
}

/* ======================== 功能2：增加已有资源数量 ======================== */
void Library::increaseCount()
{
    std::cout << "\n================ 功能2：增加已有图书 / 资源的数量 ================\n";
    std::string id = InputUtil::readString("  请输入资源编号（输入 0 返回）：");
    if (id == "0") return;

    Resource* r = resources.findById(id);
    if (r == nullptr) { std::cout << "  未找到该编号的资源！\n"; return; }

    std::cout << "  当前信息：" << r->getName()
              << "，馆藏 " << r->getTotal()
              << "，已借出 " << r->getBorrowed() << '\n';

    int n = InputUtil::readInt("  请输入本次增加的数量：", 1, 100000);
    r->addCount(n);
    std::cout << "  增加成功！" << r->getName()
              << " 现有馆藏总数：" << r->getTotal() << '\n';
}

/* ====================== 功能3：显示某一图书/资源情况 ====================== */
void Library::showOne() const
{
    std::cout << "\n================ 功能3：显示某一图书 / 资源情况 ================\n";
    std::string key = InputUtil::readString("  请输入资源编号或名称关键字：");

    bool found = false;
    // 用 begin() / end() 迭代器遍历仓库（实验五要求的遍历方式之一）
    for (Repository<Resource>::const_iterator it = resources.begin();
         it != resources.end(); ++it)
    {
        Resource* r = *it;
        if (r->getId() == key || r->getName().find(key) != std::string::npos)
        {
            std::cout << '\n';
            showByReference(*r);           // 多态：借助基类引用调用虚函数
            found = true;
        }
    }
    if (!found)
        std::cout << "  未找到符合条件的资源！\n";
}

/* ====================== 功能4：显示所有图书/资源情况 ====================== */
void Library::showAll() const
{
    std::cout << "\n================ 功能4：显示所有图书 / 资源情况 ================\n";
    if (resources.empty())
    {
        std::cout << "  当前图书馆中没有任何资源。\n";
        return;
    }

    std::cout << "  共 " << resources.size() << " 项资源：\n";
    int index = 1;
    for (Repository<Resource>::const_iterator it = resources.begin();
         it != resources.end(); ++it, ++index)
    {
        (*it)->printBrief(index);          // 多态：一行摘要也各不相同
    }

    std::cout << "  提示：想看某一项的完整信息，请使用功能3。\n";
}

/* ======================== 功能5：减少已有资源数量 ======================== */
void Library::decreaseCount()
{
    std::cout << "\n================ 功能5：减少已有图书 / 资源的数量 ================\n";
    std::string id = InputUtil::readString("  请输入资源编号（输入 0 返回）：");
    if (id == "0") return;

    Resource* r = resources.findById(id);
    if (r == nullptr) { std::cout << "  未找到该编号的资源！\n"; return; }

    std::cout << "  当前信息：" << r->getName()
              << "，馆藏 " << r->getTotal()
              << "，已借出 " << r->getBorrowed() << '\n';

    int n = InputUtil::readInt("  请输入本次减少的数量：", 1, 100000);
    if (!r->reduceCount(n))
    {
        std::cout << "  减少失败！馆藏数量不能小于已借出的数量"
                     "（当前已借出 " << r->getBorrowed() << " 件）。\n";
        return;
    }
    std::cout << "  减少成功！" << r->getName()
              << " 现有馆藏总数：" << r->getTotal() << '\n';
}

/* ============================ 功能6：删除资源 ============================ */
void Library::removeResource()
{
    std::cout << "\n================ 功能6：删除某一图书 / 资源 ================\n";
    std::string id = InputUtil::readString("  请输入要删除的资源编号（输入 0 返回）：");
    if (id == "0") return;

    int pos = resources.indexOf(id);       // 内部调用【函数模板】findIndexById
    if (pos < 0) { std::cout << "  未找到该编号的资源！\n"; return; }

    Resource* r = resources.at(static_cast<std::size_t>(pos));
    r->display();

    if (r->getBorrowed() > 0)             // 外面还有未归还的册数，不允许直接删除
    {
        std::cout << "  该项资源尚有 " << r->getBorrowed()
                  << " 件未归还，请先收回后再删除！\n";
        return;
    }

    if (InputUtil::readInt("  确认删除？(1.确认  0.取消)：", 0, 1) == 0)
    {
        std::cout << "  已取消删除。\n";
        return;
    }

    // erase 会 delete 对象并把后面的元素整体前移，这正是对象数组做不到的
    resources.eraseAt(static_cast<std::size_t>(pos));
    std::cout << "  删除成功！当前资源总数：" << resources.size() << " 项。\n";
}

/* ============================ 功能7：学生借阅 ============================ */
void Library::borrow()
{
    std::cout << "\n================ 功能7：学生借阅图书 / 资源 ================\n";

    std::string sid = InputUtil::readString("  请输入学生学号：");
    Student* stu = students.findById(sid);
    if (stu == nullptr)
    {
        std::cout << "  该学号尚未注册！\n";
        if (InputUtil::readInt("  是否立即注册该学生？(1.是  0.否)：", 0, 1) == 1)
            registerStudent();
        stu = students.findById(sid);
        if (stu == nullptr) return;   // 用户取消注册或注册时换了别的学号，放弃本次借阅
    }

    if (stu->borrowCount() >= MAX_BORROW)
    {
        std::cout << "  借阅失败：每人最多同时借阅 " << MAX_BORROW
                  << " 件，请先归还部分资源。\n";
        return;
    }

    std::string rid = InputUtil::readString("  请输入要借阅的资源编号：");
    Resource* res = resources.findById(rid);
    if (res == nullptr) { std::cout << "  未找到该编号的资源！\n"; return; }

    if (res->getAvailable() <= 0)
    {
        std::cout << "  借阅失败：「" << res->getName() << "」已全部借出，暂无可借。\n";
        return;
    }

    res->borrowOne();          // 资源侧：已借出 +1
    *stu += rid;               // 学生侧：记一条借阅记录（运算符重载 +=）

    std::cout << "  借阅成功！" << stu->getName() << "（" << stu->getId() << "）借走了 ["
              << res->typeName() << "] " << res->getName()
              << "，剩余可借 " << res->getAvailable() << " 件。\n";
}

/* ============================ 功能8：学生归还 ============================ */
void Library::giveBack()
{
    std::cout << "\n================ 功能8：学生归还图书 / 资源 ================\n";

    std::string sid = InputUtil::readString("  请输入学生学号：");
    Student* stu = students.findById(sid);
    if (stu == nullptr) { std::cout << "  该学号尚未注册！\n"; return; }

    std::string rid = InputUtil::readString("  请输入要归还的资源编号：");
    Resource* res = resources.findById(rid);
    if (res == nullptr) { std::cout << "  未找到该编号的资源！\n"; return; }

    if (!stu->hasBorrowed(rid))
    {
        std::cout << "  归还失败：" << stu->getName()
                  << " 名下没有借阅记录 " << rid << "。\n";
        return;
    }

    res->returnOne();   // 资源侧：已借出 -1
    *stu -= rid;        // 学生侧：删掉一条借阅记录（运算符重载 -=）

    std::cout << "  归还成功！感谢使用。" << res->getName()
              << " 当前可借 " << res->getAvailable() << " 件。\n";
}

/* ========================= 附加1：注册学生读者 ========================= */
void Library::registerStudent()
{
    std::cout << "\n================ 注册学生读者 ================\n";
    Student* s = new Student();            // new 一个学生对象
    s->input();

    if (students.contains(s->getId()))     // 学号不能重复
    {
        std::cout << "  学号已存在，注册失败！\n";
        delete s;
        return;
    }

    students.push(s);
    std::cout << "  注册成功！欢迎 " << s->getName()
              << "，当前读者总数：" << students.size() << " 人。\n";
}

/* ==================== 附加2：显示学生及借阅情况 ==================== */
void Library::showStudents() const
{
    std::cout << "\n================ 学生及借阅情况 ================\n";
    if (students.empty())
    {
        std::cout << "  当前还没有注册任何学生读者。\n";
        return;
    }
    for (Repository<Student>::const_iterator it = students.begin();
         it != students.end(); ++it)
    {
        std::cout << **it;                 // 运算符重载 <<：等价于 display()
    }
}

/* ================= 附加3：虚函数多态演示（实验四核心知识点） ================= */
void Library::demoPolymorphism() const
{
    std::cout << "\n=========== 虚函数多态演示 ===========\n";
    if (resources.empty())
    {
        std::cout << "  当前没有任何资源，请先使用功能1添加。\n";
        return;
    }

    std::cout << "\n【演示一】基类【指针】调用虚函数\n";
    for (Repository<Resource>::const_iterator it = resources.begin();
         it != resources.end(); ++it)
    {
        Resource* p = *it;                          // 基类指针指向派生类对象
        std::cout << "  Resource* -> " << p->typeName()
                  << "（" << p->getName() << "）\n";
    }

    std::cout << "\n【演示二】基类【引用】调用虚函数 display()\n";
    for (Repository<Resource>::const_iterator it = resources.begin();
         it != resources.end(); ++it)
    {
        showByReference(**it);                      // 同一段代码，不同行为
    }

    std::cout << "\n【演示三】运算符重载 operator<< 内部调用虚函数\n";
    for (Repository<Resource>::const_iterator it = resources.begin();
         it != resources.end(); ++it)
    {
        std::cout << **it;
    }

    std::cout << "\n【演示四】虚析构函数：delete 基类指针时会调用哪些析构函数\n"
              << "  代码：Resource* p = new Book(...);  delete p;\n"
              << "  执行过程：\n";
    Resource::setTraceDestructor(true);             // 打开析构跟踪，方便观察顺序
    Resource* p = new Book("B999", "演示用图书", "演示出版社", 2026, 1, 0,
                           "佚名", "000-000", "TP000");
    delete p;                                       // 基类指针删除派生类对象
    Resource::setTraceDestructor(false);            // 用完关掉
    std::cout << "  结论：因为 ~Resource() 声明为 virtual，delete 基类指针时\n"
              << "        先调用派生类析构函数，再调用基类析构函数，对象被完整释放。\n";
}

/* ==========================================================================
 *             【实验五】vector 容器与模板 —— 现场演示
 * --------------------------------------------------------------------------
 *  演示四件事：
 *    ① vector 里放【对象】和放【对象指针】到底有什么区别
 *    ② vector 的常用成员函数逐一跑一遍
 *    ③ 函数模板：同一份算法服务不同类型
 *    ④ 类模板：Repository<Resource> 与 Repository<Student> 是怎么来的
 * ========================================================================== */
void Library::demoVectorAndTemplate() const
{
    std::cout << "\n=========== 实验五：vector 容器与模板演示 ===========\n";

    /* ---------- ① 放对象 vs 放指针 ---------- */
    std::cout << "\n【演示一】vector 放【对象】还是放【对象指针】？\n";

    std::vector<Student> objVec;                       // 放对象
    objVec.emplace_back("S90001", "王五", "软工2302");  // emplace_back：就地构造，不产生临时对象
    objVec.push_back(Student("S90002", "赵六", "软工2302")); // push_back：先造临时对象再拷进去
    std::cout << "  vector<Student>（放对象）元素个数 size() = " << objVec.size() << '\n';
    std::cout << "  放对象：内存自动回收，但不能装抽象类，且会发生——\n";

    // 对象切片现场演示
    std::vector<SliceBase> sliceVec;                   // 容器声明的元素类型是【基类】
    SliceDerived d;
    std::cout << "    切片前：d.who() = " << d.who()
              << "，派生类特有成员 b = " << d.b << "、「" << d.extra << "」\n";
    sliceVec.push_back(d);                             // 只把基类那一部分拷了进去
    std::cout << "    切片后：v[0].who() = " << sliceVec[0].who()
              << "，再用 v[0].b 访问派生类成员？—— 编译都过不去，b 已被切掉。\n";
    std::cout << "  => 结论：想保住派生类的完整身份（多态），就必须放【指针】。\n";

    std::vector<Resource*> ptrVec;                     // 放指针
    ptrVec.push_back(new Book("B900", "指针演示图书", "演示出版社", 2026, 1, 0,
                              "佚名", "000-001", "TP000"));
    ptrVec.emplace_back(new Magazine("M900", "指针演示杂志", "演示出版社", 2026, 1, 0,
                                     "0000-0001", "2026年第1期", "月刊"));
    std::cout << "  vector<Resource*>（放基类指针）size() = " << ptrVec.size()
              << "，一个容器同时装下图书和杂志，且类型信息完整：\n";
    for (std::vector<Resource*>::iterator it = ptrVec.begin(); it != ptrVec.end(); ++it)
        std::cout << "    -> " << (*it)->typeName() << "（" << (*it)->getName() << "）\n";
    deleteAll<Resource>(ptrVec);                       // 函数模板：统一释放
    std::cout << "  释放后 size() = " << ptrVec.size() << "（clear() 已清空）\n";

    /* ---------- ② vector 常用函数 ---------- */
    std::cout << "\n【演示二】vector 常用函数逐一演示\n";
    std::vector<std::string> demo;
    demo.push_back("第一本");                 // push_back
    demo.push_back("第二本");
    demo.emplace_back("第三本");              // emplace_back
    std::cout << "  push_back / emplace_back 之后 size() = " << demo.size() << '\n';
    std::cout << "  用 begin()/end() 迭代器遍历：";
    for (std::vector<std::string>::iterator it = demo.begin(); it != demo.end(); ++it)
        std::cout << *it << ' ';
    std::cout << '\n';
    std::cout << "  用下标遍历：";
    for (std::size_t i = 0; i < demo.size(); ++i)
        std::cout << demo[i] << ' ';
    std::cout << '\n';
    // erase() 返回的是"被删元素的下一个元素"的迭代器
    std::vector<std::string>::iterator it = demo.erase(demo.begin() + 1);
    std::cout << "  erase(begin()+1) 删除了「第二本」，返回值指向「" << *it
              << "」，size() = " << demo.size() << '\n';
    demo.clear();
    std::cout << "  clear() 之后 size() = " << demo.size() << "\n";

    /* ---------- ③ 函数模板 ---------- */
    std::cout << "\n【演示三】函数模板：一份代码服务不同类型\n";
    std::vector<Student*> tmpStudents;
    tmpStudents.push_back(new Student("S91001", "模板演示甲", "计科2301"));
    tmpStudents.push_back(new Student("S91002", "模板演示乙", "计科2301"));
    std::cout << "  findIndexById<Student>(\"S91002\") = "
              << findIndexById<Student>(tmpStudents, "S91002") << '\n';
    std::cout << "  findIndexById<Student>(\"不存在\") = "
              << findIndexById<Student>(tmpStudents, "不存在") << '\n';
    std::cout << "  同一份模板换成资源也能用：findIndexById<Resource> 正在被 Library 使用。\n";
    deleteAll<Student>(tmpStudents);
    std::cout << "  函数模板 deleteAll<T>() 释放完毕，size() = " << tmpStudents.size() << '\n';

    /* ---------- ④ 类模板 ---------- */
    std::cout << "\n【演示四】类模板 Repository<T>\n";
    std::cout << "  Library 里的两个仓库其实是同一个类模板的两次实例化：\n"
              << "    Repository<Resource> resources;   size() = " << resources.size() << '\n'
              << "    Repository<Student>  students;    size() = " << students.size() << '\n';

    Repository<Student> repo;                          // 现场再实例化一个
    repo.push(new Student("S92001", "仓库演示甲", "计科2301"));
    repo.emplace(new Student("S92002", "仓库演示乙", "计科2301"));
    std::cout << "  新建 Repository<Student>：size() = " << repo.size() << '\n';
    Student* found = repo.findById("S92002");           // 用 begin()/end() 迭代器查找
    std::cout << "  findById(\"S92002\") -> "
              << (found ? found->getName() : "未找到") << '\n';
    std::cout << "  indexOf(\"S92002\")  = " << repo.indexOf("S92002") << '\n';
    std::cout << "  eraseById(\"S92001\") 结果 = " << (repo.eraseById("S92001") ? "成功" : "失败")
              << "，剩余 size() = " << repo.size() << '\n';
    repo.clear();                                       // delete + clear
    std::cout << "  clear() 之后 size() = " << repo.size()
              << "（析构时会再自动 clear 一次，保证不泄漏）\n";
}

/* ==========================================================================
 *             【实验六】输入 / 输出流 —— 现场演示
 * --------------------------------------------------------------------------
 *  cin  是 istream 对象（键盘）；cout 是 ostream 对象（屏幕）；
 *  ifstream / ofstream 是文件版，用法几乎一模一样：
 *      << 是插入操作符（往流里写），>> 是提取操作符（从流里读）。
 * ========================================================================== */
void Library::demoFileStream() const
{
    std::cout << "\n=========== 实验六：输入 / 输出流演示 ===========\n";
    const std::string demo = "demo_stream.txt";

    /* ---------- ① 打开与关闭文件的两种方式 ---------- */
    std::cout << "\n【演示一】文件的打开与关闭\n"
              << "  方式一：先定义流对象，再 open(文件名, 方式)，最后 close()\n";
    std::ofstream ofs;                       // 输出文件流（写）
    ofs.open(demo.c_str(), std::ios::out);   // 打开写文件
    if (!ofs.is_open())
    {
        std::cout << "  打开文件失败！\n";
        return;
    }
    ofs << "BOOK|B888|流演示图书|演示出版社|2026|3|0|佚名|000-888|TP000\n"
        << "MAG|M888|流演示杂志|演示出版社|2026|2|0|0000-8888|2026年第1期|月刊\n";
    ofs.close();                             // 操作完必须关闭
    std::cout << "  已用 <<（插入操作符）写入 2 行，并 close()。\n";

    std::cout << "  方式二：定义的同时就打开——ifstream ifs(\"demo_stream.txt\");\n";
    std::ifstream ifs(demo.c_str());         // 输入文件流（读），构造时直接打开
    std::cout << "  文件内容：\n";
    std::string line;
    while (FileUtil::getLine(ifs, line))
        std::cout << "    > " << line << '\n';
    ifs.close();
    std::cout << "  已用 getline 逐行读出，并 close()。\n";

    /* ---------- ② 把内存中的对象写进文件 ---------- */
    std::cout << "\n【演示二】对象 -> 文件：调用每个对象的成员函数 save()\n";
    const std::string out = "demo_library_data.txt";
    if (saveToFile(out))
        std::cout << "  已把 " << resources.size() << " 个资源、"
                  << students.size() << " 名学生写入「" << out << "」\n";
    std::ifstream show(out.c_str());
    std::cout << "  文件前几行（注意每行开头的类型标记）：\n";
    int shown = 0;
    while (FileUtil::getLine(show, line) && shown < 8)
    {
        std::cout << "    > " << line << '\n';
        ++shown;
    }
    show.close();

    /* ---------- ③ 从文件把对象读回来 ---------- */
    std::cout << "\n【演示三】文件 -> 对象：按类型标记 new 出对应的派生类\n";
    std::ifstream back(out.c_str());
    std::string section;
    int n = 0;
    while (FileUtil::getLine(back, line))
    {
        if (line.empty() || line[0] == '#') continue;
        if (line[0] == '[') { section = line; continue; }
        if (section != "[RESOURCES]") continue;
        Resource* r = ResourceFactory::createFromLine(line);   // 工厂 + 多态读取
        if (r != nullptr)
        {
            std::cout << "  读回一个 " << r->typeName()
                      << "：" << r->getName()
                      << "（馆藏 " << r->getTotal()
                      << "，已借出 " << r->getBorrowed() << "）\n";
            delete r;                        // 演示用的临时对象，用完立刻释放
            if (++n >= 3) break;
        }
    }
    back.close();
    std::cout << "  => 因为文件里记了类型标记，读回来仍然是真正的派生类对象，\n"
              << "     基类和派生类对象因此可以共存在同一个文件里。\n";
}

/* ==========================================================================
 *                 【实验六】对象 -> 文件（存档）
 * --------------------------------------------------------------------------
 *  文件格式（文本文件，一行一个对象，字段用 | 分隔）：
 *      # 注释行
 *      [RESOURCES]
 *      BOOK|B001|名称|出版社|2013|8|1|作者|ISBN|分类
 *      MAG |M001|名称|出版社|2026|5|1|ISSN|期号|周期
 *      ERES|E001|名称|出版社|2024|10|1|格式|大小|链接
 *      [STUDENTS]
 *      S202301|张三|计科2301|B001
 *      [END]
 * ========================================================================== */
bool Library::saveToFile(const std::string& path) const
{
    std::ofstream ofs;
    ofs.open(path.c_str(), std::ios::out);          // 打开写文件
    if (!ofs.is_open())
    {
        std::cout << "  打开文件「" << path << "」失败！\n";
        return false;
    }

    ofs << "# 图书资源借阅管理系统 数据文件 v1\n"
        << "# 一行一个对象，字段之间用 | 分隔；# 开头为注释，[xxx] 为分段标记\n"
        << "[RESOURCES]\n";
    // 多态：一句 save() 就能写出各自类型的全部字段
    for (Repository<Resource>::const_iterator it = resources.begin();
         it != resources.end(); ++it)
    {
        (*it)->save(ofs);
    }

    ofs << "[STUDENTS]\n";
    for (Repository<Student>::const_iterator it = students.begin();
         it != students.end(); ++it)
    {
        (*it)->save(ofs);
    }
    ofs << "[END]\n";

    ofs.close();                                     // 用完关闭
    return true;
}

/* ==========================================================================
 *                 【实验六】文件 -> 对象（读档）
 * --------------------------------------------------------------------------
 *  先读出每行的【类型标记】，交给 ResourceFactory 决定 new 哪个派生类，
 *  再由对象自己的 loadFromLine() 把字段填回去。
 * ========================================================================== */
bool Library::loadFromFile(const std::string& path)
{
    std::ifstream ifs;
    ifs.open(path.c_str(), std::ios::in);            // 打开读文件
    if (!ifs.is_open())
        return false;

    resources.clear();       // 先清空旧数据（Repository 会顺带 delete 掉旧对象）
    students.clear();

    std::string section, line;
    int rc = 0, sc = 0;
    while (FileUtil::getLine(ifs, line))
    {
        if (line.empty() || line[0] == '#') continue;   // 跳过空行和注释
        if (line[0] == '[') { section = line; continue; } // 遇到分段标记就切换状态

        if (section == "[RESOURCES]")
        {
            Resource* r = ResourceFactory::createFromLine(line);
            if (r != nullptr) { resources.push(r); ++rc; }
        }
        else if (section == "[STUDENTS]")
        {
            Student* s = new Student();
            if (s->loadFromLine(line)) { students.push(s); ++sc; }
            else delete s;
        }
    }
    ifs.close();

    std::cout << "  已从「" << path << "」读取 " << rc
              << " 项资源、" << sc << " 名学生。\n";
    return true;
}

/* ================================ 菜单 ================================ */
void Library::printMenu()
{
    std::cout << "\n========================================\n"
              << "        图 书 资 源 借 阅 管 理 系 统\n"
              << "        （实验五 vector+模板 / 实验六 文件流）\n"
              << "========================================\n"
              << "   1. 增加新的图书 / 资源\n"
              << "   2. 增加已有图书 / 资源的数量\n"
              << "   3. 显示某一图书 / 资源情况\n"
              << "   4. 显示所有图书 / 资源情况\n"
              << "   5. 减少已有图书 / 资源的数量\n"
              << "   6. 删除某一图书 / 资源\n"
              << "   7. 学生借阅图书 / 资源\n"
              << "   8. 学生归还图书 / 资源\n"
              << "----------------------------------------\n"
              << "   9. 注册学生读者\n"
              << "  10. 显示所有学生及借阅情况\n"
              << "  11. 虚函数多态演示\n"
              << "----------------------------------------\n"
              << "  12. 保存数据到文件（实验六）\n"
              << "  13. 从文件读取数据（实验六）\n"
              << "  14. vector 与模板演示（实验五）\n"
              << "  15. 文件流读写演示（实验六）\n"
              << "   0. 退出系统（自动保存）\n"
              << "========================================\n";
}

void Library::run()
{
    std::cout << "========================================\n"
              << "   欢迎使用 图书资源借阅管理系统！\n"
              << "   数据文件：" << dataFile << "\n"
              << "   （启动时自动读档，退出时自动存档）\n"
              << "========================================\n";

    while (true)
    {
        printMenu();
        int choice = InputUtil::readInt("  请输入功能编号（0-15）：", 0, 15);

        switch (choice)
        {
        case 1:  addResource();    break;
        case 2:  increaseCount();  break;
        case 3:  showOne();        break;
        case 4:  showAll();        break;
        case 5:  decreaseCount();  break;
        case 6:  removeResource(); break;
        case 7:  borrow();         break;
        case 8:  giveBack();       break;
        case 9:  registerStudent();break;
        case 10: showStudents();       break;
        case 11: demoPolymorphism();   break;
        case 12:
            if (saveToFile(dataFile))
                std::cout << "  已保存 " << resources.size() << " 项资源、"
                          << students.size() << " 名学生到「" << dataFile << "」。\n";
            break;
        case 13:
            if (loadFromFile(dataFile))
                std::cout << "  读档完成，当前 " << resources.size()
                          << " 项资源、" << students.size() << " 名学生。\n";
            else
                std::cout << "  读取失败，请确认「" << dataFile << "」是否存在。\n";
            break;
        case 14: demoVectorAndTemplate(); break;
        case 15: demoFileStream();        break;
        case 0:
            std::cout << "  正在保存数据…\n";
            if (saveToFile(dataFile))
                std::cout << "  已保存到「" << dataFile << "」，下次启动自动读取。\n";
            std::cout << "  感谢使用，再见！\n";
            return;                        // 退出 run()，随后 main 结束 -> 调用析构函数
        default:
            std::cout << "  无效的功能编号，请重新选择！\n";
            break;
        }

        if (choice != 0)
        {
            std::cout << "\n  >>> 按回车键返回菜单…";
            std::cin.get();                // 暂停，等用户敲一下回车
        }
    }
}
