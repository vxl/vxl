// This is brl/bpro/core/bvgl_pro/processes/bvgl_geo_index_region_overlap_process.cxx
#include <iostream>
#include <complex>
#include <bprb/bprb_func_process.h>
//:
// \file
// process to generate overlap of leaves to a given region
#include "vul/vul_file.h"
#include "vgl/vgl_intersection.h"
#include "vgl/vgl_area.h"
#include <bvgl/algo/bvgl_2d_geo_index.h>
#include <bvgl/algo/bvgl_2d_geo_index_sptr.h>
#include <vcl_compiler.h>
#include <bpro/core/bbas_pro/bbas_1d_array_double.h>


//: process to get leaf overlap with given 2-d bbox region
namespace bvgl_geo_index_region_overlap_process_globals
{
  unsigned n_inputs_  = 5;
  unsigned n_outputs_ = 1;
}

bool bvgl_geo_index_region_overlap_process_cons(bprb_func_process& pro)
{
  using namespace bvgl_geo_index_region_overlap_process_globals;

  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string";    // input geo index text file
  input_types_[1] = "double";        // lower left lon
  input_types_[2] = "double";        // lower left lat
  input_types_[3] = "double";        // upper right lon
  input_types_[4] = "double";        // upper right lat

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "bbas_1d_array_double_sptr";   // per-leaf overlap with given region

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool bvgl_geo_index_region_overlap_process(bprb_func_process& pro)
{
  using namespace bvgl_geo_index_region_overlap_process_globals;

  // sanity check
  if (!pro.verify_inputs()) {
    std::cerr << pro.name() << ": Wrong Inputs!!!\n";
    return false;
  }

  // general iterator
  unsigned i = 0;

  // get inputs
  i = 0;
  std::string geo_index_txt = pro.get_input<std::string>(i++);
  double ll_lon = pro.get_input<double>(i++);
  double ll_lat = pro.get_input<double>(i++);
  double ur_lon = pro.get_input<double>(i++);
  double ur_lat = pro.get_input<double>(i++);

  // read the tree
  // since we are interested only for the structure, type of the tree doesn't matter here
  if (!vul_file::exists(geo_index_txt)) {
    std::cerr << pro.name() << ": can not find geo index file " << geo_index_txt << "!\n";
    return false;
  }
  double min_size;
  bvgl_2d_geo_index_node_sptr root = bvgl_2d_geo_index::read_and_construct<float>(geo_index_txt, min_size);
  std::vector<bvgl_2d_geo_index_node_sptr> leaves;
  bvgl_2d_geo_index::get_leaves(root, leaves);

  // test region bounding box
  vgl_box_2d<double> bbox(ll_lon, ur_lon, ll_lat, ur_lat);
  std::cout << "bounding box: " << bbox << std::endl;

  // % overlap of each leaf with bounding box
  unsigned nleaves = leaves.size();
  std::vector<double> overlap(nleaves,0.0);

  for (i = 0; i < nleaves; i++) {
    double leaf_area_total = vgl_area(leaves[i]->extent_);
    double leaf_area_overlap = vgl_area(vgl_intersection(bbox, leaves[i]->extent_));
    overlap[i] = leaf_area_overlap/leaf_area_total;
  }

  // fill out bbas_1d array
  bbas_1d_array_double_sptr overlap_bbas = new bbas_1d_array_double(nleaves);
  i = 0;
  for(std::vector<double>::iterator it = overlap.begin(); it != overlap.end(); ++it, ++i)
    overlap_bbas->data_array[i] = *it;

  // generate output
  pro.set_output_val<bbas_1d_array_double_sptr>(0, overlap_bbas);
  return true;
}
