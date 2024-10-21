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

using namespace Spectra;

using SpMat = Eigen::SparseMatrix<double>; // 稀疏矩阵类型
using EigenSolver = Eigen::SelfAdjointEigenSolver<SpMat>;
using Eigen::MatrixXd;
using Triplet = Eigen::Triplet<double>;

// 153630 20W网格自由模态
// 18618 2W网格约束模态
int main() {
    std::vector<Triplet> coefficients;            // list of non-zeros coefficients
    std::ifstream infile("K.mtx");
    if (!infile) {
        std::cerr << "无法打开文件!" << std::endl;
        return 1;
    }
    // 从文件中逐行读取
    std::uint32_t a, b; double c;
    while (infile >> a >> b >> c) {
        coefficients.emplace_back(a - 1, b - 1, c);
    }
    SpMat matK(153630, 153630);
    matK.setFromTriplets(coefficients.begin(), coefficients.end());
    infile.close();

    std::ifstream infileM("M.mtx");
    if (!infileM) {
        std::cerr << "无法打开文件!" << std::endl;
        return 1;
    }
    while (infileM >> a >> b >> c) {
        coefficients.emplace_back(a - 1, b - 1, c);
    }
    SpMat matM(153630, 153630);
    matM.setFromTriplets(coefficients.begin(), coefficients.end());
    infileM.close();
    std::cout << "read file finished" << std::endl;
    using OpType = SymShiftInvert<double, Eigen::Sparse, Eigen::Sparse>;
    using BOpType = SparseSymMatProd<double>;
    OpType op(matK, matM);
    BOpType Bop(matK);
    SymGEigsShiftSolver<OpType, BOpType, GEigsMode::Buckling> geigs(op, Bop, 20, 40, 1.0);
    geigs.init();
    // int nconv = geigs.compute(SortRule::LargestAlge, 1000, 1e-10, SortRule::SmallestMagn);
    int nconv = geigs.compute(SortRule::LargestAlge);
 
    // Retrieve results
    Eigen::VectorXd evalues;
    Eigen::MatrixXd evecs;
    if (geigs.info() == CompInfo::Successful) {
        std::cout << "zzy !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!successful" << std::endl;
        evalues = geigs.eigenvalues();
        evecs = geigs.eigenvectors();
    }
    std::cout << "Number of converged generalized eigenvalues: " << nconv << std::endl;
    std::cout << "Generalized eigenvalues found:\n" << evalues << std::endl;
    // std::cout << "Generalized eigenvectors found:\n" << evecs.topRows(10) << std::endl;
 



    // // ###########################################################################################################
    // // We are going to solve the generalized eigenvalue problem
    // //     A * x = lambda * B * x,
    // // where A is symmetric and B is positive definite
    // const int n = 100;
 
    // // Define the A matrix
    // Eigen::MatrixXd M = Eigen::MatrixXd::Random(n, n);
    // Eigen::MatrixXd A = M + M.transpose();
 
    // // Define the B matrix, a tridiagonal matrix with 2 on the diagonal
    // // and 1 on the subdiagonals
    // Eigen::SparseMatrix<double> B(n, n);
    // B.reserve(Eigen::VectorXi::Constant(n, 3));
    // for (int i = 0; i < n; i++)
    // {
    //     B.insert(i, i) = 2.0;
    //     if (i > 0)
    //         B.insert(i - 1, i) = 1.0;
    //     if (i < n - 1)
    //         B.insert(i + 1, i) = 1.0;
    // }
 
    // // Construct matrix operation objects using the wrapper classes
    // // A is dense, B is sparse
    // using OpType = SymShiftInvert<double, Eigen::Dense, Eigen::Sparse>;
    // using BOpType = SparseSymMatProd<double>;
    // OpType op(A, B);
    // BOpType Bop(B);
 
    // // Construct generalized eigen solver object, seeking three generalized
    // // eigenvalues that are closest to zero. This is equivalent to specifying
    // // a shift sigma = 0.0 combined with the SortRule::LargestMagn selection rule
    // SymGEigsShiftSolver<OpType, BOpType, GEigsMode::ShiftInvert>
    //     geigs(op, Bop, 3, 6, 0.0);
 
    // // Initialize and compute
    // geigs.init();
    // int nconv = geigs.compute(SortRule::LargestMagn);
 
    // // Retrieve results
    // Eigen::VectorXd evalues;
    // Eigen::MatrixXd evecs;
    // if (geigs.info() == CompInfo::Successful)
    // {
    //     evalues = geigs.eigenvalues();
    //     evecs = geigs.eigenvectors();
    // }
 
    // std::cout << "Number of converged generalized eigenvalues: " << nconv << std::endl;
    // std::cout << "Generalized eigenvalues found:\n" << evalues << std::endl;
    // std::cout << "Generalized eigenvectors found:\n" << evecs.topRows(10) << std::endl;
 
    return 0;

}
