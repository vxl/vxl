// This is brl/bpro/core/bvgl_pro/processes/bvgl_geo_index_extent_process.cxx
#include <iostream>
#include <complex>
#include <cmath>
#include <bprb/bprb_func_process.h>
//:
// \file
// process to get geospatial extents of geo index leaves
#include <vul/vul_file.h>
#include <bvgl/algo/bvgl_2d_geo_index.h>
#include <bvgl/algo/bvgl_2d_geo_index_sptr.h>
#include <vcl_compiler.h>
#include <bpro/core/bbas_pro/bbas_1d_array_double.h>


//: process to get geospatial extents of geo index leaves
namespace bvgl_geo_index_extent_process_globals
{
  unsigned n_inputs_  = 1;
  unsigned n_outputs_ = 1;
}

bool bvgl_geo_index_extent_process_cons(bprb_func_process& pro)
{
  using namespace bvgl_geo_index_extent_process_globals;

  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string";    // input geo index text file

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "bbas_1d_array_double_sptr";   // flattened geospatial extents

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool bvgl_geo_index_extent_process(bprb_func_process& pro)
{
  using namespace bvgl_geo_index_extent_process_globals;

  // sanity check
  if (!pro.verify_inputs()) {
    std::cerr << pro.name() << ": Wrong Inputs!!!\n";
    return false;
  }

  // general iterator
  unsigned i = 0;

  // get input
  std::string geo_index_txt = pro.get_input<std::string>(0);

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
  unsigned nleaves = leaves.size();

  // save extents in flattened array
  std::vector<double> extent(4*nleaves, 0);
  for (i=0; i < nleaves; i++) {
    vgl_box_2d<double> box = leaves[i]->extent_;
    extent[4*i+0] = box.min_x(); //ll_lon
    extent[4*i+1] = box.min_y(); //ll_lat
    extent[4*i+2] = box.max_x(); //ur_lon
    extent[4*i+3] = box.max_y(); //ur_lon
  }

  // fill out bbas_1d array
  bbas_1d_array_double_sptr extent_bbas = new bbas_1d_array_double(extent.size());
  i = 0;
  for(std::vector<double>::iterator it = extent.begin(); it != extent.end(); ++it, ++i)
    extent_bbas->data_array[i] = *it;

  // generate output
  pro.set_output_val<bbas_1d_array_double_sptr>(0, extent_bbas);
  return true;
}
