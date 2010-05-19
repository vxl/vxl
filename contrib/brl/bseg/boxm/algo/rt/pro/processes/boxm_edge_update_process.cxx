//This is brl/bseg/boxm/algo/rt/pro/processes/boxm_edge_update_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for updating a boxm_scene with a set of edge images
// \author Gamze Tunali
// \date   Jan, 2010
//
// \verbatim
//  Modifications
//
// \endverbatim

#include <vcl_string.h>
#include <vcl_fstream.h>

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <boct/boct_tree.h>

#include <boxm/boxm_scene_base.h>
#include <boxm/boxm_scene.h>

#include <boxm/sample/boxm_edge_sample.h>
#include <boxm/sample/boxm_inf_line_sample.h>
#include <boxm/algo/rt/boxm_edge_tangent_updater.h>
#include <boxm/algo/rt/boxm_edge_updater.h>

namespace boxm_edge_update_process_globals
{
  const unsigned int n_inputs_ = 3;
  const unsigned int n_outputs_ = 0;
  //Define parameters here
}


//: set input and output types
bool boxm_edge_update_process_cons(bprb_func_process& pro)
{
  using namespace boxm_edge_update_process_globals;

  // process takes 3 inputs:
  //input[0]: The scene
  //input[1]: Image list filename
  //input[2]: Use ransac?
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm_scene_base_sptr";
  input_types_[1] = "vcl_string";
  input_types_[2] = "bool";
  // process has 0 outputs:
  vcl_vector<vcl_string> output_types_(n_outputs_);

  if (!pro.set_input_types(input_types_))
    return false;

  if (!pro.set_output_types(output_types_))
    return false;

  return true;
}

//: Execute the process
bool boxm_edge_update_process(bprb_func_process& pro)
{
  using namespace boxm_edge_update_process_globals;

  // check number of inputs
  if (pro.n_inputs() != n_inputs_)
  {
    vcl_cout << pro.name() << "The number of inputs should be " << n_inputs_ << vcl_endl;
    return false;
  }

  // get the parameters
  // (none)

  // get the inputs
  boxm_scene_base_sptr scene_base = pro.get_input<boxm_scene_base_sptr>(0);
  vcl_string image_list_fname = pro.get_input<vcl_string>(1);
  bool use_ransac=pro.get_input<bool>(2);

  // extract list of image_ids from file
  vcl_ifstream ifs(image_list_fname.c_str());
  if (!ifs.good()) {
    vcl_cerr << "error opening file " << image_list_fname << vcl_endl;
    return false;
  }
  vcl_vector<vcl_string> image_ids;
  unsigned int n_images = 0;
  ifs >> n_images;
  for (unsigned int i=0; i<n_images; ++i) {
    vcl_string img_id;
    ifs >> img_id;
    image_ids.push_back(img_id);
  }
  ifs.close();

  if (scene_base->appearence_model() == BOXM_EDGE_FLOAT) {
    typedef boct_tree<short,boxm_edge_sample<float> > tree_type;
    boxm_scene<tree_type> *scene = dynamic_cast<boxm_scene<tree_type>*>(scene_base.ptr());
    if (!scene) {
       vcl_cerr << "error casting scene_base to scene\n";
        return false;
    }
    boxm_edge_updater<short,float,float> updater(*scene, image_ids);
    updater.add_cells();
  }
  if (scene_base->appearence_model() == BOXM_EDGE_LINE) {
    typedef boct_tree<short,boxm_inf_line_sample<float> > tree_type;
    boxm_scene<tree_type> *scene = dynamic_cast<boxm_scene<tree_type>*>(scene_base.ptr());
    if (!scene) {
       vcl_cerr << "error casting scene_base to scene\n";
        return false;
    }

    boxm_edge_tangent_updater<short,float,float> updater(*scene, image_ids, use_ransac);
    updater.add_cells();
  }
  //store output
  // (none)

  return true;
}
