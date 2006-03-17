USES += core v3p/netlib
LIBRARY := vnl-algo
VERSION := 1.0
IMMEDIATE_LIBS := -lvnl -lnetlib
DEFINES += -DBUILDING_VNL_ALGO_DLL

# Most of vnl_algo is simply a wrapper around netlib.
# Direct netlib dependencies are:
# vnl_chi_squared                  dchscdf_
# vnl_cholesky                     dpoco_ dpodi_ dpofa_ dposl_
# vnl_complex_eigensystem          zgeev_
# vnl_conjugate_gradient           cg_
# vnl_fft                          dgpfa_ dsetgpfa_ gpfa_ setgpfa_
# vnl_generalized_eigensystem      rsg_
# vnl_generalized_schur            dgges_
# vnl_lbfgs                        lbfgs_ lb3_
# vnl_levenberg_marquardt          lmder1_ lmdif_
# vnl_lsqr                         lsqr_
# vnl_qr                           cqrdc_ cqrsl_ dqrdc_ dqrsl_ sqrdc_ sqrsl_ zqrdc_ zqrsl_
# vnl_real_eigensystem             rg_
# vnl_rpoly_roots                  rpoly_
# vnl_sparse_symmetric_eigensystem dnlaso_
# vnl_svd                          csvdc_ dsvdc_ ssvdc_ zsvdc_
# vnl_svd_economy                  csvdc_ dsvdc_ ssvdc_ zsvdc_
# vnl_symmetric_eigensystem        rs_

MANPAGE_SOURCES += vnl_algo_fwd.h
MANPAGE_SOURCES += vnl_netlib.h

# matrix decompositions
MANPAGE_SOURCES += vnl_svd.txx
MANPAGE_SOURCES += vnl_matrix_inverse.txx
MANPAGE_SOURCES += vnl_qr.txx
MANPAGE_SOURCES += vnl_scatter_3x3.txx
SOURCES += vnl_cholesky.cxx
SOURCES += vnl_real_eigensystem.cxx
SOURCES += vnl_complex_eigensystem.cxx
SOURCES += vnl_symmetric_eigensystem.cxx
SOURCES += vnl_generalized_eigensystem.cxx
SOURCES += vnl_sparse_symmetric_eigensystem.cxx
SOURCES += vnl_generalized_schur.cxx
#SOURCES += $(wildcard vnl_jordan_normal_form.cxx)
MANPAGE_SOURCES += vnl_svd_economy.txx

# optimization
SOURCES += vnl_discrete_diff.cxx
SOURCES += vnl_levenberg_marquardt.cxx
SOURCES += vnl_conjugate_gradient.cxx
SOURCES += vnl_lbfgs.cxx
SOURCES += vnl_amoeba.cxx
SOURCES += vnl_powell.cxx
SOURCES += vnl_brent.cxx
SOURCES += vnl_lsqr.cxx

# equation solvers
SOURCES += vnl_rpoly_roots.cxx
SOURCES += vnl_cpoly_roots.cxx
SOURCES += vnl_rnpoly_solve.cxx
SOURCES += vnl_solve_qp.cxx

# fft
SOURCES += vnl_fft.cxx
MANPAGE_SOURCES += vnl_fft_base.txx
MANPAGE_SOURCES += vnl_fft_1d.txx
MANPAGE_SOURCES += vnl_fft_2d.txx
MANPAGE_SOURCES += vnl_fft_prime_factors.txx

# stuff
MANPAGE_SOURCES += vnl_convolve.txx
MANPAGE_SOURCES += vnl_determinant.txx
SOURCES += vnl_determinant.cxx
SOURCES += vnl_chi_squared.cxx
SOURCES += vnl_gaussian_kernel_1d.cxx
MANPAGE_SOURCES += vnl_adjugate.txx
MANPAGE_SOURCES += vnl_orthogonal_complement.txx

# integral
SOURCES += vnl_simpson_integral.cxx
SOURCES += vnl_adaptsimpson_integral.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

# tests
ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################

ifeq "HP-UX" "$(findstring HP-UX,$(OS))"
FLAGS_Templates/vnl_determinant+vcl_complex+long_double--.cxx := -O0
FLAGS_Templates/vnl_svd+vcl_complex+long_double--.cxx := -O0
endif
