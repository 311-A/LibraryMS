#pragma once
#include <iostream>
#include <string>
#include <vector>

/* ==========================================================================
 *  FileUtil —— 文件读写小工具【实验六新增】
 * --------------------------------------------------------------------------
 *  为什么需要它？
 *    实验六要求"把对象的成员变量值存到文件里，再原样读回来"。
 *    存的时候最省事的做法是：一个对象写一行，字段之间用分隔符 '|' 隔开。
 *    但中英文名称里可能本身就含有空格，所以不能用空格分词；用 '|' 更安全。
 *    万一用户输入了 '|'，又会破坏格式 —— 于是写之前先 sanitize() 过滤掉。
 *
 *  读的时候反过来：先 getline 读一整行，再 split() 按 '|' 切成若干字段。
 *  这样"一行 = 一个对象"的对应关系永远不会被打乱。
 * ========================================================================== */
namespace FileUtil
{
    // 过滤掉会破坏文件格式的字符（分隔符、换行、制表符），替换成空格
    std::string sanitize(const std::string& s, char delim = '|');

    // 把一整行按 delim 切分成若干字段
    std::vector<std::string> split(const std::string& line, char delim = '|');

    // 字符串 -> 数字（带默认值，读档时遇到坏数据不至于崩溃）
    int    toInt(const std::string& s, int def = 0);
    double toDouble(const std::string& s, double def = 0.0);

    // 读一行，并去掉 Windows 文件里常见的行尾 '\r'
    bool   getLine(std::istream& is, std::string& line);
}
