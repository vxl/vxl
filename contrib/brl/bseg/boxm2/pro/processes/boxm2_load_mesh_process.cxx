// This is brl/bseg/boxm2/pro/processes/boxm2_load_mesh_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for loading a  mesh
//
// \author Vishal Jain
// \date Mar 15, 2013

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_file.h>
#include <vul/vul_timer.h>
#include <vnl/vnl_random.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_util.h>

//vil includes
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>
#include <vil/algo/vil_gauss_filter.h>

//vgl
#include <vgl/vgl_distance.h>
#include <vgl/vgl_triangle_scan_iterator.h>
#include <bvgl/bvgl_triangle_interpolation_iterator.h>

//vpgl camera stuff
#include <vpgl/vpgl_perspective_camera.h>

//det and imesh includes
#include <sdet/sdet_image_mesh.h>
#include <sdet/sdet_image_mesh_params.h>
#include <imesh/imesh_fileio.h>
#include <imesh/algo/imesh_render.h>

namespace boxm2_load_mesh_process_globals
{
  constexpr unsigned n_inputs_ = 1;
  constexpr unsigned n_outputs_ = 1;
}

bool boxm2_load_mesh_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_load_mesh_process_globals;

  //process takes 2 inputs
  int i=0;
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[i++] = "vcl_string";       //mesh filename

  // process has 1 output
  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0] = "imesh_mesh_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_load_mesh_process(bprb_func_process& pro)
{
  using namespace boxm2_load_mesh_process_globals;
  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  unsigned argIdx = 0;
  std::string mesh_filename = pro.get_input<std::string>(argIdx++);
  imesh_mesh_sptr mesh = new imesh_mesh();
  imesh_read(mesh_filename, *(mesh.ptr()));
  std::cout<<"# of faces "<<mesh->num_faces()<<std::endl;
  pro.set_output_val<imesh_mesh_sptr>(0,mesh);


  return true;
}
