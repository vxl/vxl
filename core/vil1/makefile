#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

USES += vxl

IMMEDIATE_LIBS := -lvcl

LIBRARY := vil
VERSION := 1.0

# basic things
SOURCES += vil_pixel.cxx
SOURCES += vil_image_impl.cxx
SOURCES += vil_image.cxx
SOURCES += vil_stream.cxx
SOURCES += vil_file_format.cxx
SOURCES += vil_byte_swap.cxx

# various vil_stream classes
SOURCES += vil_stream_fstream.cxx   # from a C++ fstream
SOURCES += vil_stream_core.cxx      # in-core vil_stream
SOURCES += vil_stream_FILE_ptr.cxx  # adapt a vil_stream to a FILE * using temp file
SOURCES += vil_stream_section.cxx   # present a section of a vil_stream

# classes derived from vil_image
SOURCES += vil_file_image.cxx
SOURCES += vil_memory_image.cxx

# image adaptors
SOURCES += vil_memory_image_impl.cxx
SOURCES += vil_crop_image_impl.cxx
SOURCES += vil_skip_image_impl.cxx
SOURCES += vil_flipud_impl.cxx
SOURCES += vil_block_cache_image_impl.cxx
SOURCES += vil_image_as.cxx

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
SOURCES += file_formats/vil_jpeg_source_mgr.cxx
SOURCES += file_formats/vil_jpeg_decompressor.cxx
SOURCES += file_formats/vil_jpeg_destination_mgr.cxx
SOURCES += file_formats/vil_jpeg_compressor.cxx
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
SOURCES += vil_skip.cxx
SOURCES += vil_flipud.cxx
MANPAGE_SOURCES += vil_convolve.txx
SOURCES += vil_smooth.cxx

# Utilities
SOURCES += vil_clamp.cxx
SOURCES += vil_rgb_byte.cxx
SOURCES += vil_memory_image_window.cxx

# turn on optimization for the convolution code
FLAGS_Templates/vil_convolve_1d.cxx := $(optimize)
FLAGS_Templates/vil_convolve_simple+byte.byte.int.int-.cxx := $(optimize)
FLAGS_Templates/vil_convolve_simple+byte.float.byte.float-.cxx := $(optimize)
FLAGS_Templates/vil_convolve_simple+byte.float.float.float-.cxx := $(optimize)
FLAGS_Templates/vil_convolve_simple+byte.float.int.float-.cxx := $(optimize)
FLAGS_Templates/vil_convolve_simple+byte.float.short.float-.cxx := $(optimize)
FLAGS_Templates/vil_convolve_simple+byte.short.short.int-.cxx := $(optimize)

# subdirectories
#SUBDIRS += algo
ifndef NOTEST
SUBDIRS += tests 
SUBDIRS += examples
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
