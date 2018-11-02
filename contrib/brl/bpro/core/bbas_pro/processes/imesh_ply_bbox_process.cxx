// This is brl/bpro/core/bbas_pro/processes/imesh_ply_bbox_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for computing bbox from a ply file.
//
// \author Vishal Jain
// \date jan 26, 2011

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <imesh/imesh_fileio.h>
#include <vgl/vgl_box_3d.h>

namespace imesh_ply_bbox_process_globals
{
  constexpr unsigned n_inputs_ = 1;
  constexpr unsigned n_outputs_ = 6;
}

bool imesh_ply_bbox_process_cons(bprb_func_process& pro)
{
  using namespace imesh_ply_bbox_process_globals;
  //process takes 1 input, the scene
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string";

  // process has 6 outputs:
  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0] = "double";// minx
  output_types_[1] = "double";// miny
  output_types_[2] = "double";// minz
  output_types_[3] = "double";// maxx
  output_types_[4] = "double";// maxy
  output_types_[5] = "double";// maxz
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool imesh_ply_bbox_process(bprb_func_process& pro)
{
  using namespace imesh_ply_bbox_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  //get the inputs
  std::string   mesh_file = pro.get_input<std::string>(0);
  imesh_mesh mesh ;
  imesh_read(mesh_file, mesh);

  imesh_vertex_array_base  & verts = mesh.vertices();
  std::cout<<" No of vertices "<< verts.size();
  vgl_box_3d<double> bbox;
  for (unsigned i = 0 ; i < verts.size(); i++)
  {
    bbox.add(vgl_point_3d<double> ( verts(i,0),verts(i,1),verts(i,2) ) );
  }

  // store outputs
  int i=0;
  pro.set_output_val<double>(i++, bbox.min_x());
  pro.set_output_val<double>(i++, bbox.min_y());
  pro.set_output_val<double>(i++, bbox.min_z());
  pro.set_output_val<double>(i++, bbox.max_x());
  pro.set_output_val<double>(i++, bbox.max_y());
  pro.set_output_val<double>(i++, bbox.max_z());
  return true;
}
