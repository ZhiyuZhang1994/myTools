/**
 * @brief 写文件测试
 * @author zhangzhiyu
 * @date 2023-04-27
 */

#include <vector>
#include <fstream>
#include <iomanip> // for std::setw

int main()
{
    // 要写入的数据
    std::vector<double> arr = {1.23, 4.56, 7.89, 0.12, 12, 23, 12.4, 234, 3, 45, 45.4, 125};

    std::fstream fout;
    fout.open("abc.txt", std::ios_base::out); // 以写入的形式打开文件"abc.txt", 不存在则创建
        for (std::uint32_t i = 0; i < arr.size(); ++i) {
            if (i % 5 == 0) { // 每隔5个数据换一行
                fout << std::endl;
            }
            fout << std::setw(4) << i << ": " << std::setw(15) << arr[i] << ", "; // std::setw设置位宽，即每个数据间的间隔
        }
    fout.close(); // 显式的关闭流到文件的连接。

    getchar();
    return 0;
}