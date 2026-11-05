#include "Magazine.h"
#include "FileUtil.h"
#include "InputUtil.h"

namespace
{
    const std::size_t I_ISSN      = Resource::IDX_DETAIL + 0;
    const std::size_t I_ISSUE     = Resource::IDX_DETAIL + 1;
    const std::size_t I_FREQUENCY = Resource::IDX_DETAIL + 2;
}

Magazine::Magazine()
    : Resource(), issn(""), issueNo(""), frequency("") {}

Magazine::~Magazine()
{
    if (traceDestructor)
        std::cout << "        [析构] ~Magazine() 派生部分（" << name << "）\n";
}

Magazine::Magazine(const std::string& id, const std::string& name,
                   const std::string& publisher, int year, int total, int borrowed,
                   const std::string& issn, const std::string& issueNo, const std::string& frequency)
    : Resource(id, name, publisher, year, total, borrowed),
      issn(issn), issueNo(issueNo), frequency(frequency) {}

void Magazine::display(std::ostream& os) const
{
    Resource::display(os);
    os << "  刊    号：" << issn       << '\n'
       << "  本  期 号：" << issueNo    << '\n'
       << "  出版周期：" << frequency  << '\n'
       << "----------------------------------------\n";
}

void Magazine::inputDetail()
{
    issn      = InputUtil::readString("  国际刊号 ISSN：");
    issueNo   = InputUtil::readString("  本期号（如 2026年第10期）：");
    frequency = InputUtil::readString("  出版周期（周刊/月刊/季刊/年刊）：");
}

void Magazine::printBriefExtra(std::ostream& os) const
{
    os << "  ISSN：" << issn
       << "  期号：" << issueNo
       << "  周期：" << frequency;
}

void Magazine::saveDetail(std::ostream& os) const
{
    os << FileUtil::sanitize(issn) << '|'
       << FileUtil::sanitize(issueNo) << '|'
       << FileUtil::sanitize(frequency);
}

void Magazine::loadDetail(const std::vector<std::string>& f)
{
    if (f.size() > I_ISSN)      issn      = f[I_ISSN];
    if (f.size() > I_ISSUE)     issueNo   = f[I_ISSUE];
    if (f.size() > I_FREQUENCY) frequency = f[I_FREQUENCY];
}
