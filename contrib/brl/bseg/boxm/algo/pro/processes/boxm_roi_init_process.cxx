// This is brl/bseg/boxm/algo/pro/processes/boxm_roi_init_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A class for clipping and image based on a 3D bounding box.
//        -  Input:
//             - Image path (string)
//             - boxm_voxel_world_sptr
//
//        -  Output:
//             - modified rational camera "vpgl_camera_double_sptr"
//             - clipped image area (NITF) "vil_image_view_base_sptr"
//
//        -  Params:
//             -geographic uncertainty (error) in meters
//
// \author  Copied from boxm/pro
// \date    May 5, 2008
// \verbatim
// \endverbatim

#include <boxm/boxm_scene.h>
#include <boxm/util/boxm_utils.h>

#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_box_3d.h>

#include <vsol/vsol_box_2d_sptr.h>
#include <vsol/vsol_box_2d.h>

#include <brip/brip_roi.h>

#include <vil/vil_load.h>
#include <vil/vil_crop.h>
#include <vil/file_formats/vil_nitf2_image.h>
#include <vil/vil_image_view_base.h>
#include <vpgl/vpgl_camera.h>

#include <bprb/bprb_parameters.h>

//: globals variables and functions
namespace boxm_roi_init_process_globals
{
  constexpr unsigned n_inputs_ = 3;
  constexpr unsigned n_outputs_ = 3;

  //functions
  bool roi_init(const std::string& img_path,
                const vpgl_camera_double_sptr& camera,
                vgl_box_3d<double> box,
                vil_image_view<unsigned char> & roi_img);

  //: projects the box on the image by taking the union of all the projected corners
  vgl_box_2d<double>* project_box(const vpgl_camera_double_sptr& cam,
                                  vgl_box_3d<double> box);
}

//: set input and output types
bool boxm_roi_init_process_cons(bprb_func_process& pro)
{
  using namespace boxm_roi_init_process_globals;

  //this process takes 3 input:
  //the filename of the image, the camera and the voxel world
  std::vector<std::string> input_types_(n_inputs_);
  unsigned  i=0;
  input_types_[i++] = "vcl_string";                // image path
  input_types_[i++] = "vpgl_camera_double_sptr";   //  camera
  input_types_[i++] = "boxm_scene_base_sptr";     // voxel world spec

  //output
  unsigned j = 0;
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[j++] = "vpgl_camera_double_sptr"; // unadjusted local rational camera
  output_types_[j++] = "vil_image_view_base_sptr";  // image ROI
  output_types_[j++] = "float"; // uncertainty

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


bool boxm_roi_init_process(bprb_func_process& pro)
{
  using namespace boxm_roi_init_process_globals;
  //static const parameters
  static const std::string error = "error";

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << " The input number should be " << n_inputs_<< std::endl;
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

  vil_image_view_base_sptr img_ptr=vil_load(image_path.c_str());
  auto *temp=new vil_image_view<unsigned char>();
  if (img_ptr->pixel_format()==VIL_PIXEL_FORMAT_BYTE)
  {
    if (auto *img=dynamic_cast<vil_image_view<unsigned char> * > (img_ptr.ptr()))
    {
        vgl_box_2d<double>* roi_box = project_box(camera, scene->get_world_bbox());
        brip_roi broi(img->ni(), img->nj());
        vsol_box_2d_sptr bb = new vsol_box_2d();
        bb->add_point(roi_box->min_x(), roi_box->min_y());
        bb->add_point(roi_box->max_x(), roi_box->max_y());
        bb = broi.clip_to_image_bounds(bb);
        if (bb->width() <= 0 || bb->height() <= 0) {
            std::cerr << "boxm_roi_init_process::roi_init()-- clipping box is out of image boundaries\n";
            return false;
        }
        temp->set_size(int(bb->width()),int(bb->height()));
        temp->deep_copy(vil_crop(*img,(unsigned int)bb->get_min_x(),
                            (unsigned int)bb->width(),
                            (unsigned int)bb->get_min_y(),
                            (unsigned int)bb->height()));
    }
  }
  if (temp->ni() == 0 || temp->nj() == 0)
    return false;

  //Store outputs
  unsigned j = 0;
  // update the camera and store
  pro.set_output_val<vpgl_camera_double_sptr >(j++, camera);
  // store image output
  pro.set_output_val<vil_image_view_base_sptr>(j++, temp);
  // store uncertainty
  pro.set_output_val<float>(j++, 0);

  return true;
}

//: roi_init function
bool boxm_roi_init_process_globals::roi_init(const std::string&  img_path,
                                             const vpgl_camera_double_sptr& camera,
                                             vgl_box_3d<double> box,
                                             vil_image_view<unsigned char> & roi_img)
{
  vil_image_view_base_sptr img_ptr=vil_load(img_path.c_str());
  if (img_ptr->pixel_format()==VIL_PIXEL_FORMAT_BYTE)
  {
    if (auto *img=dynamic_cast<vil_image_view<unsigned char> * > (img_ptr.ptr()))
    {
        vgl_box_2d<double>* roi_box = project_box(camera, box);
        brip_roi broi(img->ni(), img->nj());
        vsol_box_2d_sptr bb = new vsol_box_2d();
        bb->add_point(roi_box->min_x(), roi_box->min_y());
        bb->add_point(roi_box->max_x(), roi_box->max_y());
        bb = broi.clip_to_image_bounds(bb);
        if (bb->width() <= 0 || bb->height() <= 0) {
            std::cerr << "boxm_roi_init_process::roi_init()-- clipping box is out of image boundaries\n";
            return false;
        }

         vil_image_view<unsigned char> temp=vil_crop(*img,
                                                     (unsigned int)bb->get_min_x(),
                                                     (unsigned int)bb->width(),
                                                     (unsigned int)bb->get_min_y(),
                                                     (unsigned int)bb->height());
        roi_img=temp;
        return true;
    }
    else
      return false;
  }
  else // non-supported pixel format
    return false;
}

//: project_box function
vgl_box_2d<double>* boxm_roi_init_process_globals::project_box( const vpgl_camera_double_sptr& cam,
                                                                vgl_box_3d<double> box)
{
  // create a box with uncertainty
  std::vector<vgl_point_3d<double> > box_corners = boxm_utils::corners_of_box_3d(box);

  auto* roi = new vgl_box_2d<double>();

  for (auto & box_corner : box_corners) {
      double u,v;
      cam->project(box_corner.x(),box_corner.y(),box_corner.z(),u,v);
      vgl_point_2d<double> p2d(u,v);
      std::cout<<u<<' '<<v<<'\n';
      roi->add(p2d);
  }
  return roi;
}
