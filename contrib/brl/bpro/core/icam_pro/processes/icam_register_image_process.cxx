// This is brl/bpro/core/icam_pro/processes/icam_register_image_process.cxx

//:
// \file
// \brief A process for generating cameras that view a scene
//
// \author Gamze D. Tunali
// \date January 23, 2010
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <vpgl/vpgl_perspective_camera.h>
#include <vcl_string.h>
#include <vcl_iostream.h>

#include <icam/icam_view_sphere.h>
#include <icam/icam_view_metadata.h>
#include <icam/icam_minimizer_params.h>

#include <vil/vil_image_view.h>
#include <vil/vil_load.h>

// global variables
namespace icam_register_image_process_globals
{
  // this process takes 4 inputs and 1 output
  const unsigned n_inputs_ = 4;
  const unsigned n_outputs_ = 1;
}

template <class T>
bool load_image(vcl_string const& path, vil_image_view<T>*& image)
{
  vcl_cout << path.c_str() << vcl_endl;
  vil_image_view_base_sptr base_img = vil_load(path.c_str(),true);
  if (!base_img)
    return false;
  else
    return load_image(base_img, image);
}

//: sets input and output types
bool icam_register_image_process_cons(bprb_func_process& pro)
{
  using namespace icam_register_image_process_globals;
  unsigned i=0;
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[i++] = "vcl_string";   // the file path of the expected images list
  input_types_[i++] = "vcl_string";   // the file path of the depth images list
  input_types_[i++] = "icam_view_sphere_sptr";
  input_types_[i++] = "vil_image_view_base_sptr";      // the destination image

  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0] = "vpgl_camera_double_sptr"; // the text file that contains the paths for the view point cameras

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

//: creates a scene from parameters
bool icam_register_image_process(bprb_func_process& pro)
{
  if (!pro.verify_inputs()) {
    vcl_cout << pro.name() << "icam_register_image_process: invalid inputs" << vcl_endl;
    return false;
  }
  using namespace icam_register_image_process_globals;

  int i=0;
  vcl_string exp_path = pro.get_input<vcl_string>(i++);
  vcl_string depth_path = pro.get_input<vcl_string>(i++);
  icam_view_sphere_sptr view_sphere= pro.get_input<icam_view_sphere_sptr>(i++);
  vil_image_view_base_sptr dest_image = pro.get_input<vil_image_view_base_sptr>(i++);

  // get the parameters
  unsigned nbins=0, min_pyramid_image_size=0, box_reduction_k=0;
  double local_min_thresh=0.0, smooth_sigma=0.0, axis_search_cone_multiplier=0.0, polar_range_multiplier=0.0;
  vcl_string base_path="";
  // with dummy initialisations to avoid compiler warnings
  if (!pro.parameters()->get_value("nbins", nbins)) return false;
  if (!pro.parameters()->get_value("min_pyramid_image_size", min_pyramid_image_size)) return false;
  if (!pro.parameters()->get_value("box_reduction_k", box_reduction_k)) return false;
  if (!pro.parameters()->get_value("local_min_thresh", local_min_thresh)) return false;
  if (!pro.parameters()->get_value("smooth_sigma", smooth_sigma)) return false;
  if (!pro.parameters()->get_value("axis_search_cone_multiplier", axis_search_cone_multiplier)) return false;
  if (!pro.parameters()->get_value("polar_range_multiplier", polar_range_multiplier)) return false;
  if (!pro.parameters()->get_value("base_path", base_path)) return false;

  // create the parameters object
  icam_minimizer_params params(nbins,min_pyramid_image_size,box_reduction_k,axis_search_cone_multiplier,
                               polar_range_multiplier,local_min_thresh,smooth_sigma,base_path);

  // set the images to view points
  vcl_map<unsigned, vcl_string> images;
  vcl_map<unsigned, vcl_string> depth_images;

  // read the image list from text files
  vcl_ifstream ifs1(exp_path.c_str());
  if (!ifs1.good()) {
    vcl_cout << "Error opening " << exp_path << vcl_endl;
    return false;
  }

  // load the view point image paths
  while (ifs1) {
    unsigned id;
    vcl_string path;
    ifs1 >> id >> path;
    images[id]=path;
  }
  ifs1.close();

  // load the depth image path
  vcl_ifstream ifs2(depth_path.c_str());
  while (ifs2) {
    unsigned id;
    vcl_string path;
    ifs2 >> id >> path;
    depth_images[id]=path;
  }
  ifs2.close();

  view_sphere->set_images(images, depth_images);

  vil_image_view<float> *dest_img;
  if (load_image<float>(dest_image, dest_img)) {
    view_sphere->register_image(*dest_img,params);

    vpgl_camera_double_sptr cam = new vpgl_perspective_camera<double>();
    pro.set_output_val<vpgl_camera_double_sptr>(0, cam);
    return true;
  }
  else
    return false;
}
