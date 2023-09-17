Set WshShell = CreateObject("WScript.Shell")
' 执行批处理文件并等待其完成
ReturnCode = WshShell.Run("D:\\work_station\\github_project\\func_test\\test\\finite_element\\start_matlab\\start_matlab.bat", 0, True)

' ' 创建一个文件系统对象
' Set objFSO = CreateObject("Scripting.FileSystemObject")

' ' 指定日志文件的路径
' logFilePath = "D:\\work_station\\github_project\\func_test\\test\\finite_element\\start_matlab\\vbslog.txt"

' ' 打开或创建日志文件以供写入
' Set objFile = objFSO.OpenTextFile(logFilePath, 8, True)

' ' 获取当前日期和时间
' currentTime = Now()

' ' 格式化日期和时间
' formattedTime = FormatDateTime(currentTime, 2)

' ' 要写入的日志消息
' logMessage = "zzy test begin . Current time: " & formattedTime

' ' 将日志消息写入文件
' objFile.WriteLine logMessage

' ' 等待5秒
' WScript.Sleep 10000 ' 5000毫秒等于5秒

' ' 要写入的日志消息
' logMessage = "zzy test end."

' ' 将日志消息写入文件
' objFile.WriteLine logMessage

' ' 关闭日志文件
' objFile.Close

' ' 释放对象
' Set objFile = Nothing
' Set objFSO = Nothing