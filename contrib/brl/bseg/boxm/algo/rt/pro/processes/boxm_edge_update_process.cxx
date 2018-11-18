//This is brl/bseg/boxm/algo/rt/pro/processes/boxm_edge_update_process.cxx
#include <string>
#include <iostream>
#include <fstream>
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

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

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
  constexpr unsigned int n_inputs_ = 6;
  constexpr unsigned int n_outputs_ = 0;
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
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm_scene_base_sptr";
  input_types_[1] = "vcl_string";
  input_types_[2] = "bool";
  input_types_[3] = "int";
  input_types_[4] = "float";
  input_types_[5] = "float";
  // process has 0 outputs:
  std::vector<std::string> output_types_(n_outputs_);

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
    std::cout << pro.name() << "The number of inputs should be " << n_inputs_ << std::endl;
    return false;
  }

  // get the parameters
  // (none)

  // get the inputs
  boxm_scene_base_sptr scene_base = pro.get_input<boxm_scene_base_sptr>(0);
  std::string image_list_fname = pro.get_input<std::string>(1);
  bool use_ransac=pro.get_input<bool>(2);
  int consensus_cnt = pro.get_input<int>(3);
  auto ortho_thres = pro.get_input<float>(4);
  auto vol_ratio = pro.get_input<float>(5);

  // extract list of image_ids from file
  std::ifstream ifs(image_list_fname.c_str());
  if (!ifs.good()) {
    std::cerr << "error opening file " << image_list_fname << '\n';
    return false;
  }
  std::vector<std::string> image_ids;
  unsigned int n_images = 0;
  ifs >> n_images;
  std::cout << "using image ids: ";
  for (unsigned int i=0; i<n_images; ++i) {
    std::string img_id;
    ifs >> img_id;
    std::cout << img_id << std::endl;
    image_ids.push_back(img_id);
    std::cout << img_id << ' ';
  }
  ifs.close();
  std::cout << std::endl;

  if (scene_base->appearence_model() == BOXM_EDGE_FLOAT) {
    std::cout << "appearance model EDGE_FLOAT\n";
    typedef boct_tree<short,boxm_edge_sample<float> > tree_type;
    auto *scene = dynamic_cast<boxm_scene<tree_type>*>(scene_base.ptr());
    if (!scene) {
       std::cerr << "error casting scene_base to scene\n";
        return false;
    }
    boxm_edge_updater<short,float,float> updater(*scene, image_ids);
    updater.add_cells();
  }
  if (scene_base->appearence_model() == BOXM_EDGE_LINE) {
    std::cout << "appearance model EDGE_LINE\n";
    typedef boct_tree<short,boxm_inf_line_sample<float> > tree_type;
    auto *scene = dynamic_cast<boxm_scene<tree_type>*>(scene_base.ptr());
    if (!scene) {
       std::cerr << "error casting scene_base to scene\n";
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
  std::vector<std::string> input_types_(4);
  input_types_[0] = "boxm_scene_base_sptr";
  input_types_[1] = "vcl_string";
  input_types_[2] = "vcl_string";
  input_types_[3] = "int";
  // process has 0 outputs:
  std::vector<std::string> output_types_(0);

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
    std::cout << pro.name() << "The number of inputs should be " << 4 << std::endl;
    return false;
  }

  // get the parameters
  // (none)

  // get the inputs
  boxm_scene_base_sptr scene_base = pro.get_input<boxm_scene_base_sptr>(0);
  std::string image_list_fname = pro.get_input<std::string>(1);
  std::string cam_list_fname = pro.get_input<std::string>(2);
  int consensus_cnt = pro.get_input<int>(3);

  // extract list of image_ids from file
  std::ifstream ifs(image_list_fname.c_str());
  if (!ifs.good()) {
    std::cerr << "error opening file " << image_list_fname << '\n';
    return false;
  }
  std::vector<vil_image_view<float> > images;
  unsigned int n_images = 0;
  ifs >> n_images;
  std::cout << "using image ids: ";
  for (unsigned int i=0; i<n_images; ++i) {
    std::string img_id;
    ifs >> img_id;
    std::cout << "loading: " << img_id << std::endl;

    vil_image_view_base_sptr loaded_image = vil_load(img_id.c_str() );
    if ( !loaded_image || loaded_image->pixel_format() != VIL_PIXEL_FORMAT_FLOAT) {
      std::cerr << "Failed to load image file: " << img_id << " or format is not FLOAT img!\n";
      return false;
    }
    vil_image_view<float> img(loaded_image);
    images.push_back(img);
  }
  ifs.close();

  // extract list of image_ids from file
  std::ifstream ifsc(cam_list_fname.c_str());
  if (!ifsc.good()) {
    std::cerr << "error opening file " << cam_list_fname << '\n';
    return false;
  }

  std::vector<vpgl_camera_double_sptr> cameras;
  unsigned int n_cameras = 0;
  ifsc >> n_cameras;
  if (n_cameras != n_images) {
    std::cerr << "camera file and image file do not contain equal numbers of items!\n";
    return false;
  }

  std::cout << "using cameras: ";
  for (unsigned int i=0; i<n_cameras; ++i) {
    std::string img_id;
    ifsc >> img_id;
    std::cout << "loading camera: " << img_id << std::endl;

    vpgl_camera_double_sptr ratcam = read_local_rational_camera<double>(img_id);
    if ( !ratcam.as_pointer() ) {
      std::cerr << "Rational camera isn't local... trying global\n";
      ratcam = read_rational_camera<double>(img_id);
      if ( !ratcam.as_pointer() ) {
        std::cout << " camera: " << img_id << " is not local or global rational camera, not supporting any other type for now!\n";
        return false;
      }
    }

    cameras.push_back(ratcam);
  }
  ifsc.close();
  std::cout << std::endl;

  if (scene_base->appearence_model() == BOXM_EDGE_FLOAT) {
    std::cout << "appearance model EDGE_FLOAT\n";
    std::cerr << "this process is not defined for this type of scene!\n";
    return false;
  }
  if (scene_base->appearence_model() == BOXM_EDGE_LINE) {
    std::cout << "appearance model EDGE_LINE\n";
    typedef boct_tree<short,boxm_inf_line_sample<float> > tree_type;
    auto *scene = dynamic_cast<boxm_scene<tree_type>*>(scene_base.ptr());
    if (!scene) {
       std::cerr << "error casting scene_base to scene\n";
        return false;
    }

    boxm_edge_tangent_refine_updates<short,float,float> refiner(*scene, consensus_cnt, images, cameras);
    refiner.refine_cells();
  }

  return true;
}
