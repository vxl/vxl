include ${HOME}/fmk/fmk_params.mk

fmk_USES := vxl

fmk_LIBRARY := vnl-algo
fmk_VERSION := 1.0

# matrix decompositions
MANPAGE_SOURCES += vnl_svd.txx
MANPAGE_SOURCES += vnl_matrix_inverse.txx
MANPAGE_SOURCES += vnl_qr.txx
fmk_LIB_SOURCES += vnl_cholesky.cxx
fmk_LIB_SOURCES += vnl_real_eigensystem.cxx
fmk_LIB_SOURCES += vnl_complex_eigensystem.cxx
fmk_LIB_SOURCES += vnl_symmetric_eigensystem.cxx
fmk_LIB_SOURCES += vnl_generalized_eigensystem.cxx
fmk_LIB_SOURCES += vnl_sparse_symmetric_eigensystem.cxx
fmk_LIB_SOURCES += $(wildcard vnl_jordan_normal_form.cxx)

# optimization
fmk_LIB_SOURCES += vnl_discrete_diff.cxx
fmk_LIB_SOURCES += vnl_levenberg_marquardt.cxx
fmk_LIB_SOURCES += vnl_conjugate_gradient.cxx
fmk_LIB_SOURCES += vnl_lbfgs.cxx
fmk_LIB_SOURCES += vnl_amoeba.cxx

# equation solvers
fmk_LIB_SOURCES += vnl_rpoly_roots.cxx
fmk_LIB_SOURCES += vnl_cpoly_roots.cxx
fmk_LIB_SOURCES += vnl_rnpoly_solve.cxx

# fft
fmk_LIB_SOURCES += vnl_fftxd_prime_factors.cxx
MANPAGE_SOURCES += vnl_fft1d.cxx
MANPAGE_SOURCES += vnl_fft2d.cxx

# stuff
fmk_LIB_SOURCES += vnl_determinant.cxx
fmk_LIB_SOURCES += vnl_chi_squared.cxx
fmk_LIB_SOURCES += vnl_gaussian_kernel_1d.cxx

#FIXME DEFINES += -DBUILDING_VNL_ALGO_DLL

include ${HOME}/fmk/fmk_rules.mk
