/**
 * @brief 缩进测试
 * @author zhangzhiyu
 * @date 2023-01-05
 */

#include "tools/tassk_class.h"
#include "tools/msg_service.h"
#include "tools/table_driven.h"

#include <utility>
#include <iostream>
#include <signal.h>
#include <iostream>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <algorithm>
#include <string>
#include <sstream>

#define VTK_NUMBER_OF_BLANKS 40
static const char blanks[VTK_NUMBER_OF_BLANKS + 1] = "                                        ";


int main()
{
    auto x =  blanks + (VTK_NUMBER_OF_BLANKS - 2);
    std::cout << blanks + (VTK_NUMBER_OF_BLANKS - 2) << "zzy" << std::endl;
    std::cout << blanks + (VTK_NUMBER_OF_BLANKS - 20) << "zzy" << std::endl;
    std::stringstream dump;
    dump << "zzy \n" << "zzy2" << "\n" << "zzy3";

    std::cout << dump.str() << std::endl;
    getchar();
    return 0;
}