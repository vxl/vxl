#
# ./mul/pdf1d/tools/CMakeLists.txt
#
#   Tim Cootes
#

LINK_LIBRARIES( pdf1d mbl vnl )

ADD_EXECUTABLE(
pdf1d_test_overlap_pdf test_overlap_pdf.cxx
)

ADD_EXECUTABLE(
compare_kernel_estimates compare_kernel_estimates.cxx
)

ADD_EXECUTABLE(
pdf1d_select_n_mixtures select_n_mixtures.cxx
)

ADD_EXECUTABLE(
select_pdf select_pdf.cxx
)

ADD_EXECUTABLE(
plot_pdf_estimate plot_pdf_estimate.cxx
)
