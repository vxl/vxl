#
# ./mul/vpdfl/CMakeLists.txt
#
#   Ian Scott
#



ADD_LIBRARY(vpdfl vpdfl_sources)

SOURCE_FILES(vpdfl_sources
vpdfl_axis_gaussian.cxx
vpdfl_axis_gaussian_builder.cxx
vpdfl_axis_gaussian_sampler.cxx
vpdfl_builder_base.cxx
vpdfl_gaussian.cxx
vpdfl_gaussian_builder.cxx
vpdfl_gaussian_sampler.cxx
vpdfl_pc_gaussian.cxx
vpdfl_pc_gaussian_builder.cxx
vpdfl_pc_gaussian_sampler.cxx
vpdfl_pdf_base.cxx
vpdfl_prob_chi2.cxx
vpdfl_sampler_base.cxx
vpdfl_mixture.cxx
vpdfl_mixture_builder.cxx
vpdfl_mixture_sampler.cxx
)

AUX_SOURCE_DIRECTORY(Templates vpdfl_sources)

IF( BUILD_TESTS)
  SUBDIRS(tests)
ENDIF( BUILD_TESTS)

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE( ${allvxl_SOURCE_DIR}/mul/vpdfl/CMakeListsHeaders.txt )
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)
