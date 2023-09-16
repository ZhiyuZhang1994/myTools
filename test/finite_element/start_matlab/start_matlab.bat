@echo off
rem 删除文件
set file_to_delete=D:\work_station\github_project\func_test\test\finite_element\start_matlab\\logtable.txt

if exist "%file_to_delete%" (
    del "%file_to_delete%"
    echo 文件已删除。
) else (
    echo 文件不存在。
)

rem 启动 MATLAB 并执行 a.m 脚本
"D:\Program Files\MATLAB\R2023a\bin\matlab.exe" -nodesktop -minimize -nosplash -batch "run('D:\\work_station\\github_project\\func_test\\test\\finite_element\\start_matlab\\zzy.m');exit;"

rem matlab -nojvm -nosplash -r "run('D:\temp\matlabCode\GraduateProgramCode\zzy.m');exit;"

