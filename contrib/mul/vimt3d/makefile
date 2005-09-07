USES += contrib/mul core

LIBRARY := vimt3d
VERSION := 1.0

MANPAGE_SOURCES += vimt3d_fwd.h

SOURCES += vimt3d_load.cxx
SOURCES += vimt3d_image_3d.cxx
SOURCES += vimt3d_transform_3d.cxx
MANPAGE_SOURCES += vimt3d_image_3d_of.txx
MANPAGE_SOURCES += vimt3d_gaussian_pyramid_builder_3d.txx
MANPAGE_SOURCES += vimt3d_sample_grid_trilin.txx
MANPAGE_SOURCES += vimt3d_reconstruct_from_grid.txx
MANPAGE_SOURCES += vimt3d_sample_profile_trilin.h
MANPAGE_SOURCES += vimt3d_trilin_interp.h
MANPAGE_SOURCES += vimt3d_slice.h
MANPAGE_SOURCES += vimt3d_crop.h
SOURCES += vimt3d_add_all_loaders.cxx
SOURCES += vimt3d_vil3d_v3i.cxx
MANPAGE_SOURCES += vimt3d_from_image_2d.h
MANPAGE_SOURCES += vimt3d_gauss_reduce.h
MANPAGE_SOURCES += vimt3d_resample_trilinear.h

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
