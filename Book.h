#pragma once
#include "Resource.h"

/* ==========================================================================
 *  Book —— 一般图书（Resource 的【派生类 / 子类】）
 * --------------------------------------------------------------------------
 *  继承自 Resource：自动获得"编号、名称、出版社、年份、馆藏数量"等所有公共属性，
 *                  以及借/还、增/减数量等行为。
 *  新增特有的属性：作者、ISBN、分类。
 *  重写（override）从基类继承下来的虚函数：
 *      typeName() / inputDetail() / printBriefExtra()
 *      typeCode() / saveDetail() / loadDetail()          【实验六新增】
 * ========================================================================== */
class Book : public Resource
{
private:
    std::string author;   // 作者
    std::string isbn;     // 国际标准书号
    std::string category; // 中图分类，如"TP312C++"

public:
    Book();
    Book(const std::string& id, const std::string& name,
         const std::string& publisher, int year, int total, int borrowed,
         const std::string& author, const std::string& isbn, const std::string& category);
    ~Book() override;

    const std::string& getAuthor()   const { return author; }
    const std::string& getISBN()     const { return isbn; }
    const std::string& getCategory() const { return category; }

    void display(std::ostream& os = std::cout) const override;

    const char* typeCode() const override { return "BOOK"; }   // 文件里的类型标记
    void saveDetail(std::ostream& os) const override;
    void loadDetail(const std::vector<std::string>& f) override;

protected:
    const char* typeName() const override { return "一般图书"; }
    void inputDetail() override;
    void printBriefExtra(std::ostream& os) const override;
};
