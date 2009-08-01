//This is brl/bseg/boxm/pro/processes/boxm_upload_mesh_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for populating the octree with meshes.
//         Meshes are in ply format and the grid will be filled with 0's at the faces
//         of the meshes.
// \author Gamze D. Tunali
// \date   July 14, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_string.h>
#include <bprb/bprb_parameters.h>

#include <boxm/boxm_apm_traits.h>

#include <boxm/boxm_scene_base.h>
#include <boxm/boxm_upload_mesh.h>
#include <boxm/boxm_fill_in_mesh.h>
#include <boxm/boxm_scene.h>
#include <boct/boct_tree.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_file.h>
#include <vul/vul_string.h>

#include <imesh/imesh_mesh.h>
#include <imesh/imesh_fileio.h>

namespace boxm_upload_mesh_process_globals
{
  const unsigned n_inputs_ = 4;
  const unsigned n_outputs_ = 0;
}


//: set input and output types
bool boxm_upload_mesh_process_cons(bprb_func_process& pro)
{
  using namespace boxm_upload_mesh_process_globals;
  //This process has no inputs nor outputs only parameters
  vcl_vector<vcl_string> input_types_(n_inputs_);
  unsigned i=0;
  input_types_[i++]="vcl_string";           //the directory for ply files
  input_types_[i++]="boxm_scene_base_sptr"; //scene to be uploaded
  input_types_[i++]="bool";                 //true, if mesh vertices are in geo coordinates
  input_types_[i++]="vcl_string";                 //true, if mesh vertices are in geo coordinates
  vcl_vector<vcl_string> output_types_(n_outputs_);
  i=0;

  vcl_cout << input_types_.size();
  if (!pro.set_input_types(input_types_))
    return false;

  if (!pro.set_output_types(output_types_))
    return false;

  return true;
}


//: Execute the process
bool boxm_upload_mesh_process(bprb_func_process& pro)
{
  using namespace boxm_upload_mesh_process_globals;
  // check number of inputs
  if (pro.input_types().size() != n_inputs_)
  {
    vcl_cout << pro.name() << "The number of inputs should be " << n_inputs_ << vcl_endl;
    return false;
  }

  unsigned i=0;
  vcl_string input_path = pro.get_input<vcl_string>(i++);
  boxm_scene_base_sptr scene = pro.get_input<boxm_scene_base_sptr>(i++);
  bool use_lvcs = pro.get_input<bool>(i++);
  vcl_string draw_or_fill = pro.get_input<vcl_string>(i++);

  if (!vul_file::is_directory(input_path)) {
    vcl_cerr << "In boxm_upload_mesh_process -- input path " << input_path<< "is not valid!\n";
    return false;
  }

  // get all the files in the directory
  vcl_stringstream glob;
  glob << input_path << "/*.ply*";

  vcl_vector<imesh_mesh> meshes;
  for (vul_file_iterator file_it = glob.str().c_str(); file_it; ++file_it)
  {
    vcl_string file(file_it());
    vcl_string file_format = vul_file::extension(file);
    vul_string_upcase(file_format);


    vcl_cout << "format = " << file_format << '\n'
             << "file = " << file << '\n';
    // call appropriate load functions to load the M
    imesh_mesh mesh;
    if (file_format == ".PLY")
      imesh_read(file, mesh);
    else if (file_format == ".PLY2")
      imesh_read_ply2(file, mesh);

    meshes.push_back(mesh);
  }
  if (scene->appearence_model() == BOXM_APM_MOG_GREY) {
    if (!scene->multi_bin())
    {
      typedef boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > tree_type;
      boxm_scene<tree_type> *s = static_cast<boxm_scene<tree_type>*> (scene.as_pointer());

      boxm_sample<BOXM_APM_MOG_GREY> val(0,boxm_utils::obtain_mog_grey_unit_mode());
      if (draw_or_fill=="draw")
        vcl_cout<<"Not yet";
        //boxm_upload_mesh_into_scene<short, boxm_sample<BOXM_APM_MOG_GREY> >(*s, mesh, use_lvcs, val);
      else if (draw_or_fill=="fill")
        boxm_fill_in_mesh_into_scene<short, boxm_sample<BOXM_APM_MOG_GREY> >(*s, meshes, use_lvcs, val);
    }
    else
      vcl_cout << "boxm_upload_mesh_process: multi bin is not implemented yet" << vcl_endl;
  }
  else {
    vcl_cout << "boxm_upload_mesh_process: undefined APM type" << vcl_endl;
    return false;
  }

  return true;
}
