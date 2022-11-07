/**
 * @brief 功能测试
 * @author zhangzhiyu
 * @date 2022-09-16
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

int main()
{
    std::vector<std::string> v{"111","222", "333", "444", "555", "666", "777"};

    std::string x = "555";
    std::cout << *find_if(v.begin(), v.end(), [x](std::string& tmp)->bool { return (tmp == x)··; };) << std::endl;


    getchar();
    return 0;
}