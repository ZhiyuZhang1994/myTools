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
