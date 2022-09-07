// description: HASH算法
// author: zhangzhiyu
// date: 2022.09.07

#ifndef INCLUDE_TOOLS_HASH_FUNC_H
#define INCLUDE_TOOLS_HASH_FUNC_H

#include <string>

class Hash {
public:
    static std::uint32_t BKDRHash(const std::string& str)
    {
        std::uint32_t seed = 131; // 31 131 1313 13131 131313 etc..
        std::uint32_t hash = 0;

        for(std::size_t i = 0; i < str.length(); i++)
        {
            hash = (hash * seed) + str[i];
        }

        return hash;
    }
    /* End Of BKDR Hash Function */
};

#define GENERATE_SUBJECT_CODE(subjectName) Hash::BKDRHash(subjectName);

const std::string subjectName = "cameraPick.pickPoints";
const std::uint32_t subjectCode = GENERATE_SUBJECT_CODE(subjectName);

#endif