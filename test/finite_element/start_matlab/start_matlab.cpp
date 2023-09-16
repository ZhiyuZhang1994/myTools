#include <iostream>
#include <cstdlib>
#include <windows.h>

// int main() {
//     // 启动.bat文件
//     int result = system("start /b D:\\work_station\\github_project\\func_test\\test\\finite_element\\start_matlab\\start_matlab.bat");
//     if (result == 0) {
//         std::cout << "批处理脚本启动成功。" << std::endl;

//     } else {
//         std::cerr << "批处理脚本启动失败。" << std::endl;
//     }


//     return 0;
// }

int main() {
    const wchar_t* vbsScriptPath = L"D:\\work_station\\github_project\\func_test\\test\\finite_element\\start_matlab\\start_matlab.vbs";

    HINSTANCE hInstance = ShellExecute(NULL, L"open", L"wscript.exe", vbsScriptPath, NULL, SW_SHOWNORMAL);
    intptr_t result = reinterpret_cast<intptr_t>(hInstance);
    if (result > 32) {
        std::cout << "VBS脚本已成功启动。" << std::endl;
    } else {
        std::cout << "无法启动VBS脚本。错误代码: " << result << std::endl;
    }

    return 0;
}