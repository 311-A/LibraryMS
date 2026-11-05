#include "EResource.h"
#include "FileUtil.h"
#include "InputUtil.h"

namespace
{
    const std::size_t I_FORMAT = Resource::IDX_DETAIL + 0;
    const std::size_t I_SIZE   = Resource::IDX_DETAIL + 1;
    const std::size_t I_URL    = Resource::IDX_DETAIL + 2;
}

EResource::EResource()
    : Resource(), format(""), sizeMB(0.0), url("") {}

EResource::~EResource()
{
    if (traceDestructor)
        std::cout << "        [析构] ~EResource() 派生部分（" << name << "）\n";
}

EResource::EResource(const std::string& id, const std::string& name,
                     const std::string& publisher, int year, int total, int borrowed,
                     const std::string& format, double sizeMB, const std::string& url)
    : Resource(id, name, publisher, year, total, borrowed),
      format(format), sizeMB(sizeMB), url(url) {}

void EResource::display(std::ostream& os) const
{
    Resource::display(os);
    os << "  文件格式：" << format  << '\n'
       << "  文件大小：" << sizeMB  << " MB\n"
       << "  访问链接：" << url     << '\n'
       << "----------------------------------------\n";
}

void EResource::inputDetail()
{
    format = InputUtil::readString("  文件格式（PDF / EPUB / MP4 / 数据库 …）：");
    sizeMB = InputUtil::readDouble("  文件大小（MB）：", 0.0, 1.0e6);
    url    = InputUtil::readString("  访问链接 / 入口网址：");
}

void EResource::printBriefExtra(std::ostream& os) const
{
    os << "  格式：" << format
       << "  大小：" << sizeMB << "MB"
       << "  链接：" << url;
}

void EResource::saveDetail(std::ostream& os) const
{
    os << FileUtil::sanitize(format) << '|'
       << sizeMB << '|'
       << FileUtil::sanitize(url);
}

void EResource::loadDetail(const std::vector<std::string>& f)
{
    if (f.size() > I_FORMAT) format = f[I_FORMAT];
    if (f.size() > I_SIZE)   sizeMB = FileUtil::toDouble(f[I_SIZE], 0.0);
    if (f.size() > I_URL)    url    = f[I_URL];
}
