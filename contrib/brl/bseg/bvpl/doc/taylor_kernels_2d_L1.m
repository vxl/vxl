% Code to generate different size of taylor kernels up to second degree
% aproximations
clear; clc;
format short
% format rat

% Size of kernels in different dimensions  - assume size goes from -n to n
ni=2;
nj=2;

syms  I i j

%The following vectorss/matrices presentes coefficients of
%I0, Ix, Iy, Iz, Ixx, Iyy, Izz, Ixy, Ixz, Iyz (in order)

%The first row corresponsed to dE/dI0
dE_dI0 = [1, i, j, 0.5.*i.^2, 0.5.*j.^2, i.*j];

%dE/dIx
dE_dIx = [dE_dI0.*i];
dE_dIy = [dE_dI0.*j];

%dE/dIxx
dE_dIxx = [dE_dI0.*(i.*i)];
dE_dIyy = [dE_dI0.*(j.*j)];

%dE/dIxy
dE_dIxy = [2.*dE_dI0.*(i.*j)];

% Matrix of coefficientes
C_sym = [dE_dI0; dE_dIx; dE_dIy; dE_dIxx; dE_dIyy; dE_dIxy;]

C = symsum(symsum(C_sym, j, -nj, nj),i, -ni, ni);

keep_r= any(double(C))
keep_c = any(double(C.'))
C = C(keep_r, keep_c)

C_inv = inv(C)

B = [1, i, j, (i.*i), (j.*j), 2*(i.*j)].'

X = C_inv*B(keep_c)


% generate kernel and save them to file
dir = '/Projects/vxl/src/contrib/brl/bseg/bvpl/doc/taylor2_5_5_L1_2d'
%dir = '/Projects/vxl/src/contrib/brl/bseg/bvpl/doc/taylor2_5_5_2d'
%dir = '/Users/isa/Experiments/Taylor/basis_vectors'

file{1} = [dir  '/I0.txt'];
file{2} = [dir  '/Ix.txt'];
file{3} = [dir  '/Iy.txt'];
file{4} = [dir  '/Ixx.txt'];
file{5} = [dir  '/Iyy.txt'];
file{6} = [dir  '/Ixy.txt'];




all_kernels = cell(size(X));
kernel = zeros(length(-ni:ni),length(-nj:nj));
for x = 1:length(X);
    kernel = zeros(length(-ni:ni),length(-nj:nj));
    for j = -nj:nj;
        for i = -ni:ni;
            kernel(i+ni+1,j+nj+1) = eval(X(x));

        end
    end
    all_kernels{x} = (kernel);
end


for x = 1:length(X);
    all_kernels{x} = all_kernels{x}./sum(sum(sum(abs(all_kernels{x}))));
    sum(sum(sum(all_kernels{x})))
    sum(sum(sum(abs(all_kernels{x}))))

%     figure; imshow(all_kernels{x})
end


for x = 1:length(X);
    fid = fopen(file{x}, 'w');
    fprintf(fid, '%d,%d\n', -ni, -nj);
    fprintf(fid, '%d,%d\n', ni, nj);
    kernel = all_kernels{x};
    for j = -nj:nj;
        for i = -ni:ni;
            fprintf(fid, '%f,%f\n%f\n', i, j, kernel(i+ni+1,j+nj+1));

        end
    end
    fclose(fid);
end
