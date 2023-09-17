@echo off
rem 删除文件
set "scriptPath=%~dp0"

set "toBeDeletedFilename=solution_matlab.dat"
set "file_to_delete=%scriptPath%%toBeDeletedFilename%"

set "logFileName=vbslog.txt"
set "logFilePath=%scriptPath%%logFileName%"
@REM echo file_to_delete_path: "%file_to_delete%" >> "%logFilePath%"

if exist "%file_to_delete%" (
    del "%file_to_delete%"
    @REM echo file deleated. >> log.txt
) else (
    @REM echo file not exist. >> log.txt
)

rem 启动 MATLAB 并执行 a.m 脚本
"D:\Program Files\MATLAB\R2023a\bin\matlab.exe" -nodesktop -minimize -nosplash -batch "run('%scriptPath%\compute_solution_by_matlab.m');exit;"
