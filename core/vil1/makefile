#######################################
include ${IUEROOT}/config/usr_params.mk
#######################################

USES += vxl

IULIBS := -lvcl

LIBRARY := vil
VERSION := 1.0

# base classes
SOURCES += vil_image_impl.cxx
SOURCES += vil_image.cxx
SOURCES += vil_stream.cxx
SOURCES += vil_file_format.cxx

# class derived from vil_streams
SOURCES += vil_stream_fstream.cxx   # from a C++ fstream
SOURCES += vil_stream_core.cxx      # in-core vil_stream
SOURCES += vil_stream_FILE_ptr.cxx  # adapt a vil_stream to a FILE * using temp file
SOURCES += vil_stream_section.cxx   # present a section of a vil_stream

# classes derived from vil_image
SOURCES += vil_file_image.cxx
SOURCES += vil_memory_image.cxx

# classes derived from vil_image_impl
SOURCES += vil_memory_image_impl.cxx
SOURCES += vil_crop_image.cxx
SOURCES += vil_skip_image.cxx
SOURCES += vil_block_cache_image.cxx

# classes derived from vil_file_format 
SOURCES += file_formats/vil_pnm.cxx
SOURCES += file_formats/vil_viff.cxx
SOURCES += file_formats/vil_viff_support.c
SOURCES += file_formats/vil_iris.cxx
SOURCES += file_formats/vil_mit.cxx
SOURCES += file_formats/vil_bmp.cxx
ifeq ($(HAS_PNG),1)
USE_PNG := 1
SOURCES += file_formats/vil_png.cxx
endif
ifeq ($(HAS_JPEG),1)
USE_JPEG := 1
SOURCES += file_formats/vil_jpeg.cxx
endif
ifeq ($(HAS_TIFF),1)
USE_TIFF := 1
SOURCES += file_formats/vil_tiff.cxx
endif

# Operations
SOURCES += vil_new.cxx
SOURCES += vil_load.cxx
SOURCES += vil_save.cxx
SOURCES += vil_copy.cxx
SOURCES += vil_crop.cxx
SOURCES += vil_convolve.cxx

# Utilities
SOURCES += vil_rgb_byte.cxx
SOURCES += vil_memory_image_window.cxx

# turn on optimization for the convolution code
FLAGS_Templates/vil_convolve_simple+byte.byte.int.int-.cxx := $(optimize)
FLAGS_Templates/vil_convolve_simple+byte.float.float.byte-.cxx := $(optimize)
FLAGS_Templates/vil_convolve_simple+byte.float.float.int-.cxx := $(optimize)
FLAGS_Templates/vil_convolve_simple+byte.float.float.short-.cxx := $(optimize)

# subdirectories
ifneq (1, $(NOTEST))
SUBDIRS += tests 
endif
SUBDIRS += examples

######################################
include ${IUEROOT}/config/usr_rules.mk
######################################
