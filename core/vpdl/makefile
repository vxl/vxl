#   DESCRIPTION:
#   This is the Vision Probability Distribution Library (vpdl)
#
#   DEPENDENCIES: vnl
#
USES += core

LIBRARY := vpdl
VERSION := 1.0
DEFINES += -DBUILDING_VPDL_DLL

SOURCES += vpdl_npower.cxx
MANPAGE_SOURCES += vpdl_base_traits.h
MANPAGE_SOURCES += vpdl_distribution.txx
MANPAGE_SOURCES += vpdl_gaussian_indep.h
MANPAGE_SOURCES += vpdl_gaussian_base.h
MANPAGE_SOURCES += vpdl_gaussian.h
MANPAGE_SOURCES += vpdl_gaussian_sphere.h
MANPAGE_SOURCES += vpdl_eigen_sym_matrix.h
MANPAGE_SOURCES += vpdl_mixture.h
MANPAGE_SOURCES += vpdl_update_gaussian.h
MANPAGE_SOURCES += vpdl_multi_cmp_dist.h
MANPAGE_SOURCES += vpdl_kernel_base.h
MANPAGE_SOURCES += vpdl_kernel_gaussian_sfbw.h
MANPAGE_SOURCES += vpdl_mixture_of.h

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
