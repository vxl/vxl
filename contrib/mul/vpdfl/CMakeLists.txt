#
# ./mul/vpdfl/CMakeLists.txt
#
#   Ian Scott
#



ADD_LIBRARY(vpdfl vpdfl_sources)

SOURCE_FILES(vpdfl_sources
vpdfl_axis_gaussian
vpdfl_axis_gaussian_builder
vpdfl_axis_gaussian_sampler
vpdfl_builder_base
vpdfl_gaussian
vpdfl_gaussian_builder
vpdfl_gaussian_sampler
vpdfl_pc_gaussian
vpdfl_pc_gaussian_builder
vpdfl_pc_gaussian_sampler
vpdfl_pdf_base
vpdfl_prob_chi2
vpdfl_sampler_base
vpdfl_mixture
vpdfl_mixture_builder
vpdfl_mixture_sampler
)

AUX_SOURCE_DIRECTORY(Templates vpdfl_sources)

IF( BUILD_TESTS)
  SUBDIRS(tests)
ENDIF( BUILD_TESTS)