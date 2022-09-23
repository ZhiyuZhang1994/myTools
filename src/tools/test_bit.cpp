/**
 * @brief 测试一个数的指定bit位是否为1：最低位是右手第一位，pos为1
 * @author 
 * @date 2022-09-07
 */

#include <utility>
#include <iostream>


// 注：不要对非整型数进行bit位测试
template<typename Type>
static inline bool testBit(Type target, uint32_t pos) {
    return target & (1 << (pos - 1));
}

int main() {
    int temp = 0b0010111010110;
    int pos = 8;

    bool y = testBit(temp, pos);
    std::cout << y << std::endl;
    getchar();
    return 1;
}