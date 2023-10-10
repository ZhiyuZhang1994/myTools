/**
 * @brief 用于放置固定位置axes的widget
 * @author zhiyu.zhang@cqbdri.pku.edu.cn
 * @date 2023-08-22
 */

#include "tools/cfgdc/cfgdc.h"
#include "tools/ticker.h"


using namespace ZZY_TOOLS;

class DataCenter : public DataCenterBase {
public:
    DataCenter() = default;

    void setPara(std::uint32_t para) {
        para_ = para;
        
    }

private:
    std::uint32_t para_ = 0;

};

Subject_t ZZY_TEST = 0;
#define ADD_MEMBER_FUNCTION_CALLBACK(subject, callback, ...)                            \
    DataCenterBase* center = DataCenterMgr::instance()->getDataCenter(subject); \
    center->AddObserver(subject, this, callback,  ##__VA_ARGS__);


class A {
public:
    A() {}
    void observer(Subject_t subject, Content_t message) {
        std::cout << "receive subject: " << subject << "msg: " << message << std::endl;
    }
    void init() {
        ADD_MEMBER_FUNCTION_CALLBACK(ZZY_TEST, &A::observer);
        // ADD_MEMBER_FUNCTION_CALLBACK(ZZY_TEST, &A::observer, 20);
    }


};

int main() {
    DataCenter aaaa;
    A a;
    a.init();
}
