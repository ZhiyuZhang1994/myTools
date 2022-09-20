// description: HASH算法
// author: zhangzhiyu
// date: 2022.09.07

#ifndef INCLUDE_TOOLS_HASH_FUNC_H
#define INCLUDE_TOOLS_HASH_FUNC_H

#include <string>
#include <functional>

class Hash {
public:
    static std::uint32_t BKDRHash(const std::string& str)
    {
        std::uint32_t seed = 131;
        std::uint32_t hash = 0;

        for(std::size_t i = 0; i < str.length(); i++)
        {
            hash = (hash * seed) + str[i];
        }

        return hash;
    }
};

#define GENERATE_SUBJECT_CODE(subjectName) Hash::BKDRHash(subjectName)
#define GENERATE_SERVICE_ID(service) std::hash<std::string>()(service)

#endif