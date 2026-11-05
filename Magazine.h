#pragma once
#include "Resource.h"

/* ==========================================================================
 *  Magazine —— 杂志 / 期刊（Resource 的派生类）
 * --------------------------------------------------------------------------
 *  特有属性：ISSN（国际刊号）、期号（如"2026年第10期"）、出版周期（月刊/季刊等）。
 * ========================================================================== */
class Magazine : public Resource
{
private:
    std::string issn;       // 国际标准刊号
    std::string issueNo;    // 期号
    std::string frequency;  // 出版周期：周刊 / 月刊 / 季刊 / 年刊 …

public:
    Magazine();
    Magazine(const std::string& id, const std::string& name,
             const std::string& publisher, int year, int total, int borrowed,
             const std::string& issn, const std::string& issueNo, const std::string& frequency);
    ~Magazine() override;

    const std::string& getISSN()      const { return issn; }
    const std::string& getIssueNo()   const { return issueNo; }
    const std::string& getFrequency() const { return frequency; }

    void display(std::ostream& os = std::cout) const override;

    const char* typeCode() const override { return "MAG"; }
    void saveDetail(std::ostream& os) const override;
    void loadDetail(const std::vector<std::string>& f) override;

protected:
    const char* typeName() const override { return "杂志"; }
    void inputDetail() override;
    void printBriefExtra(std::ostream& os) const override;
};
