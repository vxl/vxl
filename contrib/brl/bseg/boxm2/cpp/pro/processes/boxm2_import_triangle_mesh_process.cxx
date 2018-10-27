// This is brl/bseg/boxm2/cpp/pro/processes/boxm2_import_triangle_mesh_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for exporting a texture mapped mesh of a scene
//
// \author Daniel Crispell
// \date 11 Jun 2015

#include <boxm2/boxm2_scene.h>
#include <boxm2/io/boxm2_cache.h>
#include <imesh/imesh_mesh.h>
#include <imesh/imesh_fileio.h>

#include <boxm2/cpp/algo/boxm2_import_triangle_mesh.h>

namespace boxm2_import_triangle_mesh_process_globals
{
  constexpr unsigned n_inputs_ = 4;
  constexpr unsigned n_outputs_ = 0;
}

bool boxm2_import_triangle_mesh_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_import_triangle_mesh_process_globals;

  //process takes 3 inputs
  int i=0;
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[i++] = "boxm2_scene_sptr";  // scene
  input_types_[i++] = "boxm2_cache_sptr";
  input_types_[i++] = "vcl_string";        // input mesh filename
  input_types_[i++] = "float";  // prob. of cells that mesh intersects

  // process has 0 output
  std::vector<std::string>  output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_import_triangle_mesh_process(bprb_func_process& pro)
{
  using namespace boxm2_import_triangle_mesh_process_globals;
  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The number of inputs should be " << n_inputs_<< std::endl;
    return false;
  }

  //get the inputs
  boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(0);
  boxm2_cache_sptr cache = pro.get_input<boxm2_cache_sptr>(1);
  std::string mesh_filename = pro.get_input<std::string>(2);
  auto occupied_prob = pro.get_input<float>(3);

#if 0
  bmsh3d_mesh  mesh;
  bmsh3d_load_ply2(&mesh, ply_file.c_str());
#else
  imesh_mesh mesh;
  imesh_read(mesh_filename, mesh);
#endif

  bool status = boxm2_import_triangle_mesh(scene, cache, mesh, occupied_prob);
  if (!status) {
    std::cerr << "ERROR: boxm2_import_triangle_mesh_process: import_triangle_mesh retured false!" << std::endl;
  }
  return status;
}
