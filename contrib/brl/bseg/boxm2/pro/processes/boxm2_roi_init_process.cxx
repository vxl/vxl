// This is brl/bseg/boxm2/pro/processes/boxm2_roi_init_process.cxx
#include <iostream>
#include <string>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
//:
// \file
// \brief A class for clipping an image based on a 3D bounding box.
//
//   -  Inputs:
//        * Image path (string)
//        * boxm2_scene_sptr
//   -  Outputs:
//        * modified rational camera "vpgl_camera_double_sptr" - local rational camera
//        * clipped image area (NITF) "vil_image_view_base_sptr"
//   -  Params:
//        * geographic uncertainty (error) in meters
//
// \author  Gamze D. Tunali
// \date    Feb 19, 2008
// \verbatim
//  Modifications
//   Ozge C. Ozcanli  - May 19, 2011 - moved to boxm2_pro and adapted to boxm2 scene.
// \endverbatim


#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/file_formats/vil_nitf2_image.h>
#include <vil/vil_image_view_base.h>

#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>

#include <boxm2/boxm2_scene.h>

#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_point_3d.h>

#include <vsol/vsol_box_2d_sptr.h>
#include <vsol/vsol_box_2d.h>

#include <brip/brip_roi.h>

#include <vpgl/vpgl_lvcs_sptr.h>

#include <vpgl/file_formats/vpgl_nitf_rational_camera.h>

//: globals variables and functions
namespace boxm2_roi_init_process_globals
{
  constexpr unsigned n_inputs_ = 7;
  constexpr unsigned n_outputs_ = 3;

  // === functions ===

  //: roi_init function
  bool roi_init(std::string const& image_path,
                vpgl_rational_camera<double>* camera,
                const boxm2_scene_sptr& scene,
                float uncertainty,
                //vil_image_view<unsigned char>* nitf_image_unsigned_char,
                vil_image_view_base_sptr& img_ptr,
                vpgl_local_rational_camera<double>& local_camera,
                bool convert_to_8_bit, int margin, int clip_box_width = -1.0,int clip_box_height = -1 );

  //: projects the box on the image by taking the union of all the projected corners
  bool project_box(const vpgl_rational_camera<double>& cam,
                   const vpgl_lvcs_sptr& lvcs,
                   const vgl_box_3d<double>& box,
                   float r,
                   vgl_box_2d<double>& roi);

  std::vector<vgl_point_3d<double> > corners_of_box_3d(vgl_box_3d<double> box);
}

//: roi_init function
bool boxm2_roi_init_process_globals::roi_init( std::string const& image_path,
                                               vpgl_rational_camera<double>* camera,
                                               const boxm2_scene_sptr& scene,
                                               float error,
                                               //vil_image_view<unsigned char>* nitf_image_unsigned_char,
                                               vil_image_view_base_sptr& img_ptr,
                                               vpgl_local_rational_camera<double>& local_camera,
                                               bool convert_to_8_bit,
                                               int margin,int clip_box_width, int clip_box_height)
{
  //: read the image and extract the camera
  vil_image_resource_sptr img = vil_load_image_resource(image_path.c_str());
  if(!img) {
    std::cerr << "boxm2_roi_init_process::execute - The NITF could not be loaded\n";
    return false;
  }
  std::string format = img->file_format();
  std::string prefix = format.substr(0,4);
  if (prefix.compare("nitf") != 0) {
    std::cerr << "boxm2_roi_init_process::execute - The image should be an NITF\n";
    return false;
  }

  auto* nitf =  static_cast<vil_nitf2_image*> (img.ptr());

  vgl_box_3d<double> box = scene->bounding_box();

  vpgl_lvcs_sptr lvcs = new vpgl_lvcs(scene->lvcs());
  vgl_box_2d<double> roi_box;
  project_box(*camera, lvcs, box, error, roi_box);

  brip_roi broi(nitf->ni(), nitf->nj());

  vsol_box_2d_sptr bb2 = new vsol_box_2d();
  bb2->add_point(roi_box.min_x(), roi_box.min_y());
  bb2->add_point(roi_box.max_x(), roi_box.max_y());
  bb2 = broi.clip_to_image_bounds(bb2);
  if (bb2->width() <= 0 || bb2->height() <= 0) {
    std::cerr << "boxm2_roi_init_process::execute - cannot clip roi to image boundaries\n";
    return false;
  }

  roi_box.set_width(roi_box.width()+2*margin);
  roi_box.set_height(roi_box.height()+2*margin);

  vsol_box_2d_sptr bb = new vsol_box_2d();
  bb->add_point(roi_box.min_x(), roi_box.min_y());
  bb->add_point(roi_box.max_x(), roi_box.max_y());
  bb = broi.clip_to_image_bounds(bb);
  if (bb->width() <= 0 || bb->height() <= 0) {
    std::cerr << "boxm2_roi_init_process::execute - cannot clip padded roi to image boundaries\n";
    return false;
  }

  //: use the margin
  double maxwidth = bb->width();
  double maxheight = bb->height();

  if(clip_box_width > 0)
    maxwidth = clip_box_width;
  if(clip_box_height > 0)
    maxheight = clip_box_height;

  vil_image_view_base_sptr roi =
    nitf->get_copy_view((unsigned int)bb->get_min_x(),
                        (unsigned int)maxwidth,
                        (unsigned int)bb->get_min_y(),
                        (unsigned int)maxheight);
  if (!roi) {
    std::cerr << "boxm2_roi_init_process::roi_init() - cannot load pixel data "
             << "(min_x:" << (unsigned int)bb->get_min_x() << " width:" << (unsigned int)maxwidth
             << " min_y:" << (unsigned int)bb->get_min_y() << " height:" << (unsigned int)maxheight
             << ")" << std::endl;
    return false;
  }

  if (!roi.as_pointer())
    return false;

  if (!convert_to_8_bit) {
    //std::cout << "ROI CROP: DO NOT CONVERT TO 8 BIT IMAGE\n";
    if (roi->pixel_format() == VIL_PIXEL_FORMAT_UINT_16)
      img_ptr = new vil_image_view<vxl_uint_16>(roi);
    else if (roi->pixel_format() == VIL_PIXEL_FORMAT_BYTE)
      img_ptr = new vil_image_view<unsigned char>(roi);
    else {
      std::cout << "boxm2_roi_init_process - Unsupported Pixel Format = " << roi->pixel_format() << std::endl;
      return false;
    }
  }
  else if (roi->pixel_format() == VIL_PIXEL_FORMAT_UINT_16)
  {
    vil_image_view<vxl_uint_16> nitf_image_vxl_uint_16(roi);

    //*nitf_image_unsigned_char = vil_image_view<unsigned char> (roi->ni(),roi->nj(),roi->nplanes());
    auto* nitf_image_unsigned_char = new vil_image_view<unsigned char> (roi->ni(),roi->nj(),roi->nplanes());


    int bigendian = 0;
    { union { unsigned int i; char c[4]; } u; u.i = 1; bigendian = u.c[0] == 0; }
    for (unsigned m=0; m<nitf_image_unsigned_char->ni(); ++m)
    {
      for (unsigned n=0; n<nitf_image_unsigned_char->nj(); ++n)
      {
        for (unsigned p=0; p<nitf_image_unsigned_char->nplanes(); ++p)
        {
        // we will ignore the most significant 5 bits and less significant 3 bits
          vxl_uint_16 curr_pixel_val = nitf_image_vxl_uint_16(m,n,p);

          if (bigendian) {
            auto* arr = (unsigned char*) &curr_pixel_val;
            // [defgh3][5abc]
            // --> [abcdefgh]
            unsigned char big = *arr;
            unsigned char small = *(++arr);
            big >>= 3;
            small <<= 5;
            curr_pixel_val = big || small;
          }
          else { // little endian
            // [5abc][defgh3]
            // --> [abcdefgh]
            curr_pixel_val <<= 5;
            // [defgh3][5abc]
            // --> [abcdefgh]
            curr_pixel_val >>= 8;
          }

          auto pixel_val = static_cast<unsigned char> (curr_pixel_val);

#if 0
          //This is how Thom use to get the region
          int temp_pix_val = int(int(curr_pixel_val)*255.0/1500.0);
          if (temp_pix_val > 255)
            temp_pix_val =255;
          unsigned char pixel_val = static_cast<unsigned char>(temp_pix_val);
          //end hack
#endif

          //(*nitf_image_unsigned_char)(m,n,p) = pixel_val;
          (*nitf_image_unsigned_char)(m,n,p) = pixel_val;
        }
      }
    }
    img_ptr = nitf_image_unsigned_char;
  }
  else if (roi->pixel_format() == VIL_PIXEL_FORMAT_BYTE) {
    //*nitf_image_unsigned_char = vil_image_view<unsigned char>(roi);
    img_ptr = new vil_image_view<unsigned char>(roi);
  }
  else
    std::cout << "boxm2_roi_init_process - Unsupported Pixel Format = " << roi->pixel_format() << std::endl;

  double u, v;
  camera->image_offset(u, v);
  double tu =  std::floor(u - bb->get_min_x()); // the image is cropped by pixel
  double tv =  std::floor(v - bb->get_min_y());
  vpgl_rational_camera<double> new_cam(*camera);
  new_cam.set_image_offset(tu, tv);
  local_camera = vpgl_local_rational_camera<double> (*lvcs, new_cam);
  return true;
}


std::vector<vgl_point_3d<double> > boxm2_roi_init_process_globals::corners_of_box_3d(vgl_box_3d<double> box)
{
  std::vector<vgl_point_3d<double> > corners;

  corners.push_back(box.min_point());
  corners.emplace_back(box.min_x()+box.width(), box.min_y(), box.min_z());
  corners.emplace_back(box.min_x()+box.width(), box.min_y()+box.height(), box.min_z());
  corners.emplace_back(box.min_x(), box.min_y()+box.height(), box.min_z());
  corners.emplace_back(box.min_x(), box.min_y(), box.max_z());
  corners.emplace_back(box.min_x()+box.width(), box.min_y(), box.max_z());
  corners.push_back(box.max_point());
  corners.emplace_back(box.min_x(), box.min_y()+box.height(), box.max_z());
  return corners;
}

//: project_box function
bool boxm2_roi_init_process_globals::project_box( const vpgl_rational_camera<double>& cam,
                                                  const vpgl_lvcs_sptr& lvcs,
                                                  const vgl_box_3d<double>& box,
                                                  float r,
                                                  vgl_box_2d<double>& roi)
{
  double xoff, yoff, zoff;
  xoff = cam.offset(vpgl_rational_camera<double>::X_INDX);
  yoff = cam.offset(vpgl_rational_camera<double>::Y_INDX);
  zoff = cam.offset(vpgl_rational_camera<double>::Z_INDX);

  // global to local
  double lx, ly, lz;
  lvcs->global_to_local(xoff, yoff, zoff, vpgl_lvcs::wgs84, lx, ly, lz, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  double center[3];
  center[0] = lx;
  center[1] = ly;
  center[2] = lz;

  // create a box with uncertainty
  vgl_box_3d<double> cam_box(center, 2*r, 2*r, 2*r, vgl_box_3d<double>::centre);
  std::vector<vgl_point_3d<double> > cam_corners = corners_of_box_3d(cam_box);
  std::vector<vgl_point_3d<double> > box_corners = corners_of_box_3d(box);

  double lon, lat, gz;
  for (auto cam_corner : cam_corners) {
    lvcs->local_to_global(cam_corner.x(), cam_corner.y(), cam_corner.z(),
                          vpgl_lvcs::wgs84, lon, lat, gz, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
    vpgl_rational_camera<double>* new_cam = cam.clone();
    new_cam->set_offset(vpgl_rational_camera<double>::X_INDX, lon);
    new_cam->set_offset(vpgl_rational_camera<double>::Y_INDX, lat);
    new_cam->set_offset(vpgl_rational_camera<double>::Z_INDX, gz);

    // project the box
    for (auto & box_corner : box_corners) {
      // convert the box corners to world coordinates
      lvcs->local_to_global(box_corner.x(), box_corner.y(), box_corner.z(),
                            vpgl_lvcs::wgs84, lon, lat, gz, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
      vgl_point_2d<double> p2d = new_cam->project(vgl_point_3d<double>(lon, lat, gz));
      roi.add(p2d);
    }
    delete new_cam;
  }
  return true;
}


//: set input and output types
bool boxm2_roi_init_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_roi_init_process_globals;
  //this process takes 5 inputs:
  std::vector<std::string> input_types_(n_inputs_);
  unsigned  i = 0;
  input_types_[i++] = "vcl_string";                // NITF image path
  input_types_[i++] = "vpgl_camera_double_sptr";   // rational camera
  input_types_[i++] = "boxm2_scene_sptr";     // scene
  input_types_[i++] = "bool";  // whether to convert to 8 bits or not, default is true=do the conversion
  input_types_[i++] = "int";   // margin - clip an image with an extra margin around, default is zero if not set
  input_types_[i++] = "int";   // set image dimensions
  input_types_[i++] = "int";   // set image dimensions

  bool good = pro.set_input_types(input_types_);

  //this process takes 3 outputs:
  unsigned j = 0;
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[j++] = "vpgl_camera_double_sptr"; // unadjusted local rational camera
  output_types_[j++] = "vil_image_view_base_sptr";  // image ROI
  output_types_[j++] = "float"; // uncertainty

  good = good && pro.set_output_types(output_types_);

  brdb_value_sptr idx = new brdb_value_t<bool>(true);
  pro.set_input(3, idx);

  brdb_value_sptr idx2 = new brdb_value_t<int>(0);
  pro.set_input(4, idx2);

    brdb_value_sptr idx3 = new brdb_value_t<int>(-1);
  pro.set_input(5, idx3);

    brdb_value_sptr idx4 = new brdb_value_t<int>(-1);
  pro.set_input(6, idx4);

  // set up  process parameters
  bprb_parameters_sptr params = new bprb_parameters();
  if (!params->add<float>("camera uncertainty","camera_uncertainty",0.0f)) {
    std::cerr << pro.name() << " Error adding parameter\n";
    good = false;
  }
  pro.set_parameters(params);

  return good;
}


bool boxm2_roi_init_process(bprb_func_process& pro)
{
  using namespace boxm2_roi_init_process_globals;
  //static const parameters
  static const std::string error = "camera_uncertainty";

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << " The number of inputs should be " << n_inputs_<< std::endl;
    return false;
  }

  // get the inputs:
  unsigned i = 0;
  // image
  std::string image_path = pro.get_input<std::string>(i++);
  // camera
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(i++);
  //voxel_world
  boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(i++);

  bool convert_to_8_bit = pro.get_input<bool>(i++);
  int margin = pro.get_input<int>(i++);

  int clip_image_width = pro.get_input<int>(i++);
    int clip_image_height = pro.get_input<int>(i++);

  // uncertainty (meters)
  float uncertainty=0;
  if ( !pro.parameters()->get_value(error, uncertainty) ) {
      std::cout << pro.name() << ": error in retrieving parameters.\n";
      std::cout << "    using default value for uncertainty: " << uncertainty << std::endl;
  }

  //vil_image_view<unsigned char>* img_ptr = new vil_image_view<unsigned char>();
  vil_image_view_base_sptr img_ptr;
  auto* rat_camera =
    dynamic_cast<vpgl_rational_camera<double>*> (camera.as_pointer());
  if (!rat_camera) {
    std::cerr << "The camera input is not a rational camera\n";
    return false;
  }

  vpgl_local_rational_camera<double> local_camera;
  if (!roi_init(image_path, rat_camera, scene, uncertainty, img_ptr, local_camera, convert_to_8_bit, margin,clip_image_width, clip_image_height)) {
    //std::cerr << "The process has failed!\n";
    return false;
  }

  if (img_ptr->ni() == 0 || img_ptr->nj() == 0)
    return false;

  //Store outputs
  unsigned j = 0;
  // update the camera and store
  pro.set_output_val<vpgl_camera_double_sptr >(j++, new vpgl_local_rational_camera<double> (local_camera));
  // store image output
  pro.set_output_val<vil_image_view_base_sptr>(j++, img_ptr);
  // store uncertainty
  pro.set_output_val<float>(j++, uncertainty);

  return true;
}
