# mul/vpdfl/CMakeLists.txt

doxygen_add_library(contrib/mul/vpdfl
  DEPENDS contrib/mul/mbl core/vnl
  PACKAGE contrib/mul
  DESCRIPTION "Multivariate Probability Density Function Library"
  )

set(vpdfl_sources
 vpdfl_all.h
 vpdfl_resample.cxx                    vpdfl_resample.h
 vpdfl_calc_mean_var.cxx               vpdfl_calc_mean_var.h
 vpdfl_axis_gaussian.cxx               vpdfl_axis_gaussian.h
 vpdfl_axis_gaussian_builder.cxx       vpdfl_axis_gaussian_builder.h
 vpdfl_axis_gaussian_sampler.cxx       vpdfl_axis_gaussian_sampler.h
 vpdfl_builder_base.cxx                vpdfl_builder_base.h
 vpdfl_gaussian.cxx                    vpdfl_gaussian.h
 vpdfl_gaussian_builder.cxx            vpdfl_gaussian_builder.h
 vpdfl_gaussian_sampler.cxx            vpdfl_gaussian_sampler.h
 vpdfl_pc_gaussian.cxx                 vpdfl_pc_gaussian.h
 vpdfl_pc_gaussian_builder.cxx         vpdfl_pc_gaussian_builder.h
 vpdfl_pc_gaussian_sampler.cxx         vpdfl_pc_gaussian_sampler.h
 vpdfl_pdf_base.cxx                    vpdfl_pdf_base.h
 vpdfl_prob_chi2.cxx                   vpdfl_prob_chi2.h
 vpdfl_sampler_base.cxx                vpdfl_sampler_base.h
 vpdfl_mixture.cxx                     vpdfl_mixture.h
 vpdfl_mixture_builder.cxx             vpdfl_mixture_builder.h
 vpdfl_mixture_sampler.cxx             vpdfl_mixture_sampler.h
 vpdfl_kernel_pdf.cxx                  vpdfl_kernel_pdf.h
 vpdfl_kernel_pdf_builder.cxx          vpdfl_kernel_pdf_builder.h
 vpdfl_gaussian_kernel_pdf.cxx         vpdfl_gaussian_kernel_pdf.h
 vpdfl_gaussian_kernel_pdf_sampler.cxx vpdfl_gaussian_kernel_pdf_sampler.h
 vpdfl_gaussian_kernel_pdf_builder.cxx vpdfl_gaussian_kernel_pdf_builder.h
 vpdfl_add_all_binary_loaders.cxx      vpdfl_add_all_binary_loaders.h
)

aux_source_directory(Templates vpdfl_sources)

vxl_add_library(LIBRARY_NAME vpdfl LIBRARY_SOURCES ${vpdfl_sources})
target_link_libraries(vpdfl mbl ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vnl_io ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}vul)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
