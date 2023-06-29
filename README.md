**本代码仓为个人所写一些工具类**
  
 1. 一行宏定义使一个类变为单例类  
 2. 编写类自动注册宏  
 3. 实现了观察者机制  
 4. 一行宏声明结构体所有比较函数  

目录结构如下：
```
|-- my_tool_repo  # 我的工具库
    |-- .vscode  # vscode配置脚本，包括sftp.json, task.json, launch.json等
    |
    |-- docs  # 文档目录：介绍相关工具的原理，研发过程
    |   |-- vtk  # vtk工具的研发过程，原理
    |
    |-- include  # 头文件目录
    |   |-- qt_component  # 自定义QT组件的头文件目录
    |   |-- tools  # 自定义C++通用工具的头文件目录
    |
    |-- src  # 源代码目录
    |   |-- finite_element  # 有限元相关代码
    |   |-- tools  # 自定义C++通用工具的源代码目录
    |   |-- vtk  # vtk相关工具的测试文件
    |
    |-- test  # 测试代码目录
    |   |-- infrastructure_functoin_test  # 自定义C++通用工具的测试文件
    |
    |-- third_party  # 第三方库目录
    |   |-- easylogging  # 日志模块
    |
    |-- CMakeLists.txt  # cmake构建脚本
```
