#pragma once
#include <climits>
#include <iostream>
#include <limits>
#include <string>

/* ==========================================================================
 *  InputUtil —— 键盘输入工具（命名空间里的一组小函数）【沿用实验四】
 * --------------------------------------------------------------------------
 *  作用：统一处理"带提示的输入 + 输入错误的重试"，
 *        让其它类里的代码只关心业务，不用到处写 cin.fail() 之类的判断。
 *  注意：readInt / readDouble 用 >> 读取后会清理缓冲区里的回车，
 *        避免和后面的 getline 混用时读到空行。
 * ========================================================================== */
namespace InputUtil
{
    // 读入一行非空字符串，tip 为提示语
    std::string readString(const std::string& tip);

    // 读入一个 [min, max] 范围内的整数
    int readInt(const std::string& tip, int min = INT_MIN, int max = INT_MAX);

    // 读入一个 [min, max] 范围内的小数
    double readDouble(const std::string& tip, double min = 0.0, double max = 1e12);
}
