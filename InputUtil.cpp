#include "InputUtil.h"

// 清空输入缓冲区里残留的内容（尤其是 >> 之后留下的回车）
static void clearBuffer()
{
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

std::string InputUtil::readString(const std::string& tip)
{
    std::string value;
    while (true)
    {
        std::cout << tip;
        std::getline(std::cin, value);
        // 去掉首尾空白字符，并把"全是空格"的输入当作空串处理
        size_t b = value.find_first_not_of(" \t\r\n");
        if (b == std::string::npos)          // 全是空白，视为空串
        {
            std::cout << "  输入不能为空，请重新输入！\n";
            continue;
        }
        size_t e = value.find_last_not_of(" \t\r\n");
        return value.substr(b, e - b + 1);
    }
}

int InputUtil::readInt(const std::string& tip, int min, int max)
{
    int value;
    while (true)
    {
        std::cout << tip;
        if (std::cin >> value)               // 读到了整数
        {
            clearBuffer();
            if (value >= min && value <= max)
                return value;
            std::cout << "  输入超出范围，请重新输入！\n";
        }
        else                                  // 读到的不是整数
        {
            std::cout << "  输入格式错误，请输入数字！\n";
            clearBuffer();
        }
    }
}

double InputUtil::readDouble(const std::string& tip, double min, double max)
{
    double value;
    while (true)
    {
        std::cout << tip;
        if (std::cin >> value)
        {
            clearBuffer();
            if (value >= min && value <= max)
                return value;
            std::cout << "  输入超出范围，请重新输入！\n";
        }
        else
        {
            std::cout << "  输入格式错误，请输入数字！\n";
            clearBuffer();
        }
    }
}
