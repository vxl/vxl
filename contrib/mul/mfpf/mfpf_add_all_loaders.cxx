#include "mfpf_add_all_loaders.h"
//:
// \file
#include <mbl/mbl_cloneables_factory.h>
#include <mfpf/mfpf_edge_finder.h>
#include <mfpf/mfpf_edge_finder_builder.h>
#include <mfpf/mfpf_norm_corr1d.h>
#include <mfpf/mfpf_norm_corr1d_builder.h>
#include <mfpf/mfpf_norm_corr2d.h>
#include <mfpf/mfpf_norm_corr2d_builder.h>
#include <mfpf/mfpf_grad_corr2d.h>
#include <mfpf/mfpf_grad_corr2d_builder.h>
#include <mfpf/mfpf_profile_pdf.h>
#include <mfpf/mfpf_profile_pdf_builder.h>
#include <mfpf/mfpf_region_pdf.h>
#include <mfpf/mfpf_region_pdf_builder.h>
#include <mfpf/mfpf_region_finder.h>
#include <mfpf/mfpf_region_finder_builder.h>
#include <mfpf/mfpf_lin_clsfy_finder_builder.h>
#include <mfpf/mfpf_sad_vec_cost.h>
#include <mfpf/mfpf_sad_vec_cost_builder.h>
#include <mfpf/mfpf_ssd_vec_cost.h>
#include <mfpf/mfpf_ssd_vec_cost_builder.h>
#include <mfpf/mfpf_log_lin_class_cost.h>

#include <mfpf/mfpf_hog_box_finder.h>
#include <mfpf/mfpf_hog_box_finder_builder.h>

#include <mfpf/mfpf_region_about_pt.h>
#include <mfpf/mfpf_region_about_lineseg.h>

#include <vpdfl/vpdfl_add_all_binary_loaders.h>

//: Add all binary loaders and factory objects for mfpf library
void mfpf_add_all_loaders()
{
  vpdfl_add_all_binary_loaders();

  mbl_cloneables_factory<mfpf_point_finder_builder>::add(mfpf_edge_finder_builder());
  mbl_cloneables_factory<mfpf_point_finder_builder>::add(mfpf_norm_corr1d_builder());
  mbl_cloneables_factory<mfpf_point_finder_builder>::add(mfpf_norm_corr2d_builder());
  mbl_cloneables_factory<mfpf_point_finder_builder>::add(mfpf_grad_corr2d_builder());
  mbl_cloneables_factory<mfpf_point_finder_builder>::add(mfpf_profile_pdf_builder());
  mbl_cloneables_factory<mfpf_point_finder_builder>::add(mfpf_region_pdf_builder());
  mbl_cloneables_factory<mfpf_point_finder_builder>::add(mfpf_region_finder_builder());
  mbl_cloneables_factory<mfpf_point_finder_builder>::add(mfpf_hog_box_finder_builder());
  mbl_cloneables_factory<mfpf_point_finder_builder>::add(mfpf_lin_clsfy_finder_builder());

  vsl_add_to_binary_loader(mfpf_edge_finder_builder());
  vsl_add_to_binary_loader(mfpf_norm_corr1d_builder());
  vsl_add_to_binary_loader(mfpf_grad_corr2d_builder());
  vsl_add_to_binary_loader(mfpf_profile_pdf_builder());
  vsl_add_to_binary_loader(mfpf_region_pdf_builder());
  vsl_add_to_binary_loader(mfpf_region_finder_builder());
  vsl_add_to_binary_loader(mfpf_hog_box_finder_builder());
  vsl_add_to_binary_loader(mfpf_lin_clsfy_finder_builder());

  mbl_cloneables_factory<mfpf_point_finder>::add(mfpf_edge_finder());
  mbl_cloneables_factory<mfpf_point_finder>::add(mfpf_norm_corr1d());
  mbl_cloneables_factory<mfpf_point_finder>::add(mfpf_norm_corr2d());
  mbl_cloneables_factory<mfpf_point_finder>::add(mfpf_grad_corr2d());
  mbl_cloneables_factory<mfpf_point_finder>::add(mfpf_profile_pdf());
  mbl_cloneables_factory<mfpf_point_finder>::add(mfpf_region_pdf());
  mbl_cloneables_factory<mfpf_point_finder>::add(mfpf_region_finder());
  mbl_cloneables_factory<mfpf_point_finder>::add(mfpf_hog_box_finder());
  vsl_add_to_binary_loader(mfpf_edge_finder());
  vsl_add_to_binary_loader(mfpf_norm_corr1d());
  vsl_add_to_binary_loader(mfpf_norm_corr2d());
  vsl_add_to_binary_loader(mfpf_grad_corr2d());
  vsl_add_to_binary_loader(mfpf_profile_pdf());
  vsl_add_to_binary_loader(mfpf_region_pdf());
  vsl_add_to_binary_loader(mfpf_region_finder());
  vsl_add_to_binary_loader(mfpf_hog_box_finder());

  mbl_cloneables_factory<mfpf_vec_cost_builder>::add(mfpf_sad_vec_cost_builder());
  mbl_cloneables_factory<mfpf_vec_cost_builder>::add(mfpf_ssd_vec_cost_builder());
  vsl_add_to_binary_loader(mfpf_sad_vec_cost_builder());
  vsl_add_to_binary_loader(mfpf_sad_vec_cost());
  vsl_add_to_binary_loader(mfpf_ssd_vec_cost_builder());
  vsl_add_to_binary_loader(mfpf_ssd_vec_cost());
  vsl_add_to_binary_loader(mfpf_log_lin_class_cost());

  mbl_cloneables_factory<mfpf_region_definer>::add(mfpf_region_about_pt());
  mbl_cloneables_factory<mfpf_region_definer>::add(mfpf_region_about_lineseg());

  vsl_add_to_binary_loader(mfpf_region_about_pt());
  vsl_add_to_binary_loader(mfpf_region_about_lineseg());


}
