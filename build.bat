@echo off
chcp 936 >nul
REM ============================================================
REM  一键编译脚本（MinGW / g++）
REM  实验五：vector 与模板    实验六：输入 / 输出流
REM  -finput-charset=UTF-8 : 源文件本身是 UTF-8 编码
REM  -fexec-charset=GBK    : 控制台是 GBK，保证中文正常显示
REM ============================================================
g++ -std=c++14 -finput-charset=UTF-8 -fexec-charset=GBK -Wall -o LibraryMS.exe ^
    InputUtil.cpp FileUtil.cpp ^
    Resource.cpp Book.cpp Magazine.cpp EResource.cpp ResourceFactory.cpp ^
    Student.cpp Library.cpp main.cpp

if errorlevel 1 (
    echo.
    echo 编译失败，请查看上面的错误信息。
) else (
    echo.
    echo 编译成功！运行 LibraryMS.exe 即可。
    echo 数据保存在 library_data.txt，删除该文件即可恢复示例数据。
)
pause
