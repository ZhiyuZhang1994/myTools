#include "tools/notifier_and_observer.h"
#include "tools/hash_func.h"
#include <boost/serialization/string.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/iostreams/stream.hpp>
#include <iostream>
#include <queue>
#include <string>
#include <sstream>

const std::string theme = "test_observer_func";
const Observer_ZZY::Subject_t  subject = GENERATE_SUBJECT_CODE(theme);

class ZZY1 {
public:
    void initialize() {
        Observer_ZZY::Observer ob;
        
        // 证明一个主题可以有多个回调
        handler = ob.attach(subject, [this](Observer_ZZY::Subject_t subject, Observer_ZZY::Content_t message) { 
            cb1(subject, message); }
        );
        handler2 = ob.attach(subject, [this](Observer_ZZY::Subject_t subject, Observer_ZZY::Content_t message) { 
            cb2(subject, message); }
        );
    }

    void cb1(Observer_ZZY::Subject_t subject, Observer_ZZY::Content_t message) {
        int d2 = 0;
        double d4 = 0;
        std::istringstream is(message);
        boost::archive::text_iarchive  ia(is);
        ia >> d2 >> d4;

        std::cout << "cb1 receive notified message, subject  is: " << subject << std::endl;
        std::cout <<  "message is " << d2 << " " << d4 << std::endl;
    }

    void cb2(Observer_ZZY::Subject_t subject, Observer_ZZY::Content_t message) {
        int d2 = 0;
        double d4 = 0;
        std::istringstream is(message);
        boost::archive::text_iarchive  ia(is);
        ia >> d2 >> d4;

        std::cout << "cb2 receive notified message, subject  is: " << subject << std::endl;
        std::cout <<  "message is " << d2 << " " << d4 << std::endl;
    }


    Observer_ZZY::CallbackHandler handler;
    Observer_ZZY::CallbackHandler handler2;
};


// 观察者机制使用案例
void test_notifier_and_observer()
{
    // 1、建立主题
    Observer_ZZY::Notifier notifier;
    notifier.buildSubject(subject);

    // 2、观察主题
    ZZY1 zzy1;
    zzy1.initialize();

    // 3、通知主题
    std::ostringstream message;
    boost::archive::text_oarchive  oa(message);
    int d1 = 2;
    double d3 = 2176.2;
    oa << d1 << d3;

    // 3.1、通知者先查询主题是否存在：
    auto subjectContent = notifier.querySubject(subject);
    if (subjectContent ==  nullptr) {
        std::cout << "subject: " << theme << " not exist" << std::endl;
    }

    // 3.2、主题查询观察者是否存在：
    if (!subjectContent->isObserved()) {
        std::cout << "subject: " << theme << " is not observed" << std::endl;
    }
    // 3.3、主题发布通知
    subjectContent->notify(message.str());

    // 4、严重注销功能是否可用

    // 4.1、注销观察者
    Observer_ZZY::Observer ob;
    ob.detach(subject, zzy1.handler);
    subjectContent->notify(message.str());

    // 4.2、注销主题：此时查询主题为空指针
    notifier.releaseSubject(subject);
    if (notifier.querySubject(subject) ==  nullptr) {
        std::cout << "subject: " << theme << " not exist" << std::endl;
    }
}

int main()
{
    test_notifier_and_observer();
} 