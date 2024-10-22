/**
 * @brief ShiftInvert求特征值
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

using namespace Spectra;

using SpMat = Eigen::SparseMatrix<double>; // 稀疏矩阵类型
using EigenSolver = Eigen::SelfAdjointEigenSolver<SpMat>;
using Eigen::MatrixXd;
using Triplet = Eigen::Triplet<double>;

// 153630 20W网格自由模态
// 19050 2W网格自由模态
// 18618 2W网格约束模态
// 150165 20W网格约束模态
// 167424 20W网格自由模态六面体
int dim_zzy = 153630;
std::uint32_t para = 0;

int main() {
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

    matK = matK + para * matM;
    using OpType = SymShiftInvert<double>;
    // SparseSymMatProd<double> op(matK);
    // SparseCholesky
    // using BOpType = SparseSymMatProd<double>;
    using BOpType = SparseSymMatProd<double>; // 2

    OpType op(matK, matM);
    BOpType Bop(matM);
    // SymGEigsShiftSolver<OpType, BOpType, GEigsMode::Buckling> geigs(op, Bop, 20, 40, 1.0);
    SymGEigsShiftSolver<OpType, BOpType, GEigsMode::ShiftInvert> geigs(op, Bop, 20, 400, 0); // 2
    geigs.init();
    int nconv = geigs.compute(SortRule::LargestMagn , 1000, 1e-3, SortRule::SmallestMagn);
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
        std::cout << sqrt(each - para) / 2.0 / M_PI << std::endl;
    }
    std::cout << "Number of converged generalized eigenvalues: " << nconv << std::endl;
    std::cout << "Generalized eigenvalues found:\n" << evalues << std::endl;
    // std::cout << "Generalized eigenvectors found:\n" << evecs.topRows(10) << std::endl;
 

    // SymGEigsSolver GEigsMode::Cholesky SparseCholesky




    return 0;

}
