#pragma once
#include "Resource.h"

/* ==========================================================================
 *  EResource —— 电子资源（Resource 的派生类）
 * --------------------------------------------------------------------------
 *  特有属性：文件格式（PDF/EPUB/MP4/数据库…）、文件大小（MB）、在线访问链接。
 *  数量含义：理解为"同时在线使用的许可数"或"馆内可访问并发数"。
 * ========================================================================== */
class EResource : public Resource
{
private:
    std::string format;    // 文件格式 / 类型，如 PDF、EPUB、数据库
    double      sizeMB;    // 文件大小（MB）
    std::string url;       // 访问链接 / DOI / 入口网址

public:
    EResource();
    EResource(const std::string& id, const std::string& name,
              const std::string& publisher, int year, int total, int borrowed,
              const std::string& format, double sizeMB, const std::string& url);
    ~EResource() override;

    const std::string& getFormat() const { return format; }
    double             getSizeMB() const { return sizeMB; }
    const std::string& getURL()    const { return url; }

    void display(std::ostream& os = std::cout) const override;

    const char* typeCode() const override { return "ERES"; }
    void saveDetail(std::ostream& os) const override;
    void loadDetail(const std::vector<std::string>& f) override;

protected:
    const char* typeName() const override { return "电子资源"; }
    void inputDetail() override;
    void printBriefExtra(std::ostream& os) const override;
};
