#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

#ifneq (1,$(strip $(HAS_NATIVE_NETLIB)))
SUBDIRS += netlib
#endif

SUBDIRS += zlib png

ifneq (,$(strip $(wildcard $(JPEG_INC_DIR)/v3p_tag)))
SUBDIRS += jpeg
endif

ifneq (,$(strip $(wildcard $(TIFF_INC_DIR)/v3p_tag)))
SUBDIRS += tiff
endif

SUBDIRS += Qv

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
