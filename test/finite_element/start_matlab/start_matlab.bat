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

@REM REM 使用where命令查找Matlab可执行文件
@REM for /f "tokens=* delims=" %%i in ('where matlab.exe') do (
@REM     set "matlab_path=%%i"
@REM )

rem 启动 MATLAB 并执行 a.m 脚本
@REM "%matlab_path%" -nodesktop -minimize -nosplash -batch "run('%scriptPath%\compute_solution_by_matlab.m');exit;"
"%matlab_path%" -nodesktop -minimize -nosplash -batch "run('%scriptPath%\compute_solution_by_matlab.m');exit;"
