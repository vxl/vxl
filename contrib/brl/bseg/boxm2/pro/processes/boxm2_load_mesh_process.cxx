// This is brl/bseg/boxm2/pro/processes/boxm2_load_mesh_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for loading a  mesh
//
// \author Vishal Jain
// \date Mar 15, 2013

#include <vcl_fstream.h>
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
  const unsigned n_inputs_  = 1;
  const unsigned n_outputs_ = 1;
}

bool boxm2_load_mesh_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_load_mesh_process_globals;

  //process takes 2 inputs
  int i=0;
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[i++] = "vcl_string";       //mesh filename

  // process has 1 output
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  output_types_[0] = "imesh_mesh_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_load_mesh_process(bprb_func_process& pro)
{
  using namespace boxm2_load_mesh_process_globals;
  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  unsigned argIdx = 0;
  vcl_string mesh_filename   = pro.get_input<vcl_string>(argIdx++);
  imesh_mesh_sptr mesh = new imesh_mesh();
  imesh_read(mesh_filename, *(mesh.ptr()));
  vcl_cout<<"# of faces "<<mesh->num_faces()<<vcl_endl;
  pro.set_output_val<imesh_mesh_sptr>(0,mesh);
  

  return true;
}
