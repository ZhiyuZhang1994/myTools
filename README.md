**本代码仓为个人所写一些工具类**
  
 1. 一行宏定义使一个类变为单例类  
 2. 编写类自动注册宏  
 3. 实现了观察者机制  
 4. 一行宏声明结构体所有比较函数  

目录结构如下：
```
|-- my_tool_repo  # 我的工具库
    |-- .vscode  # vscode配置脚本，包括sftp.json, task.json, launch.json等
    |-- docs  # 文档目录：介绍相关工具的原理，研发过程
    |   |-- vtk  # vtk工具的研发过程，原理
    |-- include  # 头文件目录
    |   |-- qt_component  # 自定义QT组件的头文件目录
    |   |-- tools  # 自定义C++通用工具的头文件目录
    |-- src  # 源代码目录
    |   |-- finite_element  # 有限元相关代码
    |   |-- tools  # 自定义C++通用工具的源代码目录
    |   |-- vtk  # vtk相关工具的测试文件
    |-- test  # 测试代码目录
    |   |-- infrastructure_functoin_test  # 自定义C++通用工具的测试文件
    |-- third_party  # 第三方库目录
    |   |-- easylogging  # 日志模块
    |-- CMakeLists.txt  # cmake构建脚本
```


https://blog.csdn.net/qq_45642410/article/details/114272114?utm_medium=distribute.pc_aggpage_search_result.none-task-blog-2~aggregatepage~first_rank_ecpm_v1~rank_v31_ecpm-2-114272114-null-null.pc_agg_new_rank&utm_term=%E9%83%A8%E5%88%86%E7%B4%A2%E5%BC%95%E6%96%87%E4%BB%B6%E4%B8%8B%E8%BD%BD%E5%A4%B1%E8%B4%A5&spm=1000.2123.3001.4430


https://blog.csdn.net/xujianjun229/article/details/118457517

https://www.shuzhiduo.com/A/1O5EEMX757/

https://shurufa.sogou.com/linux/guide

paraview编译设定QT安装位置
cmake -GNinja -DPARAVIEW_USE_PYTHON=ON -DPARAVIEW_USE_MPI=ON -DVTK_SMP_IMPLEMENTATION_TYPE=TBB -DCMAKE_BUILD_TYPE=Release ../paraview -DQt5_DIR="/opt/Qt5.12.12/5.12.12/gcc_64/lib/cmake/Qt5"
