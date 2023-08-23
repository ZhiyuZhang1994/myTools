/**
 * @brief 此工具实现了观察者机制(同进程内交互):客户不需要针对业务特性实现观察者、通知者类，是一个即拿即用的工具
 * @brief 观察者与通知者做到完全解耦
 * @author zhiyu.zhang@cqbdri.pku.edu.cn
 * @date 2022-09-08
 */

#ifndef SUPPORT_BOOST_SERIALIZE_DEFINE
#define SUPPORT_BOOST_SERIALIZE_DEFINE

#include <boost/serialization/string.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/iostreams/stream.hpp>
#include <iostream>
#include <functional>
#include <string>

namespace ZZY_TOOLS
{
using InputWrapper = boost::archive::text_iarchive;
using OutputWrapper = boost::archive::text_oarchive;

#define DEFINE_MESSAGE_INPUT_WRAPPER(message) \
    std::stringstream is(message); \
    InputWrapper message##In(is)

#define DEFINE_MESSAGE_OUTPUT_WRAPPER(message) \
    std::stringstream message; \
    OutputWrapper message##Out(message)

}


#endif
