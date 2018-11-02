//This is brl/bseg/boxm/pro/processes/boxm_upload_mesh_process.cxx
#include <string>
#include <iostream>
#include <sstream>
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

#include <bprb/bprb_parameters.h>

#include <boxm/boxm_apm_traits.h>

#include <boxm/boxm_scene_base.h>
#include <boxm/algo/boxm_upload_mesh.h>
#include <boxm/algo/boxm_fill_in_mesh.h>
#include <boxm/util/boxm_utils.h>
#include <boxm/boxm_scene.h>
#include <boct/boct_tree.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_file.h>
#include <vul/vul_string.h>

#include <imesh/imesh_mesh.h>
#include <imesh/imesh_fileio.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

namespace boxm_upload_mesh_process_globals
{
  constexpr unsigned n_inputs_ = 4;
  constexpr unsigned n_outputs_ = 0;
}


//: set input and output types
bool boxm_upload_mesh_process_cons(bprb_func_process& pro)
{
  using namespace boxm_upload_mesh_process_globals;

  // process takes 4 inputs and no output
  std::vector<std::string> input_types_(n_inputs_);
  unsigned i=0;
  input_types_[i++]="vcl_string";           //the directory for ply files
  input_types_[i++]="boxm_scene_base_sptr"; //scene to be uploaded
  input_types_[i++]="bool";                 //true, if mesh vertices are in geo coordinates
  input_types_[i++]="vcl_string";                 //true, if mesh vertices are in geo coordinates

  std::vector<std::string> output_types_(n_outputs_);
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//: Execute the process
bool boxm_upload_mesh_process(bprb_func_process& pro)
{
  using namespace boxm_upload_mesh_process_globals;
  // check number of inputs
  if (pro.input_types().size() != n_inputs_)
  {
    std::cout << pro.name() << "The number of inputs should be " << n_inputs_ << std::endl;
    return false;
  }

  unsigned i=0;
  std::string input_path = pro.get_input<std::string>(i++);
  boxm_scene_base_sptr scene = pro.get_input<boxm_scene_base_sptr>(i++);
  bool use_lvcs = pro.get_input<bool>(i++);
  std::string draw_or_fill = pro.get_input<std::string>(i++);

  if (!vul_file::is_directory(input_path)) {
    std::cerr << "In boxm_upload_mesh_process -- input path " << input_path<< "is not valid!\n";
    return false;
  }

  // get all the files in the directory
  std::stringstream glob;
  glob << input_path << "/*.ply*";

  std::vector<imesh_mesh> meshes;
  for (vul_file_iterator file_it = glob.str().c_str(); file_it; ++file_it)
  {
    std::string file(file_it());
    std::string file_format = vul_file::extension(file);
    vul_string_upcase(file_format);


    std::cout << "format = " << file_format << '\n'
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
      auto *s = static_cast<boxm_scene<tree_type>*> (scene.as_pointer());

      boxm_sample<BOXM_APM_MOG_GREY> val(0,boxm_utils::obtain_mog_grey_unit_mode());
      if (draw_or_fill=="draw")
        std::cout<<"Not yet";
        //boxm_upload_mesh_into_scene<short, boxm_sample<BOXM_APM_MOG_GREY> >(*s, mesh, use_lvcs, val);
      else if (draw_or_fill=="fill")
        boxm_fill_in_mesh_into_scene<short, boxm_sample<BOXM_APM_MOG_GREY> >(*s, meshes, use_lvcs, val);
    }
    else
      std::cout << "boxm_upload_mesh_process: multi bin is not implemented yet" << std::endl;
  }
  if (scene->appearence_model() == BOXM_APM_SIMPLE_GREY) {
    if (!scene->multi_bin())
    {
      typedef boct_tree<short, boxm_sample<BOXM_APM_SIMPLE_GREY> > tree_type;
      auto *s = static_cast<boxm_scene<tree_type>*> (scene.as_pointer());
      boxm_simple_grey simplegrey(1.0f,0.1f,1.0f);
      boxm_sample<BOXM_APM_SIMPLE_GREY> val(1,simplegrey);
      if (draw_or_fill=="draw")
        std::cout<<"Not yet";
        //boxm_upload_mesh_into_scene<short, boxm_sample<BOXM_APM_MOG_GREY> >(*s, mesh, use_lvcs, val);
      else if (draw_or_fill=="fill")
        boxm_fill_in_mesh_into_scene<short, boxm_sample<BOXM_APM_SIMPLE_GREY> >(*s, meshes, use_lvcs, val);
    }
    else
      std::cout << "boxm_upload_mesh_process: multi bin is not implemented yet" << std::endl;
  }
  else {
    std::cout << "boxm_upload_mesh_process: undefined APM type" << std::endl;
    return false;
  }

  return true;
}
