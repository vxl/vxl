USES += contrib/rpl contrib/mul core

LIBRARY := rrel
VERSION := 1.0

SOURCES += rrel_cauchy_obj.cxx
SOURCES += rrel_estimation_problem.cxx
SOURCES += rrel_homography2d_est.cxx
SOURCES += rrel_irls.cxx
SOURCES += rrel_linear_regression.cxx
SOURCES += rrel_lms_obj.cxx
SOURCES += rrel_lts_obj.cxx
SOURCES += rrel_m_est_obj.cxx
SOURCES += rrel_misc.cxx
SOURCES += rrel_mlesac_obj.cxx
SOURCES += rrel_muse_table.cxx
SOURCES += rrel_muset_obj.cxx
SOURCES += rrel_orthogonal_regression.cxx
SOURCES += rrel_ran_sam_search.cxx
SOURCES += rrel_ransac_obj.cxx
SOURCES += rrel_trunc_quad_obj.cxx
SOURCES += rrel_tukey_obj.cxx
SOURCES += rrel_kernel_density_obj.cxx
MANPAGE_SOURCES += rrel_objective.h
MANPAGE_SOURCES += rrel_util.txx
MANPAGE_SOURCES += rrel_wls_obj.h

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
SUBDIRS += examples
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
