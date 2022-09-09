#include "tools/Singleton.h"
#include "tools/construct_define.h"
#include "tools/class_register.h"
#include "tools/declare_operator_funcs.h"
#include "tools/hash_func.h"
#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <unordered_set>
#include <map>
#include <string>
#include <memory>

// 类自动注册测试类C
class C : public base_class
{
public:
    SINGLETON_CLASS(C);

    C(int x, int y) : base_class(), a(x), b(y) {}

    virtual void initialize() override
    {
        std::cout << "class C initialize!" << std::endl;
    }

    void func1() {
        std::cout << "called C::func1" << std::endl;
    }

private:
    int a;
    int b;
    DISALLOW_COPY_AND_ASSIGN(C);
};

const std::string C_NAME = "service.VTK.camera";
const class_id_t C_ID = GENERATE_SUBJECT_CODE(C_NAME);
DECLARE_CLASS_PTR_TYPE(C);
CLASS_REGISTER(C, C_ID, 1, 2);


// VTK 操作类仓库：

int main()  {

    // vtk操作类初始化
    class_repository::get_instance()->active();

    CPtr ial_tsmode_ptr = GET_CLASS_PTR(C);
    if (!ial_tsmode_ptr) {
        return;
    }
    ial_tsmode_ptr->func1();
    getchar();
}


