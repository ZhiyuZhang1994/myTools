**本代码仓包含一下内容**
  
 1. 个人编写的C++工具类，如类自动注册宏、观察者机制、单例宏、结构体所有比较函数宏
 2. ==VTK技术实现————当前主要研究内容，目录：test\vtk==
 3. 有限元算法研究

目录结构如下：
```
|-- my_tool_repo  # 我的工具库
    |-- .vscode  # vscode配置脚本，包括sftp.json, task.json, launch.json等
    |
    |-- docs  # 文档目录：介绍相关工具的原理，研发过程
    |   |-- linux  # linux使用笔记
    |   |-- vtk  # vtk工具的研发过程，原理
    |
    |-- include  # 头文件目录
    |   |-- qt_component  # 自定义QT组件的头文件目录
    |   |-- tools  # 自定义C++通用工具的头文件目录
    |
    |-- src  # 源代码目录
    |   |-- tools  # 自定义C++通用工具的源代码目录，包括定时器、打点计时器、线程池、表驱动等工具
    |   |-- temp_test.cpp  # 测试文件
    |
    |-- test  # 测试代码目录
    |   |-- finite_element  # 有限元相关测试代码
    |   |-- infrastructure_functoin_test  # 自定义C++通用工具的测试文件
    |   |-- vtk  # vtk相关工具的测试文件————当前的主要工作
    |
    |-- third_party  # 第三方库目录
    |   |-- easylogging  # 日志模块
    |
    |-- CMakeLists.txt  # cmake构建脚本
```


ChangeLog for GCGE4BDFY
$$
Ax = \lambda Bx
$$
其中矩阵 $A, B \in \mathbb{R}^{N\times N}$ 都是对称的。

[Change 20241022] : 增加lapack接口

​	目前认为当矩阵规模小于 $maxdim\_dense = 1000$ 时，直接调用lapack的接口函数将矩阵看成稠密矩阵，这个阈值还需要根据计算环境来确定。仅修改app_lapack.h(增加一些必要的lapack函数)和test_eig_sol_gcg.c(test_gcge_01.c修改一致)，增加如下功能函数：

```c
static void SortEig_dbl(double *eval, double *evec, int start, int end, int ld); //用于排序
/*
			flag == 0 : B^{-1}Ax = \lambda x
			flag == 1 : A^{-1}Bx = 1/\lambda x
			flag == 2 : generalized ep Ax = \lambda Bx, A and B are non-sym
			flag == 3 : generalized ep Ax = \lambda Bx, A: sym, B: SPD
*/
void DenseEigenSolver(CCSMAT *A, CCSMAT *B, int *nevConv, double *eval, double *evec, int flag);
```



### 问题转换和求解

当矩阵 $B$ 不是单位阵时，一共考虑如下三种方法来处理广义稠密矩阵特征值问题:

(1)先对 $B$ 求逆，然后求解如下标准特征值问题
$$
B^{-1}Ax = \lambda x
$$
(2)先对 $A$ 求逆，然后求解如下标准特征值问题
$$
\frac{1}{\lambda}x = A^{-1}B x
$$
(3)直接求解原来的广义特征值问题

当矩阵 $B$ 是单位阵时，求解的是一个对称稠密矩阵特征值问题。

### nevConv和$N$的关系

如果 $nevConv > N$，$nevConv \leftarrow N$；

如果 $N \leq maxdim\_dense$ 并且  $nevConv \leq N$， 在调用lapack的函数时，由于矩阵 $A$ 具有对称性，可以选择只求解前 $nevConv$ 个特征对，经测试该种情况下计算效率最高（广义特征值设置flag=3，标准特征值默认该情况下只算前$nevConv$)
