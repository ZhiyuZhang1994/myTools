
Set WshShell = CreateObject("WScript.Shell")
' 执行批处理文件并等待其完成
ReturnCode = WshShell.Run("D:\\work_station\\github_project\\func_test\\test\\finite_element\\start_matlab\\start_matlab.bat", 0, True)
