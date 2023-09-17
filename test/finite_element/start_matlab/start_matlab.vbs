Set objFSO = CreateObject("Scripting.FileSystemObject")
strScriptPath = objFSO.GetParentFolderName(WScript.ScriptFullName)
startMatlabBatFilePath = strScriptPath & "\" & "start_matlab.bat"

' ' 创建一个文件系统对象
' Set objFSO = CreateObject("Scripting.FileSystemObject")
' ' 打开或创建日志文件以供写入
' logFilePath = strScriptPath & "\" & "vbslog.txt"
' Set objFile = objFSO.OpenTextFile(logFilePath, 8, True)
' ' 要写入的日志消息
' logMessage = "startMatlabBatFilePath:" & startMatlabBatFilePath
' ' 将日志消息写入文件
' objFile.WriteLine logMessage
' ' 关闭日志文件
' objFile.Close

' ' 释放对象
' Set objFile = Nothing
' Set objFSO = Nothing

' 执行批处理文件并等待其完成
Set WshShell = CreateObject("WScript.Shell")
ReturnCode = WshShell.Run(startMatlabBatFilePath, 0, True)


