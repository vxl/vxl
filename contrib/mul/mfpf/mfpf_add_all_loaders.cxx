#include <mbl/mbl_cloneables_factory.h>
#include <mfpf/mfpf_edge_finder.h>
#include <mfpf/mfpf_edge_finder_builder.h>
#include <mfpf/mfpf_norm_corr1d.h>
#include <mfpf/mfpf_norm_corr1d_builder.h>
#include <mfpf/mfpf_norm_corr2d.h>
#include <mfpf/mfpf_norm_corr2d_builder.h>
#include <mfpf/mfpf_profile_pdf.h>
#include <mfpf/mfpf_profile_pdf_builder.h>
#include <mfpf/mfpf_region_pdf.h>
#include <mfpf/mfpf_region_pdf_builder.h>

#include <vpdfl/vpdfl_add_all_binary_loaders.h>

//: Add all binary loaders and factory objects for mfpf library
void mfpf_add_all_loaders()
{
  vpdfl_add_all_binary_loaders();

  mbl_cloneables_factory<mfpf_point_finder_builder>::add(mfpf_edge_finder_builder());
  mbl_cloneables_factory<mfpf_point_finder_builder>::add(mfpf_norm_corr1d_builder());
  mbl_cloneables_factory<mfpf_point_finder_builder>::add(mfpf_norm_corr2d_builder());
  mbl_cloneables_factory<mfpf_point_finder_builder>::add(mfpf_profile_pdf_builder());
  mbl_cloneables_factory<mfpf_point_finder_builder>::add(mfpf_region_pdf_builder());
  vsl_add_to_binary_loader(mfpf_edge_finder_builder());
  vsl_add_to_binary_loader(mfpf_norm_corr1d_builder());
  vsl_add_to_binary_loader(mfpf_norm_corr2d_builder());
  vsl_add_to_binary_loader(mfpf_profile_pdf_builder());
  vsl_add_to_binary_loader(mfpf_region_pdf_builder());

  mbl_cloneables_factory<mfpf_point_finder>::add(mfpf_edge_finder());
  mbl_cloneables_factory<mfpf_point_finder>::add(mfpf_norm_corr1d());
  mbl_cloneables_factory<mfpf_point_finder>::add(mfpf_norm_corr2d());
  mbl_cloneables_factory<mfpf_point_finder>::add(mfpf_profile_pdf());
  mbl_cloneables_factory<mfpf_point_finder>::add(mfpf_region_pdf());
  vsl_add_to_binary_loader(mfpf_edge_finder());
  vsl_add_to_binary_loader(mfpf_norm_corr1d());
  vsl_add_to_binary_loader(mfpf_norm_corr2d());
  vsl_add_to_binary_loader(mfpf_profile_pdf());
  vsl_add_to_binary_loader(mfpf_region_pdf());
}
