# mul/pdf1d/CMakeLists.txt

SET(pdf1d_sources
  pdf1d_all.h                                      pdf1d_add_all_loaders.cxx
  pdf1d_pdf.cxx                                    pdf1d_pdf.h
  pdf1d_sampler.cxx                                pdf1d_sampler.h
  pdf1d_builder.cxx                                pdf1d_builder.h
  pdf1d_prob_chi2.cxx                              pdf1d_prob_chi2.h
  pdf1d_gaussian.cxx                               pdf1d_gaussian.h
  pdf1d_gaussian_sampler.cxx                       pdf1d_gaussian_sampler.h
  pdf1d_gaussian_builder.cxx                       pdf1d_gaussian_builder.h
  pdf1d_flat.cxx                                   pdf1d_flat.h
  pdf1d_flat_sampler.cxx                           pdf1d_flat_sampler.h
  pdf1d_flat_builder.cxx                           pdf1d_flat_builder.h
  pdf1d_exponential.cxx                            pdf1d_exponential.h
  pdf1d_exponential_sampler.cxx                    pdf1d_exponential_sampler.h
  pdf1d_exponential_builder.cxx                    pdf1d_exponential_builder.h
  pdf1d_kernel_pdf.cxx                             pdf1d_kernel_pdf.h
  pdf1d_kernel_pdf_builder.cxx                     pdf1d_kernel_pdf_builder.h
  pdf1d_mixture.cxx                                pdf1d_gaussian_kernel_pdf.h
  pdf1d_mixture_sampler.cxx                        pdf1d_gaussian_kernel_pdf_sampler.h
  pdf1d_mixture_builder.cxx                        pdf1d_gaussian_kernel_pdf_builder.h
  pdf1d_gaussian_kernel_pdf.cxx                    pdf1d_epanech_kernel_pdf.h
  pdf1d_gaussian_kernel_pdf_sampler.cxx            pdf1d_epanech_kernel_pdf_sampler.h
  pdf1d_gaussian_kernel_pdf_builder.cxx            pdf1d_epanech_kernel_pdf_builder.h
  pdf1d_epanech_kernel_pdf.cxx                     pdf1d_weighted_kernel_pdf.h
  pdf1d_epanech_kernel_pdf_sampler.cxx             pdf1d_weighted_epanech_kernel_pdf.h
  pdf1d_epanech_kernel_pdf_builder.cxx             pdf1d_weighted_epanech_kernel_sampler.h
  pdf1d_weighted_kernel_pdf.cxx                    pdf1d_mixture.h
  pdf1d_weighted_epanech_kernel_pdf.cxx            pdf1d_mixture_sampler.h
  pdf1d_weighted_epanech_kernel_sampler.cxx        pdf1d_mixture_builder.h
  pdf1d_bhat_overlap.cxx                           pdf1d_bhat_overlap.h
  pdf1d_calc_mean_var.cxx                          pdf1d_calc_mean_var.h
  pdf1d_resample.cxx                               pdf1d_resample.h
  pdf1d_compare_to_pdf.cxx                         pdf1d_compare_to_pdf.h
  pdf1d_compare_to_pdf_bhat.cxx                    pdf1d_compare_to_pdf_bhat.h
  pdf1d_compare_to_pdf_ks.cxx                      pdf1d_compare_to_pdf_ks.h
  pdf1d_prob_ks.cxx                                pdf1d_prob_ks.h
  pdf1d_compare_samples.cxx                        pdf1d_compare_samples.h
  pdf1d_select_pdf.cxx                             pdf1d_select_pdf.h
)

AUX_SOURCE_DIRECTORY(Templates pdf1d_sources)

ADD_LIBRARY(pdf1d ${pdf1d_sources})
INSTALL_TARGETS(/lib pdf1d)
INSTALL_NOBASE_HEADER_FILES(/include/contrib/mul/pdf1d ${pdf1d_sources})
TARGET_LINK_LIBRARIES( pdf1d mbl vbl vnl_io vnl vsl )

SUBDIRS(tools)

IF( BUILD_TESTING)
  SUBDIRS(tests)
ENDIF( BUILD_TESTING)
