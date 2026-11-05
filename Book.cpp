#include "Book.h"
#include "FileUtil.h"
#include "InputUtil.h"

// 文件中"子类特有字段"所在的列（0-6 列是类型码和公共字段，见 Resource.h）
namespace
{
    const std::size_t I_AUTHOR   = Resource::IDX_DETAIL + 0;
    const std::size_t I_ISBN     = Resource::IDX_DETAIL + 1;
    const std::size_t I_CATEGORY = Resource::IDX_DETAIL + 2;
}

Book::Book() : Resource(), author(""), isbn(""), category("") {}

Book::~Book()
{
    if (traceDestructor)
        std::cout << "        [析构] ~Book() 派生部分（" << name << "）\n";
}

Book::Book(const std::string& id, const std::string& name,
           const std::string& publisher, int year, int total, int borrowed,
           const std::string& author, const std::string& isbn, const std::string& category)
    : Resource(id, name, publisher, year, total, borrowed),
      author(author), isbn(isbn), category(category) {}

// 先让基类把公共信息打印好，再补上 Book 自己的部分
void Book::display(std::ostream& os) const
{
    Resource::display(os);
    os << "  作    者：" << author   << '\n'
       << "  I S B N ：" << isbn     << '\n'
       << "  中图分类：" << category << '\n'
       << "----------------------------------------\n";
}

void Book::inputDetail()
{
    author   = InputUtil::readString("  作者：");
    isbn     = InputUtil::readString("  ISBN：");
    category = InputUtil::readString("  分类（如 TP312C++）：");
}

void Book::printBriefExtra(std::ostream& os) const
{
    os << "  作者：" << author
       << "  ISBN：" << isbn;
}

/* ------------------------ 实验六：Book 的存 / 读 ------------------------ */
void Book::saveDetail(std::ostream& os) const
{
    os << FileUtil::sanitize(author) << '|'
       << FileUtil::sanitize(isbn) << '|'
       << FileUtil::sanitize(category);
}

void Book::loadDetail(const std::vector<std::string>& f)
{
    if (f.size() > I_AUTHOR)   author   = f[I_AUTHOR];
    if (f.size() > I_ISBN)     isbn     = f[I_ISBN];
    if (f.size() > I_CATEGORY) category = f[I_CATEGORY];
}
