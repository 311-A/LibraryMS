#pragma once
#include <iostream>
#include <string>

class Resource;

/* ==========================================================================
 *  ResourceFactory —— 资源工厂【实验六新增】
 * --------------------------------------------------------------------------
 *  它回答实验六的思考题：
 *      "如何做到既要保存基类对象，又要保存派生类对象呢？"
 *
 *  困难在哪？
 *    写文件时容易：一句 r->save(ofs) 就能靠虚函数写出各自类型的内容。
 *    读文件时难：程序看到一行文本，并不知道该 new Book 还是 new Magazine。
 *
 *  解决办法：
 *    ① 存的时候在每行开头写上一个【类型标记】（BOOK / MAG / ERES），
 *       这个标记来自虚函数 typeCode()，所以写文件的人不用关心类型。
 *    ② 读的时候先读出标记，交给工厂 create() 判断该 new 哪个派生类，
 *       再把剩下的字段交给这个新对象的 loadFromLine() 去填。
 *
 *  于是"基类对象 / 派生类对象"都能在同一个文件里被完整还原，
 *  而且还原出来的是真正的派生类对象（多态性质没有丢）。
 * ========================================================================== */
class ResourceFactory
{
public:
    // 按类型标记创建一个"空"的派生类对象；标记不认识就返回 nullptr
    static Resource* create(const std::string& typeCode);

    // 已知一行文本，直接造出对应的对象并填好数据（工厂 + 多态读取）
    static Resource* createFromLine(const std::string& line);

    // 从流里读一行并创建对象；读到空行/坏行返回 nullptr
    static Resource* createFromStream(std::istream& is);
};
