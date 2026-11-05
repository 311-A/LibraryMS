#include "Resource.h"
#include "FileUtil.h"
#include "InputUtil.h"

// 类内 static 整型/枚举常量的类外定义（防止被取地址时链接报错）
const std::size_t Resource::IDX_TYPE;
const std::size_t Resource::IDX_ID;
const std::size_t Resource::IDX_NAME;
const std::size_t Resource::IDX_PUB;
const std::size_t Resource::IDX_YEAR;
const std::size_t Resource::IDX_TOTAL;
const std::size_t Resource::IDX_BORR;
const std::size_t Resource::IDX_DETAIL;

/* ---------------------------- 构造函数 / 析构函数 ---------------------------- */
bool Resource::traceDestructor = false;        // 类静态成员的类外定义

Resource::Resource()
    : id(""), name(""), publisher(""), year(0), total(0), borrowed(0) {}

Resource::Resource(const std::string& id, const std::string& name,
                   const std::string& publisher, int year, int total, int borrowed)
    : id(id), name(name), publisher(publisher),
      year(year), total(total), borrowed(borrowed) {}

// 【虚析构函数】delete 基类指针时先调用派生类析构，最后才调用到这里。
Resource::~Resource()
{
    if (traceDestructor)
        std::cout << "        [析构] ~Resource() 基类部分（编号 " << id << "）\n";
}

void Resource::setTraceDestructor(bool on)
{
    traceDestructor = on;
}

/* ------------------------------ 运算符重载 ------------------------------ */
// 函数体里只写一句 r.display(os)，具体打印什么由 display() 在运行期决定
std::ostream& operator<<(std::ostream& os, const Resource& r)
{
    r.display(os);
    return os;
}

/* ------------------------------ 数量的增删 ------------------------------ */
void Resource::addCount(int n)
{
    if (n > 0) total += n;
}

// 已经借出去的那几份还在外面，所以减少后必须保证 total >= borrowed
bool Resource::reduceCount(int n)
{
    if (n <= 0) return false;
    if (total - n < borrowed)
        return false;
    total -= n;
    return true;
}

/* ------------------------------ 借出与归还 ------------------------------ */
bool Resource::borrowOne()
{
    if (borrowed < total)
    {
        ++borrowed;
        return true;
    }
    return false;
}

bool Resource::returnOne()
{
    if (borrowed > 0)
    {
        --borrowed;
        return true;
    }
    return false;
}

/* ------------------------------ 键盘读入信息 ------------------------------ */
void Resource::input()
{
    inputBase();
    inputDetail();
}

void Resource::inputBase()
{
    id        = InputUtil::readString("  资源编号（唯一，如 B001/M001/E001）：");
    name      = InputUtil::readString("  资源名称：");
    publisher = InputUtil::readString("  出版 / 发行单位：");
    year      = InputUtil::readInt("  出版年份（如 2026）：", 1000, 9999);
    total     = InputUtil::readInt("  馆藏数量：", 0, 100000);
    borrowed  = 0;
}

/* ------------------------------ 显示：公共部分 ------------------------------ */
void Resource::displayCommon(std::ostream& os) const
{
    os << "  资源编号：" << id << '\n'
       << "  资源名称：" << name << '\n'
       << "  出版单位：" << publisher << '\n'
       << "  出版年份：" << year << '\n'
       << "  馆藏总数：" << total
       << "    已借出：" << borrowed
       << "    可借数：" << getAvailable() << '\n';
}

void Resource::display(std::ostream& os) const
{
    os << "----------------------------------------\n"
       << "  资源类型：" << typeName() << '\n';   // 多态：调用到子类的 typeName()
    displayCommon(os);
}

void Resource::printBrief(int index, std::ostream& os) const
{
    os << "[" << index << "] " << typeName()
       << "  编号：" << id
       << "  名称：" << name
       << "  馆藏：" << total
       << "  已借出：" << borrowed
       << "  可借：" << getAvailable();
    printBriefExtra(os);                     // 多态：子类补上自己的关键信息
    os << '\n';
}

/* ==========================================================================
 *                      实验六：对象的存储（写文件）
 * --------------------------------------------------------------------------
 *  save() 是虚函数，但只写"公共部分"，子类特有字段交给纯虚 saveDetail()。
 *  这样写文件时不用判断类型：
 *      for (Resource* r : resources)  r->save(ofs);
 *  每个对象会自动写出属于自己的那一串字段 —— 又一次多态。
 *
 *  行的格式：BOOK|B001|名称|出版社|2013|8|1|作者|ISBN|分类
 *            ^ 类型标记，读回来时靠它决定要 new 出哪个派生类
 * ========================================================================== */
void Resource::save(std::ostream& os) const
{
    os << typeCode() << '|'
       << FileUtil::sanitize(id) << '|'
       << FileUtil::sanitize(name) << '|'
       << FileUtil::sanitize(publisher) << '|'
       << year << '|'
       << total << '|'
       << borrowed << '|';
    saveDetail(os);          // 虚函数：子类把自己特有的字段接在后面
    os << '\n';
}

/* ==========================================================================
 *                      实验六：对象的读取（读文件）
 *  load() 就是读取版的"模板方法"：
 *      固定流程（读一行 -> 切分 -> 填公共字段）
 *      + 可变步骤（虚函数 loadDetail() 填子类特有字段）
 * ========================================================================== */
bool Resource::load(std::istream& is)
{
    std::string line;
    if (!FileUtil::getLine(is, line))
        return false;
    return loadFromLine(line);
}

bool Resource::loadFromLine(const std::string& line)
{
    if (line.empty())
        return false;
    std::vector<std::string> f = FileUtil::split(line, '|');
    if (f.size() < IDX_DETAIL)
        return false;                 // 列数不对，说明这行数据坏了
    loadCommon(f);
    loadDetail(f);                    // 虚函数：子类读自己的字段
    return true;
}

void Resource::loadCommon(const std::vector<std::string>& f)
{
    id        = f[IDX_ID];
    name      = f[IDX_NAME];
    publisher = f[IDX_PUB];
    year      = FileUtil::toInt(f[IDX_YEAR], 0);
    total     = FileUtil::toInt(f[IDX_TOTAL], 0);
    borrowed  = FileUtil::toInt(f[IDX_BORR], 0);
    if (borrowed > total) borrowed = total;   // 读到坏数据时兜个底
    if (total < 0) total = 0;
}
