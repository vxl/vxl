VXL := 1

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

USES := vxl v3p/netlib

SAUCES += test_math.cxx
SAUCES += test_vector.cxx
SAUCES += test_matrix.cxx
SAUCES += test_matrix_fixed.cxx
SAUCES += test_rnpoly_roots.cxx
SAUCES += test_sparse_matrix.cxx
SAUCES += test_qsvd.cxx
SAUCES += test_functions.cxx
SAUCES += test_numeric_limits.cxx
SAUCES += test_rpoly_roots.cxx
SAUCES += test_cpoly_roots.cxx
#SAUCES += test_matlab.cxx
SAUCES += test_resize.cxx
SAUCES += test_fft1d.cxx
SAUCES += test_fft2d.cxx
SAUCES += test_complex.cxx
SAUCES += test_qr.cxx
SAUCES += test_svd.cxx
SAUCES += test_real_eigensystem.cxx
SAUCES += test_complex_eigensystem.cxx
SAUCES += test_symmetric_eigensystem.cxx
SAUCES += test_generalized_eigensystem.cxx

IULIBS += -lvnl-algo -lnetlib -lvnl -lvcl

################################
ifeq (black, white)
  MINI_PROG_SOURCES += $(SAUCES)
  include ${IUEROOT}/config/top-rules.mk
else
  TESTS += $(SAUCES)
  include ${IUEROOT}/config/static-tests.mk
endif
################################
