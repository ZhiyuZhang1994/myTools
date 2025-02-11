/**
 * @brief 基于ShiftInvert求指定范围内所有特征值
 * @brief 20W自由模态前20阶完全正确
 * @date 2024-10-22
 */
#include <Eigen/Dense>
#include <Eigen/Eigenvalues>
#include <Eigen/Sparse>
#include <fstream>
#include <Eigen/Core>
#include <Spectra/SymEigsSolver.h>
#include <Eigen/SparseCore>
#include <Spectra/SymGEigsShiftSolver.h>
#include <Spectra/MatOp/SymShiftInvert.h>
#include <Spectra/MatOp/SparseSymMatProd.h>
#include <iostream>
#include <Spectra/MatOp/SparseCholesky.h>
#include <Spectra/SymGEigsSolver.h>
#include <chrono>
#include <omp.h>
using namespace Spectra;

using SpMat = Eigen::SparseMatrix<double>; // 稀疏矩阵类型
using Eigen::MatrixXd;
using Triplet = Eigen::Triplet<double>;

// 153630 20W网格自由模态
// 19050 2W网格自由模态
// 18618 2W网格约束模态
// 150165 20W网格约束模态
// 167424 20W网格自由模态六面体
int dim_zzy = 18618;
std::uint32_t shift = 0;

int main() {
    std::cout << "omp_get_thread_num " << omp_get_thread_num() << std::endl;
    std::cout << "omp_get_num_threads " << omp_get_max_threads() << std::endl; // 获取最大可用线程数，可在程序中用于确定潜在的最大并行能力，不需要在并行区域内使用。
    std::cout << "omp_get_num_procs " << omp_get_num_procs() << std::endl; // 返回系统的逻辑核心数，即可用的处理器数量。
    omp_set_num_threads(omp_get_num_procs() / 2); // 根据需要设置线程数:设置为系统逻辑核心数的一半
    std::cout << "omp_get_thread_num " << omp_get_thread_num() << std::endl;
    std::cout << "omp_get_num_threads " << omp_get_max_threads() << std::endl;
    std::cout << "omp_get_num_procs " << omp_get_num_procs() << std::endl; // 返回系统的逻辑核心数，即可用的处理器数量。
    std::vector<Triplet> coefficients;            // list of non-zeros coefficients
    std::ifstream infile("K.dat");
    if (!infile) {
        std::cerr << "无法打开文件!" << std::endl;
        return 1;
    }
    // 从文件中逐行读取
    std::uint32_t a, b; double c;
    while (infile >> a >> b >> c) {
        coefficients.emplace_back(a - 1, b - 1, c);
    }
    SpMat matK(dim_zzy, dim_zzy);
    matK.setFromTriplets(coefficients.begin(), coefficients.end());
    infile.close();
    coefficients.clear();

    std::ifstream infileM("M.dat");
    if (!infileM) {
        std::cerr << "无法打开文件!" << std::endl;
        return 1;
    }
    while (infileM >> a >> b >> c) {
        coefficients.emplace_back(a - 1, b - 1, c);
    }
    SpMat matM(dim_zzy, dim_zzy);
    matM.setFromTriplets(coefficients.begin(), coefficients.end());
    infileM.close();
    coefficients.clear();
    std::cout << "read file finished" << std::endl;

    matK = matK + shift * matM;
    using OpType = SymShiftInvert<double>;
    using BOpType = SparseSymMatProd<double>;
    auto start = std::chrono::high_resolution_clock::now();

    OpType op(matK, matM);
    BOpType Bop(matM);

    // ----------------------------------求解指定范围所有特征值----------------------------------
    double startNum = 6.98052e+08;
    double currNum = startNum;
    double endNum = 2.10000e+09;
    int eachSearchNum = 10; // 这个参数需要根据实际特征值分布进行调整，也可以在循环中动态调整
    bool eachSearchRes = true;
    Eigen::VectorXd evalues;
    Eigen::MatrixXd evecs;

    int count = 1;
    do {
        std::cout << "currIter: " << count << std::endl;
        SymGEigsShiftSolver<OpType, BOpType, GEigsMode::ShiftInvert> geigs(op, Bop, eachSearchNum, 400, currNum); // 2
        geigs.init();
        int nconv = geigs.compute(SortRule::LargestAlge ,500, 1e-3, SortRule::SmallestMagn);
        // evalues = geigs.eigenvalues();
        // evecs = geigs.eigenvectors();
        evalues.conservativeResize(evalues.size() + geigs.eigenvalues().size());
        evalues.tail(geigs.eigenvalues().size()) = geigs.eigenvalues();
        evecs.conservativeResize(evecs.rows(), evecs.cols() + geigs.eigenvectors().cols());
        evecs.rightCols(geigs.eigenvectors().cols()) = geigs.eigenvectors();

        count++;
        eachSearchRes = geigs.info() == CompInfo::Successful ? true : false;
        currNum = evalues(evalues.size() - 1);
    } while (eachSearchRes && currNum < endNum);
    




    for (auto each : evalues) {
        std::cout << sqrt(each - shift) / 2.0 / M_PI << std::endl;
    }
    std::cout << "Number of converged generalized eigenvalues: " << nconv << std::endl;
    std::cout << "Generalized eigenvalues found:\n" << evalues << std::endl;

    // 获取结束时间点
    auto end = std::chrono::high_resolution_clock::now();

    // 计算时间间隔
    std::chrono::duration<double, std::milli> duration = end - start;
    // 输出执行时间
    std::cout << "Function execution time: " << duration.count() << " ms" << std::endl;
    return 0;

}

