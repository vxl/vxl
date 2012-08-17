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

#include <vil/vil_load.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>

namespace boxm_edge_update_process_globals
{
  const unsigned int n_inputs_ = 6;
  const unsigned int n_outputs_ = 0;
  //Define parameters here
}


//: set input and output types
bool boxm_edge_update_process_cons(bprb_func_process& pro)
{
  using namespace boxm_edge_update_process_globals;

  // process takes 6 inputs:
  //input[0]: The scene
  //input[1]: Image list filename
  //input[2]: Use ransac?
  //input[3]: consensus cnt, param of ransac algo, value depends on number of training images
  //          unless planes from this number of images agree at a voxel on a hypothesis, a 3D edge won't be formed,
  //          there should be at least this number of training images during the update for any kind of consensus to be met at any voxel
  //          (if consensus cnt < training img cnt --> edge world will be empty)
  //input[4]: orthogonality threshold, a good value 0.01, param of ransac algo, would see more 3D edges as this value gets bigger, but they would be less and less good
  //input[5]: volume ratio denominator: a good value 128 , param of ransac algo, would see more 3D edges as this value gets bigger, but they would be less and less good
  //          algo checks whether a current plane is within cell_volume/denominator radius of current hypothesis
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm_scene_base_sptr";
  input_types_[1] = "vcl_string";
  input_types_[2] = "bool";
  input_types_[3] = "int";
  input_types_[4] = "float";
  input_types_[5] = "float";
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
  int consensus_cnt = pro.get_input<int>(3);
  float ortho_thres = pro.get_input<float>(4);
  float vol_ratio = pro.get_input<float>(5);

  // extract list of image_ids from file
  vcl_ifstream ifs(image_list_fname.c_str());
  if (!ifs.good()) {
    vcl_cerr << "error opening file " << image_list_fname << '\n';
    return false;
  }
  vcl_vector<vcl_string> image_ids;
  unsigned int n_images = 0;
  ifs >> n_images;
  vcl_cout << "using image ids: ";
  for (unsigned int i=0; i<n_images; ++i) {
    vcl_string img_id;
    ifs >> img_id;
    vcl_cout << img_id << vcl_endl;
    image_ids.push_back(img_id);
    vcl_cout << img_id << ' ';
  }
  ifs.close();
  vcl_cout << vcl_endl;

  if (scene_base->appearence_model() == BOXM_EDGE_FLOAT) {
    vcl_cout << "appearance model EDGE_FLOAT\n";
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
    vcl_cout << "appearance model EDGE_LINE\n";
    typedef boct_tree<short,boxm_inf_line_sample<float> > tree_type;
    boxm_scene<tree_type> *scene = dynamic_cast<boxm_scene<tree_type>*>(scene_base.ptr());
    if (!scene) {
       vcl_cerr << "error casting scene_base to scene\n";
        return false;
    }

    boxm_edge_tangent_updater<short,float,float> updater(*scene, image_ids, use_ransac, ortho_thres, vol_ratio, consensus_cnt);
    updater.add_cells();
  }
  //store output
  // (none)

  return true;
}

//: set input and output types
bool boxm_edge_refine_updates_process_cons(bprb_func_process& pro)
{
  // process takes 3 inputs:
  //input[0]: The scene
  //input[1]: Image list filename
  //input[2]: camera list filename
  //input[3]: consensus cnt,
  vcl_vector<vcl_string> input_types_(4);
  input_types_[0] = "boxm_scene_base_sptr";
  input_types_[1] = "vcl_string";
  input_types_[2] = "vcl_string";
  input_types_[3] = "int";
  // process has 0 outputs:
  vcl_vector<vcl_string> output_types_(0);

  if (!pro.set_input_types(input_types_))
    return false;

  if (!pro.set_output_types(output_types_))
    return false;

  return true;
}

//: Execute the process
bool boxm_edge_refine_updates_process(bprb_func_process& pro)
{
  using namespace boxm_edge_update_process_globals;

  // check number of inputs
  if (pro.n_inputs() != 4)
  {
    vcl_cout << pro.name() << "The number of inputs should be " << 4 << vcl_endl;
    return false;
  }

  // get the parameters
  // (none)

  // get the inputs
  boxm_scene_base_sptr scene_base = pro.get_input<boxm_scene_base_sptr>(0);
  vcl_string image_list_fname = pro.get_input<vcl_string>(1);
  vcl_string cam_list_fname = pro.get_input<vcl_string>(2);
  int consensus_cnt = pro.get_input<int>(3);

  // extract list of image_ids from file
  vcl_ifstream ifs(image_list_fname.c_str());
  if (!ifs.good()) {
    vcl_cerr << "error opening file " << image_list_fname << '\n';
    return false;
  }
  vcl_vector<vil_image_view<float> > images;
  unsigned int n_images = 0;
  ifs >> n_images;
  vcl_cout << "using image ids: ";
  for (unsigned int i=0; i<n_images; ++i) {
    vcl_string img_id;
    ifs >> img_id;
    vcl_cout << "loading: " << img_id << vcl_endl;

    vil_image_view_base_sptr loaded_image = vil_load(img_id.c_str() );
    if ( !loaded_image || loaded_image->pixel_format() != VIL_PIXEL_FORMAT_FLOAT) {
      vcl_cerr << "Failed to load image file" << img_id << " or format is not FLOAT img!\n";
      return false;
    }
    vil_image_view<float> img(loaded_image);
    images.push_back(img);
  }
  ifs.close();

  // extract list of image_ids from file
  vcl_ifstream ifsc(cam_list_fname.c_str());
  if (!ifsc.good()) {
    vcl_cerr << "error opening file " << cam_list_fname << '\n';
    return false;
  }

  vcl_vector<vpgl_camera_double_sptr> cameras;
  unsigned int n_cameras = 0;
  ifsc >> n_cameras;
  if (n_cameras != n_images) {
    vcl_cerr << "camera file and image file do not contain equal numbers of items!\n";
    return false;
  }

  vcl_cout << "using cameras: ";
  for (unsigned int i=0; i<n_cameras; ++i) {
    vcl_string img_id;
    ifsc >> img_id;
    vcl_cout << "loading camera: " << img_id << vcl_endl;

    vpgl_camera_double_sptr ratcam = read_local_rational_camera<double>(img_id);
    if ( !ratcam.as_pointer() ) {
      vcl_cerr << "Rational camera isn't local... trying global\n";
      ratcam = read_rational_camera<double>(img_id);
      if ( !ratcam.as_pointer() ) {
        vcl_cout << " camera: " << img_id << " is not local or global rational camera, not supporting any other type for now!\n";
        return false;
      }
    }

    cameras.push_back(ratcam);
  }
  ifsc.close();
  vcl_cout << vcl_endl;

  if (scene_base->appearence_model() == BOXM_EDGE_FLOAT) {
    vcl_cout << "appearance model EDGE_FLOAT\n";
    vcl_cerr << "this process is not defined for this type of scene!\n";
    return false;
  }
  if (scene_base->appearence_model() == BOXM_EDGE_LINE) {
    vcl_cout << "appearance model EDGE_LINE\n";
    typedef boct_tree<short,boxm_inf_line_sample<float> > tree_type;
    boxm_scene<tree_type> *scene = dynamic_cast<boxm_scene<tree_type>*>(scene_base.ptr());
    if (!scene) {
       vcl_cerr << "error casting scene_base to scene\n";
        return false;
    }

    boxm_edge_tangent_refine_updates<short,float,float> refiner(*scene, consensus_cnt, images, cameras);
    refiner.refine_cells();
  }

  return true;
}

