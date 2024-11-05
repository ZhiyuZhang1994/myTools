/**
 * @brief SymShiftInvert求解接近某个已知特征值的特征值
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
    omp_set_num_threads(16); // 根据需要设置线程数
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

    SymGEigsShiftSolver<OpType, BOpType, GEigsMode::ShiftInvert> geigs(op, Bop, 3, 400, 6.98052e+08); // 2
    geigs.init();
	// 第一个参数对应的结果：
	// SortRule::LargestMagn：计算出大于目标值的指定个数特征值 6.98163e+08 7.82832e+08 8.4073e+08
	// SortRule::BothEnds：计算出特征值左右两侧指定个数的特征值 4.85048e+08 6.98163e+08 7.82832e+08
    int nconv = geigs.compute(SortRule::BothEnds , 1000, 1e-6, SortRule::SmallestMagn);
 
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
