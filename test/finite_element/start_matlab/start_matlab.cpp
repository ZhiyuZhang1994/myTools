#include <iostream>
#include <cstdlib>
#include <windows.h>

int main() {
    // 启动.bat文件
    int result = system("D:\\work_station\\github_project\\func_test\\src\\start_matlab.bat");

    if (result == 0) {
        std::cout << "批处理脚本启动成功。" << std::endl;
    } else {
        std::cerr << "批处理脚本启动失败。" << std::endl;
    }

    
    return 0;
}