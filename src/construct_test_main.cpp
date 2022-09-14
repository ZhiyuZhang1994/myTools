#include <iostream>

using namespace std;
 
class A {
public:
	int x;
    A()
    {
		cout << "default Constructor" << endl;
    }
	A(int x) : x(x)
	{
		cout << "Constructor" << endl;
	}
	A(A& a) : x(a.x)
	{
		cout << "Copy Constructor" << endl;
	}
	A& operator=(A& a)
	{
		x = a.x;
		cout << "Copy Assignment operator" << endl;
		return *this;
	}
	A(A&& a) : x(a.x)
	{
		cout << "Move Constructor" << endl;
	}
	A& operator=(A&& a)
	{
		x = a.x;
		cout << "Move Assignment operator" << endl;
		return *this;
	}
};
 

#include <mutex>
#include <sys/param.h>
#include <thread>
#include <atomic>

class BaseApp {
public:
    BaseApp();
    explicit BaseApp(A& a);
    explicit BaseApp(A&& a);

    virtual ~BaseApp() = default;

protected:
    A a_;
    std::atomic<bool> running_{false};
};

BaseApp ::BaseApp() : running_(false) {
};

BaseApp::BaseApp(A& a) : a_(a), running_(false) {}
BaseApp::BaseApp(A&& a) : a_(std::move(a)), running_(false) {}


int main()
{
    A a;
    a.x = 1;
    // 传递左值：则一次拷贝一次移动
    auto aa = std::move(a);
    BaseApp base(aa);
 
    // 传递右值：则两次移动
    A a2;
    BaseApp base2(std::move(a2));
    //参考资料：https://www.zhihu.com/question/37834703
    getchar();
	return 0;
}