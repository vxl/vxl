################################
include ${IUEROOT}/config/top-params.mk
################################

USES := vxl
VXL := 1

LIBRARY := vnl-algo
VERSION := 1.0

# matrix decompositions
MANPAGE_SOURCES += vnl_svd.txx
MANPAGE_SOURCES += vnl_matrix_inverse.txx
SOURCES += vnl_qr.cxx
SOURCES += vnl_cholesky.cxx
SOURCES += linpack_svd.cxx
SOURCES += vnl_real_eigensystem.cxx
SOURCES += vnl_complex_eigensystem.cxx
SOURCES += vnl_symmetric_eigensystem.cxx
SOURCES += vnl_generalized_eigensystem.cxx
SOURCES += vnl_sparse_symmetric_eigensystem.cxx

# optimization
SOURCES += vnl_discrete_diff.cxx
SOURCES += vnl_levenberg_marquardt.cxx
SOURCES += vnl_conjugate_gradient.cxx
SOURCES += vnl_lbfgs.cxx
SOURCES += vnl_amoeba.cxx

# equation solvers
SOURCES += vnl_rpoly_roots.cxx
SOURCES += vnl_cpoly_roots.cxx
SOURCES += vnl_rnpoly_solve.cxx

# fft
SOURCES += vnl_fftxd_prime_factors.cxx
#MANPAGE_SOURCES += vnl_fft1d.txx
#MANPAGE_SOURCES += vnl_fft2d.txx

# stuff
SOURCES += vnl_chi_squared.cxx
SOURCES += vnl_gaussian_kernel_1d.cxx
SOURCES += vnl_affine_approx.cxx       # hmmm.... (fsm)

IULIBS := -lvnl -lvcl

DEFINES += -DBUILDING_VNL_ALGO_DLL

################################
include ${IUEROOT}/config/top-rules.mk
################################
