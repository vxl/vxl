#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

SUBDIRS += netlib

SUBDIRS += Qv

SUBDIRS += rply

# do not compile zlib, png, jpeg, tiff etc. by default as they
# are "native" libraries on some systems. clients who wish
# to compile the 3rd libraries which come with the v3p source
# distribution must specify include paths to the v3p versions.
# it will be detected here.

ifneq (,$(strip $(wildcard $(ZLIB_INC_DIR)/../v3p_tag)))
SUBDIRS += zlib
endif

ifneq (,$(strip $(wildcard $(BZLIB_INC_DIR)/../v3p_tag)))
SUBDIRS += bzlib
endif

ifneq (,$(strip $(wildcard $(PNG_INC_DIR)/../v3p_tag)))
SUBDIRS += png
endif

ifneq (,$(strip $(wildcard $(JPEG_INC_DIR)/../v3p_tag)))
SUBDIRS += jpeg
endif

ifneq (,$(strip $(wildcard $(TIFF_INC_DIR)/../v3p_tag)))
SUBDIRS += tiff
endif

ifneq (,$(strip $(wildcard $(GEOTIFF_INC_DIR)/../v3p_tag)))
SUBDIRS += geotiff
endif

ifneq (,$(strip $(wildcard $(MPEG2_INC_DIR)/../../v3p_tag)))
SUBDIRS += mpeg2
endif

ifneq (,$(strip $(wildcard $(DCMTK_INC_DIR)/../../../v3p_tag)))
SUBDIRS += dcmtk
endif

ifneq (,$(strip $(wildcard $(J2K_INC_DIR)/../../../v3p_tag)))
SUBDIRS += j2k
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
