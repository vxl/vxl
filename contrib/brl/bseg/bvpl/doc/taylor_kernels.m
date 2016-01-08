% Code to generate different size of taylor kernels up to second degree
% aproximations
clear; clc;
format short
% format rat

% Size of kernels in different dimensions  - assume size goes from -n to n
ni=2;
nj=2;
nk=2;

syms  I i j k

%The following vectorss/matrices presentes coefficients of
%I0, Ix, Iy, Iz, Ixx, Iyy, Izz, Ixy, Ixz, Iyz (in order)

%The first row corresponsed to dE/dI0
dE_dI0 = [1, i, j, k, 0.5.*i.^2, 0.5.*j.^2, 0.5*k.^2, i.*j, i.*k , j.*k];

%dE/dIx
dE_dIx = [dE_dI0.*i];
dE_dIy = [dE_dI0.*j];
dE_dIz = [dE_dI0.*k];

%dE/dIxx
dE_dIxx = [dE_dI0.*(i.*i)];
dE_dIyy = [dE_dI0.*(j.*j)];
dE_dIzz = [dE_dI0.*(k.*k)];

%dE/dIxy
dE_dIxy = [2.*dE_dI0.*(i.*j)];
dE_dIxz = [2.*dE_dI0.*(i.*k)];
dE_dIyz = [2.*dE_dI0.*(j.*k)];

% Matrix of coefficientes
C_sym = [dE_dI0; dE_dIx; dE_dIy; dE_dIz; dE_dIxx; dE_dIyy; dE_dIzz; dE_dIxy; dE_dIxz; dE_dIyz]

C = symsum(symsum(symsum(C_sym,k, -nk, nk), j, -nj, nj),i, -ni, ni);

keep_r= any(double(C))
keep_c = any(double(C.'))
C = C(keep_r, keep_c)

C_inv = inv(C)

B = [1, i, j, k, (i.*i), (j.*j), (k.*k), 2*(i.*j), 2*(i.*k), 2*(j.*k)].'

X = C_inv*B(keep_c)


% generate kernel and save them to file
% file{1} =
%
% fid = fopen('test.txt', 'w');



all_kernels = cell(size(X));
kernel = zeros(length(-ni:ni),length(-nj:nj),length(-nk:nk));
for x = 1:length(X);
    k = zeros(length(-ni:ni),length(-nj:nj),length(-nk:nk));
    for k = -nk:nk;
        for j = -nj:nj;
            for i = -ni:ni;
                kernel(i+ni+1,j+nj+1,k+nk+1) = eval(X(x));
%                 fprintf(fid, '%d,%d,%d\n%f\n', i, j, k, eval(X(x)));
            end
        end
    end
    all_kernels{x} = (kernel);
end

