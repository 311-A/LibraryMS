#pragma once
#include <iostream>
#include <string>
#include <vector>

/* ==========================================================================
 *  Resource —— 图书资源【抽象基类】（基类 / 父类）
 * --------------------------------------------------------------------------
 *  设计思想：
 *    把"所有资源都有的东西"放在基类里，避免在每个子类中重复写：
 *      公共属性：编号 id、名称 name、出版单位 publisher、
 *                年份 year、馆藏总数 total、已借出数 borrowed
 *      公共操作：增加数量 addCount、减少数量 reduceCount、
 *                借出 borrowOne、归还 returnOne、显示公共信息 displayCommon
 *
 *    把"每种资源都不一样的东西"声明为纯虚函数，交给子类去实现：
 *      typeName()       返回类型名称（一般图书 / 杂志 / 电子资源）
 *      typeCode()       返回文件中的类型标记（BOOK / MAG / ERES）【实验六】
 *      inputDetail()    读入该类资源特有的属性
 *      printBriefExtra()在列表里额外显示的该类资源特有属性
 *      saveDetail()     把该类资源特有属性写进文件【实验六】
 *      loadDetail()     从文件里读回该类资源特有属性【实验六】
 *
 *  【多态的关键】
 *    基类指针 Resource* 可以指向任意子类对象；
 *    通过 Resource* 调用虚函数时，程序会在运行时自动找到子类的版本。
 *    这样 Library 里只需要写一份容器，就能管理所有类型的资源。
 *
 *  【为什么析构函数要写成 virtual】
 *    delete (Resource*)p 时若析构函数不是虚函数，只会析构基类部分，
 *    子类自己申请的资源就泄漏了。写成虚析构可以保证完整析构。
 * ========================================================================== */
class Resource
{
protected:
    std::string id;         // 资源编号（唯一标识，例如 B001 / M001 / E001）
    std::string name;       // 资源名称
    std::string publisher;  // 出版 / 发行单位
    int  year;              // 出版年份
    int  total;             // 馆藏总数（电子资源理解为"并发使用许可数"）
    int  borrowed;          // 已借出数量

    static bool traceDestructor;   // 是否打印析构调用过程（默认关闭）

public:
    Resource();
    Resource(const std::string& id, const std::string& name,
             const std::string& publisher, int year, int total, int borrowed = 0);

    // 【虚析构函数】凡是设计虚函数作为接口的基类，析构函数必须声明为 virtual。
    virtual ~Resource();

    static void setTraceDestructor(bool on);

    // ---------------- 运算符重载（与虚函数多态配合使用） ----------------
    friend std::ostream& operator<<(std::ostream& os, const Resource& r);

    bool operator==(const Resource& rhs) const { return id == rhs.id; }
    bool operator==(const std::string& rhsId) const { return id == rhsId; }

    // ---------------- 普通成员函数（所有子类直接继承使用） ----------------
    const std::string& getId()        const { return id; }
    const std::string& getName()      const { return name; }
    const std::string& getPublisher() const { return publisher; }
    int getYear()      const { return year; }
    int getTotal()     const { return total; }
    int getBorrowed()  const { return borrowed; }
    int getAvailable() const { return total - borrowed; }

    void addCount(int n);
    bool reduceCount(int n);
    bool borrowOne();
    bool returnOne();

    // 统一入口：一次调用就把"公共信息 + 子类特有信息"全部读完（模板方法）
    void input();

    virtual void display(std::ostream& os = std::cout) const;
    void printBrief(int index, std::ostream& os = std::cout) const;

    virtual const char* typeName() const = 0;   // 资源类型名称（纯虚）
    virtual const char* typeCode() const = 0;   // 文件里的类型标记（纯虚，实验六）

    /* ==================== 实验六：对象的存储与读取 ====================
     * 保存：save() 是虚函数。写文件时只要一句 os << *res，就会自动按真实类型
     *       写出"类型标记 + 公共字段 + 子类特有字段"。
     * 读取：load() 是模板方法 —— 读一整行、切分成字段，先填公共字段，
     *       再通过虚函数 loadDetail() 落到具体子类填特有字段。
     * ================================================================ */
    virtual void save(std::ostream& os) const;   // 写一行（类型码|公共字段|子类字段）
    bool load(std::istream& is);                 // 从流里读一行并解析
    bool loadFromLine(const std::string& line);  // 解析已经读到的一行

    // 文件里各字段所在的列号（0 是类型码，1..6 是公共字段，7 之后留给子类特有字段）
    static const std::size_t IDX_TYPE   = 0;
    static const std::size_t IDX_ID     = 1;
    static const std::size_t IDX_NAME   = 2;
    static const std::size_t IDX_PUB    = 3;
    static const std::size_t IDX_YEAR   = 4;
    static const std::size_t IDX_TOTAL  = 5;
    static const std::size_t IDX_BORR   = 6;
    static const std::size_t IDX_DETAIL = 7;   // 子类特有字段从这里开始

protected:
    void inputBase();
    void displayCommon(std::ostream& os) const;

    void loadCommon(const std::vector<std::string>& f);   // 读公共字段

    // ---------------- 纯虚函数：必须由子类实现（体现"继承 + 派生"） ----------------
    virtual void inputDetail() = 0;
    virtual void printBriefExtra(std::ostream& os) const = 0;
    virtual void saveDetail(std::ostream& os) const = 0;                            // 实验六
    virtual void loadDetail(const std::vector<std::string>& f) = 0;                 // 实验六
};
