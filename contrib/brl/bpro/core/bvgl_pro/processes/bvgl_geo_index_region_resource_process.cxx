// This is brl/bpro/core/bvgl_pro/processes/bvgl_geo_index_region_resource.cxx
#include <iostream>
#include <complex>
#include <bprb/bprb_func_process.h>
//:
// \file
// processes to generate list of leaves that intersect with given region
#include <vul/vul_file.h>
#include <vgl/vgl_intersection.h>
#include <vgl/vgl_area.h>
#include <bvgl/algo/bvgl_2d_geo_index.h>
#include <bvgl/algo/bvgl_2d_geo_index_sptr.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bkml/bkml_parser.h>


//: process to get list of leaves that intersect with given 2-d bbox region
namespace bvgl_geo_index_region_resource_process_globals
{
  unsigned n_inputs_  = 6;
  unsigned n_outputs_ = 1;
}

bool bvgl_geo_index_region_resource_process_cons(bprb_func_process& pro)
{
  using namespace bvgl_geo_index_region_resource_process_globals;
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string";    // input geo index text file
  input_types_[1] = "double";        // lower left lon
  input_types_[2] = "double";        // lower left lat
  input_types_[3] = "double";        // upper right lon
  input_types_[4] = "double";        // upper right lat
  input_types_[5] = "vcl_string";    // output file to store the ids of leaves that intersect with the given region
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "unsigned";     // number of leaves that intersect with given region
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool bvgl_geo_index_region_resource_process(bprb_func_process& pro)
{
  using namespace bvgl_geo_index_region_resource_process_globals;
  // sanity check
  if (!pro.verify_inputs()) {
    std::cerr << pro.name() << ": Wrong Inputs!!!\n";
    return false;
  }
  // get inputs
  unsigned i = 0;
  std::string geo_index_txt = pro.get_input<std::string>(i++);
  auto ll_lon = pro.get_input<double>(i++);
  auto ll_lat = pro.get_input<double>(i++);
  auto ur_lon = pro.get_input<double>(i++);
  auto ur_lat = pro.get_input<double>(i++);
  std::string out_txt = pro.get_input<std::string>(i++);
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
  // get the ids of intersected leaves
  std::vector<unsigned> leaf_ids;
  vgl_box_2d<double> bbox(ll_lon, ur_lon, ll_lat, ur_lat);
  std::cout << "bounding box: " << bbox << std::endl;

  for (unsigned i = 0; i < leaves.size(); i++) {
    if (vgl_area(vgl_intersection(bbox, leaves[i]->extent_)) > 0)
      leaf_ids.push_back(i);
  }

  // write it to file
  std::ofstream ofs(out_txt.c_str());
  if (!ofs.is_open()) {
    std::cerr << pro.name() << ": can not write into file " << out_txt << "!\n";
    return false;
  }
  ofs << leaf_ids.size() << " leaves intersect with region [" << ll_lon << ',' << ll_lat << "] , [" << ur_lon << ',' << ur_lat << "]\n";
  for (unsigned int leaf_id : leaf_ids)
    ofs << leaf_id << '\n';
  ofs.close();
  pro.set_output_val<unsigned>(0, leaf_ids.size());
  return true;
}

//: process to get leaves that intersect with given polygon (input as kml)
namespace bvgl_geo_index_region_poly_resource_process_globals
{
  unsigned n_inputs_  = 3;
  unsigned n_outputs_ = 1;
}

bool bvgl_geo_index_region_poly_resource_process_cons(bprb_func_process& pro)
{
  using namespace bvgl_geo_index_region_poly_resource_process_globals;
  // this process takes 3 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string";  // input geo index text file
  input_types_[1] = "vcl_string";  // input polygon kml file
  input_types_[2] = "vcl_string";  // output file to store the ids of leaves that intersect with input polygon region
  // this process takes 1 outputs
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "unsigned";   // number of leaves that intersect with given polygon region
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool bvgl_geo_index_region_poly_resource_process(bprb_func_process& pro)
{
  using namespace bvgl_geo_index_region_poly_resource_process_globals;
  // sanity check
  if (!pro.verify_inputs()) {
    std::cerr << pro.name() << ": Wrong Inputs!\n";
    return false;
  }
  // get inputs
  unsigned in_i = 0;
  std::string geo_index_txt = pro.get_input<std::string>(in_i++);
  std::string poly_kml_file = pro.get_input<std::string>(in_i++);
  std::string out_txt       = pro.get_input<std::string>(in_i++);

  // read the tree
  if (!vul_file::exists(geo_index_txt)) {
    std::cerr << pro.name() << ": can not find geo index file " << geo_index_txt << "!\n";
    return false;
  }
  double min_size;
  bvgl_2d_geo_index_node_sptr root = bvgl_2d_geo_index::read_and_construct<float>(geo_index_txt, min_size);
  std::vector<bvgl_2d_geo_index_node_sptr> leaves;
  bvgl_2d_geo_index::get_leaves(root, leaves);

  // read the polygon
  if (!vul_file::exists(poly_kml_file)) {
    std::cerr << pro.name() << ": can not find input polygon kml file " << poly_kml_file << "!\n";
    return false;
  }
  vgl_polygon<double> poly = bkml_parser::parse_polygon(poly_kml_file);

  // get the ids of intersected leaves
  std::vector<unsigned> leaf_ids;
  for (unsigned i = 0; i < leaves.size(); i++)
    if (vgl_intersection(leaves[i]->extent_, poly))
      leaf_ids.push_back(i);

  // write to output file
  std::ofstream ofs(out_txt.c_str());
  if (!ofs.is_open()) {
    std::cerr << pro.name() << ": can not write into file " << out_txt << "!\n";
    return false;
  }
  ofs << leaf_ids.size() << " leaves intersect with given polygon!\n";
  for (unsigned int leaf_id : leaf_ids)
    ofs << leaf_id << '\n';
  ofs.close();

  // output
  pro.set_output_val<unsigned>(0, leaf_ids.size());
  return true;
}
