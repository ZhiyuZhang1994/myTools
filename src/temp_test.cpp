/**
 * @brief SparseCholesky分解求特征值
 * 1) RegularInverse求特征值不好用
 * @brief 2W约束模态前20阶完全正确
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
int dim_zzy = 150165;
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
    using OpType = SparseSymMatProd<double>;
    // SparseSymMatProd<double> op(matK);
    // SparseCholesky
    // using BOpType = SparseSymMatProd<double>;
    using BOpType = SparseCholesky<double>; // 2

    auto start = std::chrono::high_resolution_clock::now();
    OpType op(matK);
    BOpType Bop(matM);
    // SymGEigsShiftSolver<OpType, BOpType, GEigsMode::Buckling> geigs(op, Bop, 20, 40, 1.0);
    SymGEigsSolver<OpType, BOpType, GEigsMode::Cholesky> geigs(op, Bop, 20, 200); // 2
    geigs.init();
    double tolerance = 1e-1;
    int nconv = geigs.compute(SortRule::SmallestMagn, 1000, tolerance, SortRule::SmallestMagn);
    // int nconv = geigs.compute(SortRule::SmallestMagn);
 
    // Retrieve results
    Eigen::VectorXd evalues;
    Eigen::MatrixXd evecs;
    if (geigs.info() == CompInfo::Successful) {
        std::cout << "zzy !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!successful" << std::endl;
    } else {
        std::cout << "zzy -----------------------------failed" << std::endl;
    }
    evalues = geigs.eigenvalues();
    evecs = geigs.eigenvectors();
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
