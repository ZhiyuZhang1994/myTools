/**
 * @brief 类模板用法测试
 * @author zhiyu.zhang@cqbdri.pku.edu.cn
 * @date 2023-06-05
 */

#include <chrono>
#include <ctime>
#include <iostream>
#include <ratio>
#include <vector>
#include <cmath>
#include <thread>
#include <iomanip>
#include <functional>   // std::plus
#include <algorithm>    // std::transform
#include <vector>
#include <cmath>
#include <iostream>
#include <fstream>

using namespace std;

/**
 * @brief 按数组形式计算方法
 */

std::vector<std::vector<double>> calInvariantsOfStressTensors(const std::vector<double>& s11, const std::vector<double>& s22, const std::vector<double>& s33,
    const std::vector<double>& s12, const std::vector<double>& s13, const std::vector<double>& s23) {
    std::vector<double> I1(s11.size());
    std::vector<double> I2(s11.size());
    std::vector<double> I3(s11.size());
    for (size_t i = 0; i < s11.size(); ++i) {
        I1[i] = s11[i] + s22[i] + s33[i];
        I2[i] = s11[i] * s22[i] + s22[i] * s33[i] + s11[i] * s33[i] - std::pow(s12[i],2) - std::pow(s13[i],2) - std::pow(s23[i],2);
        I3[i] = s11[i] * (s22[i] * s33[i] - std::pow(s23[i],2)) - s12[i] * (s12[i] * s33[i] - s13[i] * s23[i]) + s13[i] * (s12[i] * s23[i] - s13[i] * s22[i]);
    }
    return {I1, I2, I3};
}

std::vector<std::vector<double>> calMainStressUsingInvariants(std::vector<std::vector<double>>& invariants) {
    auto& I1 = invariants[0];
    auto& I2 = invariants[1]; // theta占用其位置，节省内存
    auto& I3 = invariants[2]; // p, main3占用其位置，节省内存
    std::vector<double> q(I1.size()); // main1占用其位置，节省内存
    std::vector<double> main2(I1.size());
    for (size_t i = 0; i < I1.size(); ++i) {
        q[i] = (9 * I1[i] * I2[i] - 2 * std::pow(I1[i], 3) - 27 * I3[i]) / 27; // 计算q
        I3[i] = (3 * I2[i] - std::pow(I1[i], 2)) / 3; // 计算p
        I2[i] = std::acos(-q[i] / 2 * std::pow(-(std::pow(I3[i], 3) / 27), -0.5)); // 计算theta
        q[i] = I1[i] / 3 + 2 * std::pow(-I3[i] / 3, 0.5) * std::cos(I2[i] / 3); // 计算main1
        main2[i] = I1[i] / 3 - std::pow(-I3[i] / 3, 0.5) * (std::cos(I2[i] / 3) - std::pow(3, 0.5) * std::sin(I2[i] / 3)); // 计算main2
        I3[i] = I1[i] / 3 - std::pow(-I3[i] / 3, 0.5) * (std::cos(I2[i] / 3) + std::pow(3, 0.5) * std::sin(I2[i] / 3)); // 计算main3
    }
    return {q, main2, I3};
}

std::vector<std::vector<double>> calMainStress(const std::vector<double>& s11, const std::vector<double>& s22, const std::vector<double>& s33,
    const std::vector<double>& s12, const std::vector<double>& s13, const std::vector<double>& s23) {
    std::vector<std::vector<double>> invariants = calInvariantsOfStressTensors(s11, s22, s33, s12, s13, s23);
    return calMainStressUsingInvariants(invariants);
}
/**
 * @brief 计算单一节点的原始方法：danyh
 */
////////////////////////////////////通过分量计算不变量////////////////////////////////
//传入的components={s11,s22,s33,s12,s13,s23}
std::vector<double> Invariant(std::vector<double> components) {
    std::vector<double> result;
    double I1 = components[0] + components[1] + components[2];
    result.push_back(I1);
    double I2 = components[0] * components[1] + components[1] * components[2] + components[0] * components[2];
    for (int i = 3; i < 6; i++) {
        I2 -= std::pow(components[i], 2);
    }
    result.push_back(I2);
    double I3 = 0;
    I3 += components[0] * (components[1] * components[2] - components[5] * components[5]);
    I3 -= components[3] * (components[3] * components[2] - components[4] * components[5]);
    I3 += components[4] * (components[3] * components[5] - components[4] * components[1]);
    result.push_back(I3);
    return result;
}

///////////////////////////////卡丹公式//////////////////////////////////////////////
std::vector<double> Cardano(double I1, double I2, double I3) {
    double p = (3 * I2 - pow(I1, 2)) / 3;
    double q = (9 * I1 * I2 - 2 * pow(I1, 3) - 27 * I3) / 27;
    double theta = acos((-q) / 2 * pow(-(pow(p, 3) / 27), -0.5));
    std::vector<double> ans;
    ans.push_back(I1 / 3 + 2 * pow(-p / 3, 0.5) * cos(theta / 3));
    ans.push_back(I1 / 3 - pow(-p / 3, 0.5) * (cos(theta / 3) - pow(3, 0.5) * sin(theta / 3)));
    ans.push_back(I1 / 3 - pow(-p / 3, 0.5) * (cos(theta / 3) + pow(3, 0.5) * sin(theta / 3)));
    return ans;
}

int main() {
    // std::vector<double> components{0, -95.6945, 0, 0, 0, 0};
    std::vector<double> components{282.251, 104.607, 0, -69.7038, 0, 0};
    std::vector<double> invariants = Invariant(components);
    std::vector<double> Cd = Cardano(invariants[0], invariants[1], invariants[2]);
    std::cout << Cd[0] << " " << Cd[1] << " " << Cd[2] << " " << std::endl;
    for (std::uint32_t i = 0; i < Cd.size(); ++i) {
        std::cout << std::setw(4) << i + 1 << " " << std::setw(15) << Cd[i] << std::endl;;
    }
    std::cout << std::endl << std::endl;

    std::vector<double> s11{0, 282.251};
    std::vector<double> s22{-95.6945, 104.607};
    std::vector<double> s33{0, 0};
    std::vector<double> s12{0, -69.7038};
    std::vector<double> s13{0, 0};
    std::vector<double> s23{0, 0};
    std::vector<std::vector<double>> ans = calMainStress(s11, s22, s33, s12, s13, s23);

    std::fstream fout;
    fout.open("abc.txt", std::ios_base::out);
    for (std::uint32_t i = 0; i < ans[0].size(); ++i) {
        fout << std::setw(4) << i + 1 << " " << std::setw(15) << ans[0][i] << std::endl;
        fout << std::setw(4) << i + 1 << " " << std::setw(15) << ans[1][i] << std::endl;
        fout << std::setw(4) << i + 1 << " " << std::setw(15) << ans[2][i] << std::endl;
    }
    std::cout << std::endl << std::endl;
}