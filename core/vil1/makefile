######################################
include ${IUEROOT}/config/top-params.mk
#######################################

USES += vxl

LIBRARY := vil
VERSION := 1.0

MANPAGE_SOURCES += vil_fwd.h

# basic things
SOURCES += vil_pixel.cxx
SOURCES += vil_image_impl.cxx
SOURCES += vil_image.cxx
SOURCES += vil_stream.cxx
SOURCES += vil_file_format.cxx
SOURCES += vil_byte_swap.cxx
SOURCES += vil_16bit.cxx
SOURCES += vil_32bit.cxx

# various vil_stream classes
SOURCES += vil_stream_fstream.cxx     # from a file
SOURCES += vil_stream_core.cxx        # in-core vil_stream
SOURCES += vil_stream_section.cxx     # present a section of a vil_stream
SOURCES += vil_stream_url.cxx         # load image from URL

# classes derived from vil_image
SOURCES += vil_file_image.cxx
SOURCES += vil_memory_image.cxx
MANPAGE_SOURCES += vil_memory_image_of.txx
MANPAGE_SOURCES += vil_memory_image_of_format.txx
SOURCES += vil_image_proxy.cxx

# image adaptors
SOURCES += vil_memory_image_impl.cxx
SOURCES += vil_crop_image_impl.cxx
SOURCES += vil_skip_image_impl.cxx
SOURCES += vil_resample_image_impl.cxx
SOURCES += vil_flipud_impl.cxx
SOURCES += vil_flip_components_impl.cxx
SOURCES += vil_block_cache_image_impl.cxx
SOURCES += vil_clamp_image_impl.cxx
SOURCES += vil_clamp_image.cxx # Helper specializations for vil_clamp_image_impl.cxx
SOURCES += vil_scale_intensities_image_impl.cxx
SOURCES += vil_scale_intensities_image.cxx # Helper specializations for vil_scale_intensities_image_impl.cxx

# file format readers/writers.
SOURCES += file_formats/vil_gen.cxx
SOURCES += file_formats/vil_pnm.cxx
SOURCES += file_formats/vil_viff.cxx
SOURCES += file_formats/vil_viff_support.c
MANPAGE_SOURCES += file_formats/vil_viffheader.h
SOURCES += file_formats/vil_iris.cxx
SOURCES += file_formats/vil_mit.cxx
SOURCES += file_formats/vil_bmp_file_header.cxx
SOURCES += file_formats/vil_bmp_core_header.cxx
SOURCES += file_formats/vil_bmp_info_header.cxx
SOURCES += file_formats/vil_bmp.cxx
SOURCES += file_formats/vil_gif.cxx
SOURCES += file_formats/vil_ras.cxx
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
SOURCES += vil_image_as.cxx
SOURCES += vil_new.cxx
SOURCES += vil_open.cxx
SOURCES += vil_load.cxx
SOURCES += vil_save.cxx
SOURCES += vil_copy.cxx
SOURCES += vil_crop.cxx
SOURCES += vil_skip.cxx
SOURCES += vil_resample.cxx
SOURCES += vil_warp.cxx
MANPAGE_SOURCES += vil_warp.txx
SOURCES += vil_flipud.cxx
SOURCES += vil_flip_components.cxx
MANPAGE_SOURCES += vil_convolve.txx
MANPAGE_SOURCES += vil_convolve_simple.txx
MANPAGE_SOURCES += vil_convolve_1d.txx
MANPAGE_SOURCES += vil_convolve_1d_x.txx
MANPAGE_SOURCES += vil_convolve_1d_y.txx
SOURCES += vil_smooth.cxx
SOURCES += vil_clamp.cxx
SOURCES += vil_scale_intensities.cxx
MANPAGE_SOURCES += vil_interpolate.txx
MANPAGE_SOURCES += vil_resample_image.txx
MANPAGE_SOURCES += vil_ncc.txx
MANPAGE_SOURCES += vil_ssd.txx

# Stuff
SOURCES += vil_pyramid.cxx
MANPAGE_SOURCES += vil_property.h
MANPAGE_SOURCES += vil_byte.h
MANPAGE_SOURCES += vil_rgb.txx
MANPAGE_SOURCES += vil_rgba.txx
SOURCES += vil_rgb_byte.cxx
SOURCES += vil_colour_space.cxx
SOURCES += vil_memory_image_window.cxx
MANPAGE_SOURCES += vil_jpeglib.h
MANPAGE_SOURCES += vil_ip_traits.h

ifeq ($(OS),Linux2)
  optimize := -O3 -ffast-math -fstrict-aliasing -march=pentiumpro
endif

# turn on optimization for the convolution code
FLAGS_Templates/vil_convolve_1d.cxx := $(optimize)
FLAGS_Templates/vil_convolve_simple+byte.byte.int.int-.cxx := $(optimize)
FLAGS_Templates/vil_convolve_simple+byte.float.byte.float-.cxx := $(optimize)
FLAGS_Templates/vil_convolve_simple+byte.float.float.float-.cxx := $(optimize)
FLAGS_Templates/vil_convolve_simple+byte.float.int.float-.cxx := $(optimize)
FLAGS_Templates/vil_convolve_simple+byte.float.short.float-.cxx := $(optimize)
FLAGS_Templates/vil_convolve_simple+byte.short.short.int-.cxx := $(optimize)

# and for the interpolators
FLAGS_Templates/vil_interpolate+double.double-.cxx := $(optimize)
FLAGS_Templates/vil_interpolate+float.double-.cxx := $(optimize)
FLAGS_Templates/vil_interpolate+float.float-.cxx := $(optimize)
FLAGS_Templates/vil_interpolate+uchar.double-.cxx := $(optimize)
FLAGS_Templates/vil_interpolate+uchar.uchar-.cxx := $(optimize)
FLAGS_Templates/vil_interpolate+vil_rgb+uchar-.vil_rgb+double-.cxx := $(optimize)
FLAGS_Templates/vil_interpolate+vil_rgb+uchar-.vil_rgb+float-.cxx := $(optimize)
FLAGS_Templates/vil_interpolate+vil_rgb+uchar-.vil_rgb+uchar-.cxx := $(optimize)

# subdirectories
#SUBDIRS += algo
SUBDIRS += io

ifndef NOTEST
SUBDIRS += tests
SUBDIRS += examples
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
