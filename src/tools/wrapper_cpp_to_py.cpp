/**
 * @brief 测试一个数的指定bit位是否为1：最低位是右手第一位，pos为1
 * @author 
 * @date 2022-09-07
 */

#include <iostream>
#include "pybind11.h"

class Test{
public:
    Test( int i, int j )
    :mI(i),mJ(j){

    }

    void Print(){
        std::cout <<"i= " << mI <<" j= " <<mJ << std::endl;
    }
private:
    int mI;
    int mJ;
};

PYBIND11_MODULE(zzy, m ){
    m.doc() = "pybind11 example";
    pybind11::class_<Test>(m, "Test" )
        .def( pybind11::init< int , int >() )  //构造器的模版参数列表中需要按照构造函数的参数类型填入才能调用对应的参数
        .def( "print", &Test::Print );
}
