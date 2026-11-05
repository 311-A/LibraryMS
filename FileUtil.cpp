#include "FileUtil.h"
#include <cstdlib>

std::string FileUtil::sanitize(const std::string& s, char delim)
{
    std::string r = s;
    for (size_t i = 0; i < r.size(); ++i)
    {
        // 中文是多字节字符，每个字节都 >= 0x80，不会和这几个 ASCII 符号冲突
        if (r[i] == delim || r[i] == '\n' || r[i] == '\r' || r[i] == '\t')
            r[i] = ' ';
    }
    return r;
}

std::vector<std::string> FileUtil::split(const std::string& line, char delim)
{
    std::vector<std::string> fields;
    std::string cur;
    for (size_t i = 0; i < line.size(); ++i)
    {
        if (line[i] == delim)
        {
            fields.push_back(cur);
            cur.clear();
        }
        else
        {
            cur += line[i];
        }
    }
    fields.push_back(cur);          // 最后一个分隔符后面那一段也要收进来
    return fields;
}

int FileUtil::toInt(const std::string& s, int def)
{
    if (s.empty()) return def;
    return std::atoi(s.c_str());
}

double FileUtil::toDouble(const std::string& s, double def)
{
    if (s.empty()) return def;
    return std::atof(s.c_str());
}

bool FileUtil::getLine(std::istream& is, std::string& line)
{
    if (!std::getline(is, line))
        return false;
    if (!line.empty() && line[line.size() - 1] == '\r')
        line.erase(line.size() - 1);        // 去掉 Windows 行尾的 \r
    return true;
}
