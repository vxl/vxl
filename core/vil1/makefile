USES += core

LIBRARY := vil1
VERSION := 1.0

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

MANPAGE_SOURCES += vil1_fwd.h

# basic things
SOURCES += vil1_pixel.cxx
SOURCES += vil1_image_impl.cxx
SOURCES += vil1_image.cxx
SOURCES += vil1_stream.cxx
SOURCES += vil1_file_format.cxx
SOURCES += vil1_byte_swap.cxx
SOURCES += vil1_16bit.cxx
SOURCES += vil1_32bit.cxx

# various vil1_stream classes
SOURCES += vil1_stream_fstream.cxx     # from a file
SOURCES += vil1_stream_core.cxx        # in-core vil1_stream
SOURCES += vil1_stream_section.cxx     # present a section of a vil1_stream
SOURCES += vil1_stream_url.cxx         # load image from URL

# classes derived from vil1_image
SOURCES += vil1_file_image.cxx
SOURCES += vil1_memory_image.cxx
MANPAGE_SOURCES += vil1_memory_image_of.txx
MANPAGE_SOURCES += vil1_memory_image_of_format.txx
SOURCES += vil1_image_proxy.cxx

# image adaptors
SOURCES += vil1_memory_image_impl.cxx
SOURCES += vil1_crop_image_impl.cxx
SOURCES += vil1_skip_image_impl.cxx
SOURCES += vil1_resample_image_impl.cxx
SOURCES += vil1_flipud_impl.cxx
SOURCES += vil1_flip_components_impl.cxx
SOURCES += vil1_block_cache_image_impl.cxx
SOURCES += vil1_clamp_image_impl.cxx
SOURCES += vil1_clamp_image.cxx # Helper specializations for vil1_clamp_image_impl.cxx
SOURCES += vil1_scale_intensities_image_impl.cxx
SOURCES += vil1_scale_intensities_image.cxx # Helper specializations for vil1_scale_intensities_image_impl.cxx

# Wrapper classes for vil/vil1 conversion
MANPAGE_SOURCES += vil1_vil.h

# file format readers/writers.
SOURCES += file_formats/vil1_gen.cxx
SOURCES += file_formats/vil1_pnm.cxx
SOURCES += file_formats/vil1_viff.cxx
SOURCES += file_formats/vil1_viff_support.c
MANPAGE_SOURCES += file_formats/vil1_viffheader.h
SOURCES += file_formats/vil1_iris.cxx
SOURCES += file_formats/vil1_mit.cxx
SOURCES += file_formats/vil1_bmp_file_header.cxx
SOURCES += file_formats/vil1_bmp_core_header.cxx
SOURCES += file_formats/vil1_bmp_info_header.cxx
SOURCES += file_formats/vil1_bmp.cxx
SOURCES += file_formats/vil1_gif.cxx
SOURCES += file_formats/vil1_ras.cxx
ifeq ($(HAS_PNG),1)
USE_PNG := 1
SOURCES += file_formats/vil1_png.cxx
endif
ifeq ($(HAS_JPEG),1)
USE_JPEG := 1
SOURCES += file_formats/vil1_jpeg.cxx
SOURCES += file_formats/vil1_jpeg_source_mgr.cxx
SOURCES += file_formats/vil1_jpeg_decompressor.cxx
SOURCES += file_formats/vil1_jpeg_destination_mgr.cxx
SOURCES += file_formats/vil1_jpeg_compressor.cxx
endif
ifeq ($(HAS_TIFF),1)
USE_TIFF := 1
SOURCES += file_formats/vil1_tiff.cxx
endif

# Operations
SOURCES += vil1_image_as.cxx
SOURCES += vil1_new.cxx
SOURCES += vil1_open.cxx
SOURCES += vil1_load.cxx
SOURCES += vil1_save.cxx
SOURCES += vil1_copy.cxx
SOURCES += vil1_crop.cxx
SOURCES += vil1_skip.cxx
SOURCES += vil1_resample.cxx
SOURCES += vil1_warp.cxx
MANPAGE_SOURCES += vil1_warp.txx
SOURCES += vil1_flipud.cxx
SOURCES += vil1_flip_components.cxx
MANPAGE_SOURCES += vil1_convolve.txx
MANPAGE_SOURCES += vil1_convolve_simple.txx
MANPAGE_SOURCES += vil1_convolve_1d.txx
MANPAGE_SOURCES += vil1_convolve_1d_x.txx
MANPAGE_SOURCES += vil1_convolve_1d_y.txx
SOURCES += vil1_smooth.cxx
SOURCES += vil1_clamp.cxx
SOURCES += vil1_scale_intensities.cxx
MANPAGE_SOURCES += vil1_interpolate.txx
MANPAGE_SOURCES += vil1_resample_image.txx
MANPAGE_SOURCES += vil1_ncc.txx
MANPAGE_SOURCES += vil1_ssd.txx

# Stuff
SOURCES += vil1_pyramid.cxx
MANPAGE_SOURCES += vil1_property.h
MANPAGE_SOURCES += vil1_rgb.txx
MANPAGE_SOURCES += vil1_rgba.txx
MANPAGE_SOURCES += vil1_rgb_byte.h
SOURCES += vil1_colour_space.cxx
SOURCES += vil1_memory_image_window.cxx
MANPAGE_SOURCES += vil1_jpeglib.h
MANPAGE_SOURCES += vil1_ip_traits.h

# subdirectories
#SUBDIRS += algo

ifndef NOTEST
SUBDIRS += tests
SUBDIRS += examples
endif

ifeq ($(OS),Linux2)
  optimize := -O3 -ffast-math -fstrict-aliasing -march=pentiumpro
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################

# turn on optimization for the convolution code
FLAGS_Templates/vil1_convolve_1d.cxx := $(optimize)
FLAGS_Templates/vil1_convolve_simple+byte.byte.int.int-.cxx := $(optimize)
FLAGS_Templates/vil1_convolve_simple+byte.float.byte.float-.cxx := $(optimize)
FLAGS_Templates/vil1_convolve_simple+byte.float.float.float-.cxx := $(optimize)
FLAGS_Templates/vil1_convolve_simple+byte.float.int.float-.cxx := $(optimize)
FLAGS_Templates/vil1_convolve_simple+byte.float.short.float-.cxx := $(optimize)
FLAGS_Templates/vil1_convolve_simple+byte.short.short.int-.cxx := $(optimize)

# and for the interpolators
FLAGS_Templates/vil1_interpolate+double.double-.cxx := $(optimize)
FLAGS_Templates/vil1_interpolate+float.double-.cxx := $(optimize)
FLAGS_Templates/vil1_interpolate+float.float-.cxx := $(optimize)
FLAGS_Templates/vil1_interpolate+uchar.double-.cxx := $(optimize)
FLAGS_Templates/vil1_interpolate+uchar.uchar-.cxx := $(optimize)
FLAGS_Templates/vil1_interpolate+vil1_rgb+uchar-.vil1_rgb+double-.cxx := $(optimize)
FLAGS_Templates/vil1_interpolate+vil1_rgb+uchar-.vil1_rgb+float-.cxx := $(optimize)
FLAGS_Templates/vil1_interpolate+vil1_rgb+uchar-.vil1_rgb+uchar-.cxx := $(optimize)

# but turn off with gcc 3.0 on alpha for some files (memory overflow):
ifeq ($(OS),OSF1V4)
FLAGS_vil1_image_as.cxx := -O0
endif
