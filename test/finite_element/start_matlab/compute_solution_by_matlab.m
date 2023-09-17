clear
clc

currentScript = mfilename('fullpath');
[currentPath, ~, ~] = fileparts(currentScript);
b1FullPath = fullfile(currentPath, 'bc.dat');
A1FullPath = fullfile(currentPath, 'stiffness_matrix.dat');

b1=load(b1FullPath);
A1=load(A1FullPath);

A = sparse(A1(:,1),A1(:,2),A1(:,3),A1(size(A1,1),1),A1(size(A1,1),1));
tic
x=A\b1;
toc

fidFullPath = fullfile(currentPath, 'solution_matlab.dat');
fid=fopen(fidFullPath,'w');
for jj=1:length(x)
fprintf(fid,'%.8f\r\n',x(jj));   
end
fclose(fid);

