// This is brl/bseg/boxm2/cpp/pro/processes/boxm2_cpp_points_to_volume_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  Process to create a new volume from a point cloud
//
// \author Andrew Miller
// \date Jan 29, 2012

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/io/boxm2_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
//brdb stuff
#include <brdb/brdb_value.h>
#include <boxm2/cpp/algo/boxm2_points_to_volume_function.h>

//imesh/point cloud stuff
#include <bbas/imesh/imesh_mesh.h>
#include <bbas/imesh/imesh_fileio.h>

//directory utility
#include <vcl_where_root_dir.h>

namespace boxm2_cpp_points_to_volume_process_globals
{
  constexpr unsigned n_inputs_ = 3;
  constexpr unsigned n_outputs_ = 0;
}

bool boxm2_cpp_points_to_volume_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_cpp_points_to_volume_process_globals;

  //process inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr"; //input scene to be written to
  input_types_[1] = "boxm2_cache_sptr"; //cache used
  input_types_[2] = "vcl_string";       //path to ply file

  // process has 0 output:
  // output[0]: scene sptr
  std::vector<std::string>  output_types_(n_outputs_);
  bool good =pro.set_input_types(input_types_) && pro.set_output_types(output_types_);

  //default value
  return good;
}

bool boxm2_cpp_points_to_volume_process(bprb_func_process& pro)
{
  using namespace boxm2_cpp_points_to_volume_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_cache_sptr cache = pro.get_input<boxm2_cache_sptr>(i++);
  std::string point_file = pro.get_input<std::string>(i++);

  //load point file
  imesh_mesh points;
  bool loaded = imesh_read( point_file, points);
  if(!loaded) {
    std::cout<<"points to volume could not load file: "<<point_file<<std::endl;
    return false;
  }
  std::cout<<"point cloud loaded of size: "<<points.num_verts()<<std::endl;

  //fill out the volume and save
  boxm2_points_to_volume points2volume(scene, cache, points);
  points2volume.fillVolume();

  return true;
}
