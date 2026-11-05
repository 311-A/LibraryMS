#include "Student.h"
#include "FileUtil.h"
#include "InputUtil.h"

Student::Student() : id(""), name(""), className("") {}

Student::Student(const std::string& id, const std::string& name, const std::string& className)
    : id(id), name(name), className(className) {}

void Student::input()
{
    id        = InputUtil::readString("  学号：");
    name      = InputUtil::readString("  姓名：");
    className = InputUtil::readString("  班级：");
}

void Student::display(std::ostream& os) const
{
    os << "  学号：" << id << "  姓名：" << name << "  班级：" << className << '\n'
       << "    在借数量：" << borrowedIds.size() << " 件";
    if (borrowedIds.empty())
    {
        os << "（无借阅记录）\n";
    }
    else
    {
        os << "  编号：";
        for (size_t i = 0; i < borrowedIds.size(); ++i)
            os << borrowedIds[i] << ' ';
        os << '\n';
    }
    os << "----------------------------------------\n";
}

std::ostream& operator<<(std::ostream& os, const Student& s)
{
    s.display(os);
    return os;
}

Student& Student::operator+=(const std::string& resourceId)
{
    addBorrow(resourceId);
    return *this;
}

Student& Student::operator-=(const std::string& resourceId)
{
    removeBorrow(resourceId);
    return *this;
}

void Student::addBorrow(const std::string& resourceId)
{
    borrowedIds.push_back(resourceId);
}

// 只删第一条匹配的（可能借了多期同名杂志）
bool Student::removeBorrow(const std::string& resourceId)
{
    for (size_t i = 0; i < borrowedIds.size(); ++i)
    {
        if (borrowedIds[i] == resourceId)
        {
            borrowedIds.erase(borrowedIds.begin() + i);
            return true;
        }
    }
    return false;
}

bool Student::hasBorrowed(const std::string& resourceId) const
{
    for (size_t i = 0; i < borrowedIds.size(); ++i)
        if (borrowedIds[i] == resourceId)
            return true;
    return false;
}

/* ==========================================================================
 *                    实验六：学生对象的存 / 读
 *  一行格式：S202301|张三|计科2301|B001|M001
 *            （前面三个是固定字段，后面跟任意多个已借资源编号）
 * ========================================================================== */
void Student::save(std::ostream& os) const
{
    os << FileUtil::sanitize(id) << '|'
       << FileUtil::sanitize(name) << '|'
       << FileUtil::sanitize(className);
    for (size_t i = 0; i < borrowedIds.size(); ++i)
        os << '|' << FileUtil::sanitize(borrowedIds[i]);
    os << '\n';
}

bool Student::load(std::istream& is)
{
    std::string line;
    if (!FileUtil::getLine(is, line))
        return false;
    return loadFromLine(line);
}

bool Student::loadFromLine(const std::string& line)
{
    if (line.empty())
        return false;
    std::vector<std::string> f = FileUtil::split(line, '|');
    if (f.size() < 3)
        return false;
    id        = f[0];
    name      = f[1];
    className = f[2];
    borrowedIds.clear();
    for (size_t i = 3; i < f.size(); ++i)
        if (!f[i].empty())
            borrowedIds.push_back(f[i]);
    return true;
}
