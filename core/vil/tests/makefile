USES += core
include ../config.mk
include ${VXLROOT}/core/vul/config.mk
include ${VXLROOT}/core/vpl/config.mk
include ${VXLROOT}/core/testlib/config.mk

# This is where vil/vil_config.h resides
INCDIRS += ../$(RELOBJDIR)/include

TESTMAIN = test_driver.cxx

# basic things
TESTS += test_image_resource.cxx
TESTS += test_image_view.cxx
TESTS += test_memory_chunk.cxx
TESTS += test_pixel_format.cxx

# file format readers/writers
TESTS += test_file_format_read.cxx
PARAMS_test_file_format_read := file_read_data
TESTS += test_save_load_image.cxx
TESTS += test_image_loader_robustness.cxx
TESTS += test_stream.cxx
PARAMS_test_stream := file_read_data

# image operations
TESTS += test_deep_copy_3_plane.cxx
TESTS += test_image_view_maths.cxx
TESTS += test_convert.cxx
PARAMS_test_convert := file_read_data
TESTS += test_rotate_image.cxx
TESTS += test_warp.cxx

# Sampling Operations
TESTS += test_bilin_interp.cxx
TESTS += test_sample_profile_bilin.cxx
TESTS += test_sample_grid_bilin.cxx
TESTS += test_resample_bilin.cxx
TESTS += test_bicub_interp.cxx
TESTS += test_sample_profile_bicub.cxx
TESTS += test_sample_grid_bicub.cxx
TESTS += test_resample_bicub.cxx

# Math
TESTS += test_math_value_range.cxx

# Blocking
TESTS += test_blocked_image_resource.cxx

#MINI_PROG_SOURCES += vil_test_access_timings.cxx
#MINI_PROG_SOURCES += vil_test_random_access_timings.cxx
#MINI_PROG_SOURCES += vil_test_access_timings_noplanes.cxx
#MINI_PROG_SOURCES += vil_test_pair_access_timings.cxx

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
