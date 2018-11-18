// This is brl/bseg/boxm/algo/pro/processes/boxm_roi_init_rational_camera_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A class for clipping and image based on a 3D bounding box.
// *  Input:
//   - Image path (string)
//   - boxm_voxel_world_sptr
//
// *  Output:
//   - modified rational camera "vpgl_camera_double_sptr"
//   - clipped image area (NITF) "vil_image_view_base_sptr"
//
// *  Params:
//   - geographic uncertainty (error) in meters
//
// \author  Copied from boxm/pro
// \date    May 5, 2008

#include <boxm/boxm_scene.h>
#include <boxm/util/boxm_utils.h>

#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_box_3d.h>

#include <vsol/vsol_box_2d_sptr.h>
#include <vsol/vsol_box_2d.h>

#include <brip/brip_roi.h>

#include <vil/vil_load.h>
#include <vil/file_formats/vil_nitf2_image.h>
#include <vil/vil_image_view_base.h>
#include <vpgl/vpgl_local_rational_camera.h>
#include <vpgl/vpgl_lvcs.h>

#include <bprb/bprb_parameters.h>

//: globals variables and functions
namespace boxm_roi_init_rational_camera_process_globals
{
  constexpr unsigned n_inputs_ = 3;
  constexpr unsigned n_outputs_ = 3;

  // functions
  bool roi_init(std::string const& image_path,
                vpgl_rational_camera<double>* camera,
                vgl_box_3d<double> box,
                const vpgl_lvcs& lvcs,
                float uncertainty,
                vil_image_view<unsigned char>* nitf_image_unsigned_char,
                vpgl_local_rational_camera<double>& local_camera);

  //: projects the box on the image by taking the union of all the projected corners
  vgl_box_2d<double>* project_box(vpgl_rational_camera<double>* cam,
                                  vpgl_lvcs lvcs,
                                  vgl_box_3d<double> box,
                                  float r);
}

//:set input and output types
bool boxm_roi_init_rational_camera_process_cons(bprb_func_process& pro)
{
  using namespace boxm_roi_init_rational_camera_process_globals;

  //this process takes 3 input:
  //the filename of the image, the camera and the voxel world
  std::vector<std::string> input_types_(n_inputs_);
  unsigned  i=0;
  input_types_[i++] = "vcl_string";                // NITF image path
  input_types_[i++] = "vpgl_camera_double_sptr";   // rational camera
  input_types_[i++] = "boxm_scene_base_sptr";     // voxel world spec

  //output
  unsigned j = 0;
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[j++] = "vpgl_camera_double_sptr"; // unadjusted local rational camera
  output_types_[j++] = "vil_image_view_base_sptr";  // image ROI
  output_types_[j++] = "float"; // uncertainty

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


bool boxm_roi_init_rational_camera_process(bprb_func_process& pro)
{
  using namespace boxm_roi_init_rational_camera_process_globals;
  //static const parameters
  static const std::string error = "error";

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << " The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  // uncertainty (meters) -- SHOULD BE A PARAM
  float uncertainty=10.0;
  if ( !pro.parameters()->get_value(error, uncertainty) ) {
    std::cout << pro.name() << ": error in retrieving parameters\n";
    return false;
  }

  // get the inputs:
  unsigned i = 0;
  // image
  std::string image_path = pro.get_input<std::string>(i++);
  // camera
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(i++);
  //voxel_world
  boxm_scene_base_sptr scene = pro.get_input<boxm_scene_base_sptr>(i++);

  auto* img_ptr = new vil_image_view<unsigned char>();
  auto* rat_camera =
  dynamic_cast<vpgl_rational_camera<double>*> (camera.as_pointer());

  if (!rat_camera) {
    std::cerr << "The camera input is not a rational camera\n";
    return false;
  }

  vpgl_local_rational_camera<double> local_camera;
  if (scene->appearence_model() == BOXM_APM_MOG_GREY ||
      scene->appearence_model() == BOXM_EDGE_FLOAT ||
      scene->appearence_model() == BOXM_EDGE_LINE) {
    typedef boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > tree_type;
    auto *s = static_cast<boxm_scene<tree_type>*> (scene.as_pointer());

    if (!roi_init(image_path, rat_camera, s->get_world_bbox(),(s->lvcs()), uncertainty, img_ptr, local_camera)) {
      std::cerr << "The process has failed!\n";
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
  }
  else {
    std::cout << "boxm_refine_scene_process: undefined APM type" << std::endl;
    return false;
  }
  return true;
}

//: roi_init function
bool boxm_roi_init_rational_camera_process_globals::roi_init(std::string const& image_path,
                                                             vpgl_rational_camera<double>* camera,
                                                             vgl_box_3d<double> box,
                                                             const vpgl_lvcs& lvcs,
                                                             float uncertainty,
                                                             vil_image_view<unsigned char>* nitf_image_unsigned_char,
                                                             vpgl_local_rational_camera<double>& local_camera)
{
  // read the image and extract the camera
  vil_image_resource_sptr img = vil_load_image_resource(image_path.c_str());
  std::string format = img->file_format();
  std::string prefix = format.substr(0,4);
  if (prefix.compare("nitf") != 0) {
    std::cerr << "boxm_roi_init_rational_camera_process::execute - The image should be an NITF\n";
    return false;
  }

  auto* nitf =  static_cast<vil_nitf2_image*> (img.ptr());

  vgl_box_2d<double>* roi_box = project_box(camera, lvcs, box, uncertainty);

  brip_roi broi(nitf->ni(), nitf->nj());
  vsol_box_2d_sptr bb = new vsol_box_2d();
  bb->add_point(roi_box->min_x(), roi_box->min_y());
  bb->add_point(roi_box->max_x(), roi_box->max_y());
  bb = broi.clip_to_image_bounds(bb);

  if (bb->width() <= 0 || bb->height() <= 0) {
    std::cerr << "boxm_roi_init_rational_camera_process::roi_init()-- clipping box is out of image boundaries\n";
    return false;
  }

  vil_image_view_base_sptr roi = nitf->get_copy_view((unsigned int)bb->get_min_x(),
                                                     (unsigned int)bb->width(),
                                                     (unsigned int)bb->get_min_y(),
                                                     (unsigned int)bb->height());
  if (!roi) {
    std::cerr << "boxm_roi_init_rational_camera_process::roi_init()-- clipping box is out of image boundaries\n";
    return false;
  }

  if (!roi.as_pointer())
    return false;

  if (roi->pixel_format() == VIL_PIXEL_FORMAT_UINT_16)
  {
    vil_image_view<vxl_uint_16> nitf_image_vxl_uint_16(roi);

    *nitf_image_unsigned_char = vil_image_view<unsigned char> (roi->ni(),roi->nj(),roi->nplanes());

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
          //This is how Thom uses to get the region
          int temp_pix_val = int(int(curr_pixel_val)*255.0/1500.0);
          if (temp_pix_val > 255)
            temp_pix_val =255;
          unsigned char pixel_val = static_cast<unsigned char>(temp_pix_val);
          //end hack
#endif

          (*nitf_image_unsigned_char)(m,n,p) = pixel_val;
        }
      }
    }
  }
  else if (roi->pixel_format() == VIL_PIXEL_FORMAT_BYTE) {
    *nitf_image_unsigned_char = vil_image_view<unsigned char>(roi);
  }
  else
    std::cout << "boxm_roi_init_rational_camera_process - Unsupported Pixel Format = " << roi->pixel_format() << std::endl;

  double u, v;
  camera->image_offset(u, v);
  double tu =  u - roi_box->min_x();
  double tv =  v - roi_box->min_y();
  camera->set_image_offset(tu, tv);
  local_camera = vpgl_local_rational_camera<double> (lvcs, *camera);
  delete roi_box;

  return true;
}

//:project_box function
vgl_box_2d<double>*boxm_roi_init_rational_camera_process_globals::project_box( vpgl_rational_camera<double>* cam,
                                                                               vpgl_lvcs lvcs,
                                                                               vgl_box_3d<double> box,
                                                                               float r)
{
  double xoff, yoff, zoff;
  xoff = cam->offset(vpgl_rational_camera<double>::X_INDX);
  yoff = cam->offset(vpgl_rational_camera<double>::Y_INDX);
  zoff = cam->offset(vpgl_rational_camera<double>::Z_INDX);

  // global to local
  double lx, ly, lz;
  lvcs.global_to_local(xoff, yoff, zoff, vpgl_lvcs::wgs84, lx, ly, lz, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  double center[3];
  center[0] = lx;
  center[1] = ly;
  center[2] = lz;

  // create a box with uncertainty
  vgl_box_3d<double> cam_box(center, 2*r, 2*r, 2*r, vgl_box_3d<double>::centre);
  std::vector<vgl_point_3d<double> > cam_corners = boxm_utils::corners_of_box_3d(cam_box);
  std::vector<vgl_point_3d<double> > box_corners = boxm_utils::corners_of_box_3d(box);
  auto* roi = new vgl_box_2d<double>();

  double lon, lat, gz;
  for (auto cam_corner : cam_corners) {
    lvcs.local_to_global(cam_corner.x(), cam_corner.y(), cam_corner.z(),
                         vpgl_lvcs::wgs84, lon, lat, gz, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
    vpgl_rational_camera<double>* new_cam = cam->clone();
    new_cam->set_offset(vpgl_rational_camera<double>::X_INDX, lon);
    new_cam->set_offset(vpgl_rational_camera<double>::Y_INDX, lat);
    new_cam->set_offset(vpgl_rational_camera<double>::Z_INDX, gz);

    // project the box
    for (auto & box_corner : box_corners) {
      // convert the box corners to world coordinates
      lvcs.local_to_global(box_corner.x(), box_corner.y(), box_corner.z(),
                           vpgl_lvcs::wgs84, lon, lat, gz, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
      vgl_point_2d<double> p2d = new_cam->project(vgl_point_3d<double>(lon, lat, gz));
      roi->add(p2d);
    }
  }
  return roi;
}
