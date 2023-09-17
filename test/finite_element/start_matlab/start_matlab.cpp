#include <iostream>
#include <cstdlib>
#include <windows.h>
#include <filesystem>
#include <chrono>
#include <thread>
#include <shellapi.h>
// #########################################方案一：用cmd调用bat脚本：会弹出cmd窗口，但是串行
// int main() {
//     // 启动.bat文件
//     int result = system("D:\\work_station\\github_project\\func_test\\test\\finite_element\\start_matlab\\start_matlab.bat");
//     if (result == 0) {
//         std::cout << "批处理脚本启动成功。" << std::endl;
//     } else {
//         std::cerr << "批处理脚本启动失败。" << std::endl;
//     }
//     return 0;
// }

// #########################################方案二：用cmd调用vbs脚本：会弹出cmd窗口，是串行
// int main() {
//     // 指定要运行的VBScript脚本文件路径
//     const char* vbsScript = "D:\\work_station\\github_project\\func_test\\test\\finite_element\\start_matlab\\start_matlab.vbs"; 
//     // 构建执行命令
//     std::string command = "cscript.exe /nologo " + std::string(vbsScript);
//     // 使用system函数执行VBScript脚本，并等待其完成
//     int returnCode = system(command.c_str());
//     // 检查VBScript脚本的返回值
//     if (returnCode == 0) {
//         std::cout << "VBScript脚本成功执行。" << std::endl;
//     } else {
//         std::cout << "VBScript脚本执行失败。" << std::endl;
//     }
//     return 0;
// }

// #########################################结论：system是串行调用、system要弹出cmd窗口############################################################




// ######################################### 方案三：用ShellExecute调用vbs脚本：实现不弹出cmd窗口，但不是串行
// int main() {
//     const wchar_t* vbsScriptPath = L"D:\\work_station\\github_project\\func_test\\test\\finite_element\\start_matlab\\start_matlab.vbs";

//     HINSTANCE hInstance = ShellExecute(NULL, L"open", L"wscript.exe", vbsScriptPath, NULL, SW_HIDE);
//     intptr_t result = reinterpret_cast<intptr_t>(hInstance);
//     if (result > 32) {
//         std::cout << "VBS脚本已成功启动。" << std::endl;
//     } else {
//         std::cout << "无法启动VBS脚本。错误代码: " << result << std::endl;
//     }

//     return 0;
// }

// ######################################### 方案四：用ShellExecuteEx调用vbs脚本：实现不弹出cmd窗口，也是串行
int main() {
    SHELLEXECUTEINFO sei = {sizeof(SHELLEXECUTEINFO)};
    sei.fMask = SEE_MASK_NOCLOSEPROCESS;                                                                               // 指定要等待进程结束
    sei.lpFile = L"D:\\work_station\\github_project\\func_test\\test\\finite_element\\start_matlab\\start_matlab.vbs";
    sei.nShow = SW_HIDE;                                                                                               // 隐藏窗口
    auto result = ShellExecuteEx(&sei);

    if (!result) {
        std::cout << "无法启动VBS脚本。错误代码: " << result << std::endl;
    }
    std::cout << "VBS脚本已成功启动。" << std::endl;
    WaitForSingleObject(sei.hProcess, INFINITE); // 等待执行完成
    CloseHandle(sei.hProcess);
    std::cout << "VBS脚本执行完成" << std::endl;
}
// #########################################结论：ShellExecute是异步调用、ShellExecuteEx是同步调用，两个均不弹出cmd窗口#######################################################
