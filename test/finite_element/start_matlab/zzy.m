%创建一个包含两行的矩阵y
x = 0:10:100;
y = [x; log(x)];
 
%打开文件进行写入
fid = fopen('D:\\work_station\\github_project\\func_test\\test\\finite_element\\start_matlab\\logtable.txt', 'w');
 
%表格标题
fprintf(fid, 'Log     Function\n\n');
 
%按列顺序打印值
%文件的每一行上显示两个值
fprintf(fid, '%f    %f\n', y);
fclose(fid);