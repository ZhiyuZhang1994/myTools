/**
 * @brief 打印程序异常退出时的调用栈信息
 * @author zhangzhiyu
 * @date 2022-11-07
 */

#include <utility>
#include <iostream>
#include <vector>

class StackTrace {
public:
    static const std::uint16_t MAX_STACK = 64;
    static const std::uint16_t STACK_START = 2; // 忽略最上层的打印入口

    class StackTraceEntry {
    public:
        StackTraceEntry(std::size_t index, const std::string &loc, const std::string &demang, const std::string &hex,
            const std::string &addr);

        StackTraceEntry(std::size_t index, const std::string &loc) : m_index(index), m_location(loc) {}

        std::size_t m_index;
        std::string m_location;
        std::string m_demangled;
        std::string m_hex;
        std::string m_addr;
        friend std::ostream &operator<<(std::ostream &ss, const StackTraceEntry &si);

    private:
        StackTraceEntry();
    };

    StackTrace() { generateStack(); }
    ~StackTrace() = default;

    inline std::vector<StackTraceEntry> &getLatestStack()
    {
        return m_stack;
    }

    friend std::ostream &operator<<(std::ostream &os, const StackTrace &st);

private:
    void generateStack();

private:
    std::vector<StackTraceEntry> m_stack;
};