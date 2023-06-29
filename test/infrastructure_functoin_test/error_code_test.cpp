/**
 * @file error.h
 * @author Yifan Chen, ZhangWenqian (wqzhang034whu@163.com)
 * @brief 
 * @version 0.1
 * @date 2022-07-10
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef __ERROR_H__
#define __ERROR_H__
#include <iostream>
#include <stdexcept>
#include <string>
#include <exception>
#include <sstream>

// #include <befemcore/tool/types.h>

namespace BeFEM
{
/**
 * @namespace BeFEM::error
 * @brief inner namespace of BeFEM i.e. BeFEM::error
 */
namespace error
{
    // 修改后的异常类
    class BeFEMException : public std::exception {
    public:
        // errCode = 1 为默认错误码
        BeFEMException(std::string msg, std::uint32_t errCode = 1) : msg_(msg), errCode_(errCode) {}

        const char* what() const noexcept override {
            return msg_.c_str();
        }

        std::uint32_t getErrcode() { return errCode_; }

    private:
        std::string msg_; // 异常消息提示
    
    protected:
        std::uint32_t errCode_ = 1; // 给前端的错误码

    };

} // namespace error
} // namespace BeFEM
#endif // __ERROR_H__


// 修改前：
// #define THROW_RUNTIME_ERROR(msg)                                               \
//     BeFEM::error::throwError<std::runtime_error>(__FILE__, __func__, __LINE__, \
//                                                  msg)

// #define THROW_INVALID_ARGUMENT_ERROR(msg)                               \
//     BeFEM::error::throwError<std::invalid_argument>(__FILE__, __func__, \
//                                                     __LINE__, msg)

// 修改后：
// 1、用于保证现有代码可用
#define THROW_RUNTIME_ERROR(msg)                                               \
    std::stringstream ss; \
    ss << "File: " << __FILE__ << "\n\tFunction: " << __func__ << "\n\tLine: " << __LINE__ << "\n\tError: " << msg; \
    BeFEM::error::BeFEMException tmpException(ss.str()); \
    throw std::move(tmpException);

#define THROW_INVALID_ARGUMENT_ERROR(msg)                               \
    std::stringstream ss; \
    ss << "File: " << __FILE__ << "\n\tFunction: " << __func__ << "\n\tLine: " << __LINE__ << "\n\tError: " << msg; \
    BeFEM::error::BeFEMException tmpException(ss.str()); \
    throw std::move(tmpException);

// 之后要使用这个
#define THROW_BEFEM_ERROR(msg, code)                                               \
    std::stringstream ss; \
    ss << "File: " << __FILE__ << "\n\tFunction: " << __func__ << "\n\tLine: " << __LINE__ << "\n\tError: " << msg; \
    BeFEM::error::BeFEMException tmpException(ss.str(), code); \
    throw std::move(tmpException);

// 案例
// 问题1：求解器未返回详细错误码
// 问题2：返回的英文字符串匹配中文文本不方便
void composeStiffnessMatrix() {
    // THROW_RUNTIME_ERROR("zzy!!!");
    THROW_BEFEM_ERROR("zzy!!!", 20);
}

// 后端求解器
std::uint32_t solve() {
    std::cout << "start solve: " << std::endl;
    try {
        // 业务逻辑：组装刚度矩阵
        composeStiffnessMatrix();
    }
    catch(BeFEM::error::BeFEMException& e) {
        //  记录日志
        std::cerr << "solve failed, because: " << e.what() << std::endl;
        // 返回给前端错误码
        return e.getErrcode();
    }
    std::cout << "start success!" << std::endl;
    return 0;
}

int main() {
    // 调用求解器接口
    auto errCode = solve();
    if (errCode == 0) {
        std::cout << "solve success!" << std::endl;
    } else {
        std::cout << "solve failed, because: " << errCode << std::endl;
    }
    return 0;
}




// std::uint32_t BeFEM::intern::LinearElasticityFemSolver2D::solve(const SolverInputV1& solver_inp) {
//     bool solve_status = true;
//     try {
//         std::cout << "Begin set solver_inp"<<std::endl;
//         this->compute_time_.start_time();
//         this->set_solver_inptr(solver_inp);
//         this->compute_time_.end_time();
//         this->compute_time_.compute_set_solver_input_time();
//         std::cout << "set solver_inp time: "<<this->compute_time_.set_solver_input_time()<<"s"<<std::endl;
//         std::cout << "End set solver_inp."<<std::endl;
//         std::cout << "Begin assemble" << std::endl;
//         this->compute_time_.start_time();
//         BeFEM::SpMat A;
//         BeFEM::vec1d b;
//         this->matrix_and_vector(A, b);
//         this->compute_time_.end_time();
//         this->compute_time_.compute_assemble_time();
//         std::cout << "assemble time: "<<this->compute_time_.assemble_time()<<"s"<<std::endl;
//         std::cout << "End assemble." << std::endl;
//         std::cout << "Begin solve linear eqaution" << std::endl;
//         this->compute_time_.start_time();
//         BeFEM::vec1d x = BeFEM::vec1d(b.size(), 0.0);
//         solve_status = BeFEM::lac_solver(A, b, &x, this->solver_inptr_->solver_info_);
//         this->compute_time_.end_time();
//         this->compute_time_.compute_solve_linear_equation_time();
//         std::cout << "solve linear equation time: "<<this->compute_time_.solve_linear_equation_time()<<"s"<<std::endl;
//         std::cout << "End solve linear eqaution." << std::endl;
//         if (solve_status) {
//             std::cout<<"solve successfully!!!"<<std::endl;
//             std::cout<<"Begin write output"<<std::endl;
//             this->write_output(x);
//             std::cout<<"End write output ok"<<std::endl;
//         } else {
//             std::cout<<"solve failure!!!!!"<<std::endl;
//         }
//     }
//     catch(BeFEM::error::BeFEMException& e) {
//         //  记录日志
//         std::cerr << "solve failed, because: " << e.what() << std::endl;
//         // 返回给前端错误码
//         return e.getErrcode();
//     }

//     return solve_status ? 0 : 1; // 成功返回0, 没有抛出异常场景下，但没求解成功返回1
// }