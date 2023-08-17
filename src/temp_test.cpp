#include <iostream>
#include <utility>
#include "tools/class_register.h"
#include "tools/gui_common_define.h"

#include <chrono>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace std;

int main() {
    base_class a;
    a.init(InitStage::INIT_SELF);
    getchar();
    return 1;
}