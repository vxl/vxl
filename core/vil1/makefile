VXL := 1 
include vil_config.mk

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

USES += vxl

IULIBS := -lvcl

LIBRARY := vil
VERSION := 1.0

# base classes, utilities
SOURCES += vil_generic_image.cxx
SOURCES += vil_image_ref.cxx
SOURCES += vil_file_format.cxx
SOURCES += vil_stream.cxx
SOURCES += vil_stream_fstream.cxx
SOURCES += vil_stream_FILE_ptr.cxx
SOURCES += vil_rgb_byte.cxx

# generic_image derived classes
SOURCES += vil_memory_image.cxx
SOURCES += vil_crop_image.cxx
SOURCES += vil_skip_image.cxx

# file_format subclasses
SOURCES += file_formats/vil_pnm.cxx
SOURCES += file_formats/vil_viff.cxx
SOURCES += file_formats/vil_viff_support.c
SOURCES += file_formats/vil_iris.cxx
SOURCES += file_formats/vil_mit.cxx

ifeq ($(HAS_PNG),1)
SOURCES += file_formats/vil_png.cxx
endif
ifeq ($(HAS_JPEG),1)
INCDIRS += $(IUEROOT)/Image/JPEG
SOURCES += file_formats/vil_jpeg.cxx
endif

SOURCES += file_formats/vil_bmp.cxx


# Operations
SOURCES += vil_load.cxx
SOURCES += vil_save.cxx
SOURCES += vil_copy.cxx
SOURCES += vil_crop.cxx

# Subdirectories with makefiles
ifneq (1, $(NOTEST))
SUBDIRS += tests 
SUBDIRS += examples
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
