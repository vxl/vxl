//This is brl/bseg/bvxm/grid/pro/processes/bvxm_fill_mesh_grid_process.cxx

//:
// \file
// \brief  A process for populating the grid with meshes.
//         Meshes are in ply format and the grid will be filled with 0's at the faces of the mesh.
// \author Gamze D. Tunali
// \date   July 14, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_string.h>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <bvxm/grid/bvxm_voxel_grid_base.h>
#include <bvxm/grid/bvxm_voxel_grid.h>
#include <bvxm/grid/bvxm_voxel_grid_basic_ops.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_file.h>
#include <vul/vul_string.h>

#include <imesh/imesh_mesh.h>
#include <imesh/imesh_fileio.h>

namespace bvxm_fill_mesh_grid_process_globals
{
  const unsigned n_inputs_ = 6;
  const unsigned n_outputs_ = 1;
}


//: set input and output types
bool bvxm_fill_mesh_grid_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_fill_mesh_grid_process_globals;
  //This process has no inputs nor outputs only parameters
  vcl_vector<vcl_string> input_types_(n_inputs_);
  unsigned i=0;
  input_types_[i++]="vcl_string"; //the input path, the directory for ply files
  input_types_[i++]="vcl_string";
  input_types_[i++]="vcl_string";
  input_types_[i++]="unsigned";
  input_types_[i++]="unsigned";
  input_types_[i++]="unsigned";

  vcl_vector<vcl_string> output_types_(n_outputs_);
  i=0;
  output_types_[i++]="bvxm_voxel_grid_base_sptr";  // The resulting grid

  vcl_cout << input_types_.size();
  if (!pro.set_input_types(input_types_))
    return false;

  if (!pro.set_output_types(output_types_))
    return false;

  return true;
}


//: Execute the process
bool bvxm_fill_mesh_grid_process(bprb_func_process& pro)
{
  using namespace bvxm_fill_mesh_grid_process_globals;
  // check number of inputs
  if (pro.input_types().size() != n_inputs_)
  {
    vcl_cout << pro.name() << "The number of inputs should be " << n_inputs_ << vcl_endl;
    return false;
  }

  unsigned i=0;
  vcl_string input_path = pro.get_input<vcl_string>(i++);
  vcl_string vox_path = pro.get_input<vcl_string>(i++);

  if (!vul_file::is_directory(input_path)) {
    vcl_cerr << "In bvxm_fill_mesh_grid_process -- input path " << input_path<< "is not valid!\n";
    return false;
  }

  // read lvcs input
  bgeo_lvcs lvcs;
  vcl_string lvcs_path = pro.get_input<vcl_string>(i++);
  vcl_ifstream s(lvcs_path.data());
  lvcs.read(s);

  //get the world dimensions
  unsigned world_x = pro.get_input<unsigned>(i++);
  unsigned world_y = pro.get_input<unsigned>(i++);
  unsigned world_z = pro.get_input<unsigned>(i++);

  // get all the files in the directory
  vcl_stringstream glob;
  glob << input_path << "/*.ply*";

  //insert grids
  //bvxm_voxel_grid_base_sptr grid = new bvxm_voxel_grid<float>();
  bvxm_voxel_grid_base_sptr grid = new bvxm_voxel_grid<float>(vox_path, vgl_vector_3d<unsigned int>(world_x,world_y,world_z));
  bvxm_voxel_grid<float>* g = static_cast<bvxm_voxel_grid<float>*>(grid.as_pointer());
  g->initialize_data(vcl_numeric_limits<float>::max());
  for (vul_file_iterator file_it = glob.str().c_str(); file_it; ++file_it)
  {
    vcl_string file = file_it.filename();
    vcl_string file_format = vul_file::extension(file);
    vul_string_upcase(file_format);

    vcl_cout << "format = " << file_format << '\n'
             << "file = " << file << '\n';
    // call appropriate load functions to load the M
    imesh_mesh mesh;
    imesh_read(file, mesh);

	  float val=0.0f;
    bvxm_voxel_grid<float>* g = static_cast<bvxm_voxel_grid<float>*>(grid.as_pointer());
    bvxm_load_mesh_into_grid<float>(g, mesh, lvcs,val);
  }

  pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid);
  return true;
}
