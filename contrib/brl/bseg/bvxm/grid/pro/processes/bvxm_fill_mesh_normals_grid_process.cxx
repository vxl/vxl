//This is brl/bseg/bvxm/grid/pro/processes/bvxm_fill_mesh_normals_grid_process.cxx

//:
// \file
// \brief  A process for populating the grid with meshes.
//         Meshes are in ply format and the grid will be filled with normals at the faces of the mesh.
// \author Vishal Jain
// \date   Aug 11, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

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

#include <vcl_string.h>
#include <vcl_sstream.h>

namespace bvxm_fill_mesh_normals_grid_process_globals
{
  const unsigned n_inputs_ = 4;
  const unsigned n_outputs_ = 1;
}


//: set input and output types
bool bvxm_fill_mesh_normals_grid_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_fill_mesh_normals_grid_process_globals;

  // process takes 4 inputs and has 1 output.
  vcl_vector<vcl_string> input_types_(n_inputs_);
  unsigned i=0;
  input_types_[i++]="vcl_string"; //the input path, the directory for ply files
  input_types_[i++]="bvxm_voxel_grid_base_sptr"; //: input grid
  input_types_[i++]="vcl_string"; //: path for lvcs file
  input_types_[i++]="bool"; //: use lvcs file

  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0]="bvxm_voxel_grid_base_sptr";  // The resulting grid

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//: Execute the process
bool bvxm_fill_mesh_normals_grid_process(bprb_func_process& pro)
{
  using namespace bvxm_fill_mesh_normals_grid_process_globals;
  // check number of inputs
  if (pro.input_types().size() != n_inputs_)
  {
    vcl_cout << pro.name() << "The number of inputs should be " << n_inputs_ << vcl_endl;
    return false;
  }

  unsigned i=0;
  vcl_string input_path = pro.get_input<vcl_string>(i++);

  if (!vul_file::is_directory(input_path)) {
    vcl_cerr << "In bvxm_fill_mesh_normals_grid_process -- input path " << input_path<< "is not valid!\n";
    return false;
  }
  bvxm_voxel_grid_base_sptr grid = pro.get_input<bvxm_voxel_grid_base_sptr>(i++);

  // read lvcs input
  vpgl_lvcs lvcs;
  vcl_string lvcs_path = pro.get_input<vcl_string>(i++);
  vcl_ifstream s(lvcs_path.data());
  lvcs.read(s);
  bool use_lvcs=pro.get_input<bool>(i++);
  // get all the files in the directory
  vcl_stringstream glob;
  glob << input_path << "/*.ply*";

  //insert grids
  if (bvxm_voxel_grid<vnl_vector_fixed<float,3> >* g = dynamic_cast<bvxm_voxel_grid<vnl_vector_fixed<float,3> >*>(grid.as_pointer()))
  {
      g->initialize_data(vnl_vector_fixed<float,3>(0.0f,0.0f,0.0f));
      for (vul_file_iterator file_it = glob.str().c_str(); file_it; ++file_it)
      {
          vcl_string file = input_path+"/"+file_it.filename();
          vcl_string file_format = vul_file::extension(file);
          vul_string_upcase(file_format);

          vcl_cout << "format = " << file_format << '\n'
                   << "file = " << file << '\n';
          // call appropriate load functions to load the M
          imesh_mesh mesh;
          imesh_read(file, mesh);
          if (use_lvcs)
              covert_global_mesh_to_local(mesh,lvcs);
          mesh.compute_face_normals(true);

          bvxm_load_mesh_normals_into_grid(g, mesh);
      }

      pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, g);
      return true;
  }
  else
  {
      vcl_cout<<"Grid Type Mismatch"<<vcl_endl;
      return false;
  }
}
