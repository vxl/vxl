// This is brl/bpro/core/bvgl_pro/processes/bvgl_geo_index_region_distance_process.cxx
#include <iostream>
#include <complex>
#include <cmath>
#include <bprb/bprb_func_process.h>
//:
// \file
// process to generate distance of leaves to given geolocation
#include <vul/vul_file.h>
#include <vpgl/vpgl_lvcs.h>
#include <bvgl/algo/bvgl_2d_geo_index.h>
#include <bvgl/algo/bvgl_2d_geo_index_sptr.h>
#include <vcl_compiler.h>
#include <bpro/core/bbas_pro/bbas_1d_array_double.h>
#include <bpro/core/bbas_pro/bbas_1d_array_unsigned.h>


//: process to get leaf distance to given lon/lat location
// returns approximate meter distance between centroid of leaf extent
// and input lon/lat location
namespace bvgl_geo_index_region_distance_process_globals
{
  unsigned n_inputs_  = 4;
  unsigned n_outputs_ = 2;
}

bool bvgl_geo_index_region_distance_process_cons(bprb_func_process& pro)
{
  using namespace bvgl_geo_index_region_distance_process_globals;

  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string";    // input geo index text file
  input_types_[1] = "double";        // longitude
  input_types_[2] = "double";        // latitude
  input_types_[3] = "unsigned";      // max nodes for distance calc

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "bbas_1d_array_double_sptr";    // distance of unique nodes to lon/lat
  output_types_[1] = "bbas_1d_array_unsigned_sptr";  // per-leaf index into unique node list

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool bvgl_geo_index_region_distance_process(bprb_func_process& pro)
{
  using namespace bvgl_geo_index_region_distance_process_globals;

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
  double lon = pro.get_input<double>(i++);
  double lat = pro.get_input<double>(i++);
  unsigned max_nodes = pro.get_input<unsigned>(i++);

  // read the tree
  if (!vul_file::exists(geo_index_txt)) {
    std::cerr << pro.name() << ": can not find geo index file " << geo_index_txt << "!\n";
    return false;
  }
  double min_size;
  bvgl_2d_geo_index_node_sptr root = bvgl_2d_geo_index::read_and_construct<float>(geo_index_txt, min_size);

  // get all tree leaves
  std::vector<bvgl_2d_geo_index_node_sptr> nodes;
  bvgl_2d_geo_index::get_leaves(root, nodes);
  unsigned n_leaves = nodes.size();
  std::cout << "n_leaves = " << n_leaves << std::endl;

  // traverse hierarchy upwards until number of unique nodes is less
  // than the maximum number of nodes
  unsigned n_nodes = n_leaves;
  if (max_nodes>0) {
    while (n_nodes > max_nodes && n_nodes != 1) {

      // set every node to its parent
      for (i = 0; i < n_leaves; i++)
        nodes[i] = nodes[i]->parent_;

      // count unique nodes
      n_nodes = 1;
      for (i = 1; i < n_leaves; i++)
        if (nodes[i] != nodes[i-1]) {n_nodes++;}

      std::cout << "move to parent, n_nodes = " << n_nodes << std::endl;

    }
  }

  // unique nodes & leaf indices into unique nodes
  std::vector<bvgl_2d_geo_index_node_sptr> nodes_unique;
  std::vector<unsigned> indices(n_leaves,0);

  for (i = 0; i < n_leaves; i++) {
    if (i==0 || nodes[i]!=nodes[i-1])
      nodes_unique.push_back(nodes[i]);
    indices[i] = nodes_unique.size()-1;
  }

  // LVCS for input location
  vpgl_lvcs lvcs(lat, lon, 0.0, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);

  // distance of each unique node to lvcs origin
  std::vector<double> distance(n_nodes,0.0);
  for (i = 0; i < n_nodes; i++) {
    vgl_point_2d<double> xy = nodes_unique[i]->extent_.centroid();
    double lx, ly, lz;
    lvcs.global_to_local(xy.x(),xy.y(),0.0,
                         vpgl_lvcs::wgs84, lx,ly,lz,
                         vpgl_lvcs::DEG, vpgl_lvcs::METERS);
    distance[i] = std::sqrt(lx*lx + ly*ly);
  }

  // fill out bbas_1d arrays
  bbas_1d_array_double_sptr distance_bbas = new bbas_1d_array_double(distance.size());
  i = 0;
  for(std::vector<double>::iterator it = distance.begin(); it != distance.end(); ++it, ++i)
    distance_bbas->data_array[i] = *it;

  bbas_1d_array_unsigned_sptr indices_bbas = new bbas_1d_array_unsigned(indices.size());
  i = 0;
  for(std::vector<unsigned>::iterator it = indices.begin(); it != indices.end(); ++it, ++i)
    indices_bbas->data_array[i] = *it;

  // generate output
  pro.set_output_val<bbas_1d_array_double_sptr>(0, distance_bbas);
  pro.set_output_val<bbas_1d_array_unsigned_sptr>(1, indices_bbas);
  return true;
}
