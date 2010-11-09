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

#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_convert.h>
#include <vil/vil_save.h>
// global variables
namespace icam_register_image_process_globals
{
  // this process takes 4 inputs and 1 output
  const unsigned n_inputs_ = 4;
  const unsigned n_outputs_ = 1;
}

template <class T>
bool load_image(vil_image_view_base_sptr const& base_img, vil_image_view<T>*& image)
{
  if (base_img->pixel_format() == VIL_PIXEL_FORMAT_FLOAT) {
    vil_image_view<float>* fimage = static_cast<vil_image_view<float>*> (base_img.ptr());
    image = new vil_image_view<T>(base_img->ni(), base_img->nj());
    vil_convert_cast<float,T>(*fimage, *image);
  }
  else if (base_img->pixel_format() == VIL_PIXEL_FORMAT_BYTE) {
    vil_image_view<vxl_byte>* byte_image = static_cast<vil_image_view<vxl_byte>*> (base_img.ptr());
    image = new vil_image_view<T>(base_img->ni(), base_img->nj());
    vil_convert_cast<vxl_byte,T>(*byte_image, *image);
  }
  else if (base_img->pixel_format() == VIL_PIXEL_FORMAT_DOUBLE) {
    vil_image_view<double>* img = static_cast<vil_image_view<double>*> (base_img.ptr());
    image = new vil_image_view<T>(base_img->ni(), base_img->nj());
    vil_convert_cast<double,T>(*img, *image);
  }
  else {
    vcl_cout << "icam_register_image_process -- image type " << base_img->pixel_format() << " is not supported!" << vcl_endl;
    return false;
  }
  return true;
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

  // set the images to view points
  vcl_map<unsigned, vil_image_view<float>* > images;
  vcl_map<unsigned, vil_image_view<double>* > depth_images;

  // read the image list from text files
  vcl_ifstream ifs1(exp_path.c_str());
  if (!ifs1.good()) {
    vcl_cout << "Error opening " << exp_path << vcl_endl;
    return false;
  }

  while (ifs1) {
    unsigned id;
    vcl_string path;
    ifs1 >> id >> path;
    vcl_cout << id << ' ' << path << vcl_endl;
    vil_image_view<float>* image;
    if (load_image<float>(path, image))
      images[id] = image;
  }
  ifs1.close();

  // load the depth images
  vcl_ifstream ifs2(depth_path.c_str());
  while (ifs2) {
    unsigned id;
    vcl_string path;
    ifs2 >> id >> path;
    vil_image_view<double>* image;
    if (load_image<double>(path, image))
      depth_images[id] = image;
  }
  ifs2.close();

  view_sphere->set_images(images, depth_images);

  vil_image_view<float>* dest_img;
  if (load_image<float>(dest_image, dest_img)) {
    view_sphere->register_image(*dest_img);

    vpgl_camera_double_sptr cam = new vpgl_perspective_camera<double>();
    pro.set_output_val<vpgl_camera_double_sptr>(0, cam);
    return true;
  }
  else
    return false;
}
