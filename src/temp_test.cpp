
#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <unordered_set>
#include <map>
#include <string>
#include <memory>
#include <iostream> 
#include <boost/serialization/string.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/serialization/version.hpp> 
#include <sstream>


void TestArchive1(std::string content)
{
    int d2 = 0;
    double d4 = 0;
	std::istringstream is(content);
	boost::archive::binary_iarchive ia(is);
	ia >> d2 >> d4;//从一个保存序列化数据的string里面反序列化，从而得到原来的对象。
 
	std::cout << "data : " << d2 << " "<< d4 << "\n";
}

int main() 
{
    int d1 = 2;
    double d3 = 2176.2;

	std::ostringstream os;
	boost::archive::binary_oarchive oa(os);
	oa << d1 << d3;//序列化到一个ostringstream里面
 
	std::string content = os.str(); //content保存了序列化后的数据。
    std::cout << content << std::endl;
    TestArchive1(content);
} 