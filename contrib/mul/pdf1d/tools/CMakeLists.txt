#
# ./mul/pdf1d/tools/CMakeLists.txt
#
#   Tim Cootes
#

link_libraries( pdf1d mbl ${VXL_LIB_PREFIX}vnl )

add_executable(
pdf1d_test_overlap_pdf test_overlap_pdf.cxx
)

add_executable(
compare_kernel_estimates compare_kernel_estimates.cxx
)

add_executable(
pdf1d_select_n_mixtures select_n_mixtures.cxx
)

add_executable(
select_pdf select_pdf.cxx
)

add_executable(
plot_pdf_estimate plot_pdf_estimate.cxx
)
