#include "ResourceFactory.h"
#include "Resource.h"
#include "Book.h"
#include "Magazine.h"
#include "EResource.h"

Resource* ResourceFactory::create(const std::string& typeCode)
{
    // 这里用 if 判断类型标记；返回的是【基类指针】，指向刚 new 出来的派生类对象
    if (typeCode == "BOOK") return new Book();
    if (typeCode == "MAG")  return new Magazine();
    if (typeCode == "ERES") return new EResource();
    return nullptr;
}

Resource* ResourceFactory::createFromLine(const std::string& line)
{
    if (line.empty())
        return nullptr;

    // 第 0 个字段就是类型标记
    std::string code = line;
    std::string::size_type p = line.find('|');
    if (p != std::string::npos)
        code = line.substr(0, p);

    Resource* r = create(code);
    if (r == nullptr)
        return nullptr;

    if (!r->loadFromLine(line))     // 让对象自己把自己填好（内部靠虚函数落到子类）
    {
        delete r;
        return nullptr;
    }
    return r;
}

Resource* ResourceFactory::createFromStream(std::istream& is)
{
    std::string line;
    if (!std::getline(is, line))
        return nullptr;
    if (!line.empty() && line[line.size() - 1] == '\r')
        line.erase(line.size() - 1);
    return createFromLine(line);
}
