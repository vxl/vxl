// This is brl/bpro/core/bvgl_pro/processes/bvgl_geo_index_region_resource.cxx
#include <bprb/bprb_func_process.h>
// :
// \file
// process to generate list of leaves that intersect with given region
#include <vul/vul_file.h>
#include <vgl/vgl_intersection.h>
#include <bvgl/algo/bvgl_2d_geo_index.h>
#include <bvgl/algo/bvgl_2d_geo_index_sptr.h>
#include <vcl_complex.h>

namespace bvgl_geo_index_region_resource_process_globals
{
unsigned n_inputs_  = 6;
unsigned n_outputs_ = 1;
}

bool bvgl_geo_index_region_resource_process_cons(bprb_func_process& pro)
{
  using namespace bvgl_geo_index_region_resource_process_globals;
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string";    // input geo index text file
  input_types_[1] = "double";        // lower left lon
  input_types_[2] = "double";        // lower left lat
  input_types_[3] = "double";        // upper right lon
  input_types_[4] = "double";        // upper right lat
  input_types_[5] = "vcl_string";    // output file to store the ids of leaves that intersect with the given region
  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0] = "unsigned";     // number of leaves that intersect with given region
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool bvgl_geo_index_region_resource_process(bprb_func_process& pro)
{
  using namespace bvgl_geo_index_region_resource_process_globals;
  // sanity check
  if( !pro.verify_inputs() )
    {
    vcl_cerr << pro.name() << ": Wrong Inputs!!!\n";
    return false;
    }
  // get inputs
  unsigned   i = 0;
  vcl_string geo_index_txt = pro.get_input<vcl_string>(i++);
  double     ll_lon = pro.get_input<double>(i++);
  double     ll_lat = pro.get_input<double>(i++);
  double     ur_lon = pro.get_input<double>(i++);
  double     ur_lat = pro.get_input<double>(i++);
  vcl_string out_txt = pro.get_input<vcl_string>(i++);
  // read the tree
  // since we are interested only for the structure, type of the tree doesn't matter here
  if( !vul_file::exists(geo_index_txt) )
    {
    vcl_cerr << pro.name() << ": can not find geo index file " << geo_index_txt << "!\n";
    return false;
    }
  double                                  min_size;
  bvgl_2d_geo_index_node_sptr             root = bvgl_2d_geo_index::read_and_construct<float>(geo_index_txt, min_size);
  vcl_vector<bvgl_2d_geo_index_node_sptr> leaves;
  bvgl_2d_geo_index::get_leaves(root, leaves);
  // get the ids of intersected leaves
  vcl_vector<unsigned> leaf_ids;
  vgl_box_2d<double>   bbox(ll_lon, ur_lon, ll_lat, ur_lat);
  vcl_cout << "bounding box: " << bbox << vcl_endl;
  for( unsigned i = 0; i < leaves.size(); i++ )
    {
    if( vgl_intersection(bbox, leaves[i]->extent_).area() > 0 )
      {
      leaf_ids.push_back(i);
      }
    }

  // write it to file
  vcl_ofstream ofs(out_txt.c_str() );
  if( !ofs.is_open() )
    {
    vcl_cerr << pro.name() << ": can not write into file " << out_txt << "!\n";
    return false;
    }
  ofs << leaf_ids.size() << " leaves intersect with region [" << ll_lon << ',' << ll_lat << "] , [" << ur_lon << ','
      << ur_lat << "]\n";
  for( unsigned i = 0; i < leaf_ids.size(); i++ )
    {
    ofs << leaf_ids[i] << '\n';
    }
  ofs.close();
  pro.set_output_val<unsigned>(0, leaf_ids.size() );
  return true;
}
