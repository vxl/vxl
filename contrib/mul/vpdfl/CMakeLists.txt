# ./mul/vpdfl/CMakeLists.txt

ADD_LIBRARY(vpdfl vpdfl_sources)

SOURCE_FILES(vpdfl_sources
  vpdfl_resample.cxx
  vpdfl_calc_mean_var.cxx
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
  vpdfl_kernel_pdf.cxx
  vpdfl_kernel_pdf_builder.cxx
  vpdfl_gaussian_kernel_pdf.cxx
  vpdfl_gaussian_kernel_pdf_sampler.cxx
  vpdfl_gaussian_kernel_pdf_builder.cxx
  vpdfl_add_all_binary_loaders.cxx
)

AUX_SOURCE_DIRECTORY(Templates vpdfl_sources)

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE(${allvxl_SOURCE_DIR}/mul/vpdfl/CMakeListsHeaders.txt)
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)

IF( BUILD_TESTING)
  SUBDIRS(tests)
ENDIF( BUILD_TESTING)
