// /**
//  * @brief 打印程序异常退出时的调用栈信息
//  * @author zhangzhiyu
//  * @date 2022-11-07
//  */

// #include "tools/stack_track.h"
// #include <execinfo.h>

// // StackTrace
// StackTraceEntry::StackTraceEntry(std::size_t index, const std::string &loc, const std::string &demang,
//     const std::string &hex, const std::string &addr) : m_index(index), m_location(loc), m_demangled(demang),
//     m_hex(hex), m_addr(addr) {
// }

// std::ostream &operator<<(std::ostream &ss, const StackTrace::StackTraceEntry &si)
// {
//     ss << "[" << si.m_index << "] " << si.m_location << (si.m_hex.empty() ? "" : "+") << si.m_hex << " " << si.m_addr <<
//         (si.m_demangled.empty() ? "" : ":") << si.m_demangled;
//     return ss;
// }

// std::ostream &operator<<(std::ostream &os, const StackTrace &st)
// {
//     std::vector<StackTrace::StackTraceEntry>::const_iterator it = st.m_stack.begin();
//     while (it != st.m_stack.end()) {
//         os << "    " << *it++ << "\n";
//     }
//     return os;
// }


//         for (std::size_t i = kStackStart; i < size; ++i)
//         {
//             std::string mangName;
//             std::string location;
//             std::string hex;
//             std::string addr;

//             // entry: 2   crash.cpp.bin                       0x0000000101552be5 _ZN2el4base5debug10StackTraceC1Ev + 21
//             const std::string line(strings[i]);
//             auto p = line.find("_");
//             if (p != std::string::npos)
//             {
//                 mangName = line.substr(p);
//                 mangName = mangName.substr(0, mangName.find(" +"));
//             }
//             p = line.find("0x");
//             if (p != std::string::npos)
//             {
//                 addr = line.substr(p);
//                 addr = addr.substr(0, addr.find("_"));
//             }
//             // Perform demangling if parsed properly
//             if (!mangName.empty())
//             {
//                 int status = 0;
//                 char *demangName = abi::__cxa_demangle(mangName.data(), 0, 0, &status);
//                 // if demangling is successful, output the demangled function name
//                 if (status == 0)
//                 {
//                     // Success (see http://gcc.gnu.org/onlinedocs/libstdc++/libstdc++-html-USERS-4.3/a01696.html)
//                     StackTraceEntry entry(i - 1, location, demangName, hex, addr);
//                     m_stack.push_back(entry);
//                 }
//                 else
//                 {
//                     // Not successful - we will use mangled name
//                     StackTraceEntry entry(i - 1, location, mangName, hex, addr);
//                     m_stack.push_back(entry);
//                 }
//                 free(demangName);
//             }
//             else
//             {
//                 StackTraceEntry entry(i - 1, line);
//                 m_stack.push_back(entry);
//             }
//         }
//     }
//     free(strings);


// void StackTrace::generateStack()
// {
//     m_stack.clear();
//     void *stack[MAX_STACK];
//     std::uint16_t count = backtrace(stack, MAX_STACK);
//     char **strings = backtrace_symbols(stack, count);
//     for (std::size_t i = STACK_START; i < count; ++i)
//     {
//         std::string mangName;
//         std::string location;
//         std::string hex;
//         std::string addr;

//         // entry: 2   crash.cpp.bin                       0x0000000101552be5 _ZN2el4base5debug10StackTraceC1Ev + 21
//         const std::string line(strings[i]);
//         auto p = line.find("_");
//         if (p != std::string::npos)
//         {
//             mangName = line.substr(p);
//             mangName = mangName.substr(0, mangName.find(" +"));
//         }
//         p = line.find("0x");
//         if (p != std::string::npos)
//         {
//             addr = line.substr(p);
//             addr = addr.substr(0, addr.find("_"));
//         }
//         // Perform demangling if parsed properly
//         if (!mangName.empty())
//         {
//             int status = 0;
//             char *demangName = abi::__cxa_demangle(mangName.data(), 0, 0, &status);
//             // if demangling is successful, output the demangled function name
//             if (status == 0)
//             {
//                 // Success (see http://gcc.gnu.org/onlinedocs/libstdc++/libstdc++-html-USERS-4.3/a01696.html)
//                 StackTraceEntry entry(i - 1, location, demangName, hex, addr);
//                 m_stack.push_back(entry);
//             }
//             else
//             {
//                 // Not successful - we will use mangled name
//                 StackTraceEntry entry(i - 1, location, mangName, hex, addr);
//                 m_stack.push_back(entry);
//             }
//             free(demangName);
//         }
//         else
//         {
//             StackTraceEntry entry(i - 1, line);
//             m_stack.push_back(entry);
//         }
//     }
//     free(strings);
// }