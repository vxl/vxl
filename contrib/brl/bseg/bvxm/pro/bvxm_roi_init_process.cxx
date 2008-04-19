#include "bvxm_roi_init_process.h"
#include "bvxm_voxel_world.h"
#include "bvxm_world_params.h"
#include <bvxm/bvxm_util.h>

#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_point_2d.h>
#include <vsol/vsol_box_2d_sptr.h>
#include <vsol/vsol_box_2d.h>
#include <brip/brip_roi.h>

#include <vil/vil_load.h>
#include <vil/file_formats/vil_nitf2_image.h>
#include <vil/vil_image_view_base.h>
#include <vpgl/vpgl_local_rational_camera.h>

#include <bprb/bprb_parameters.h>


bvxm_roi_init_process::bvxm_roi_init_process()
{
  //this process takes 3 input:
  //the filename of the image, the camera and the voxel world
  input_data_.resize(3, brdb_value_sptr(0));
  input_types_.resize(3);

  int i=0;
  input_types_[i++] = "vcl_string";                // NITF image path
  input_types_[i++] = "vpgl_camera_double_sptr";   // rational camera
  input_types_[i++] = "bvxm_voxel_world_sptr";     // voxel world spec

  //output
  output_data_.resize(2,brdb_value_sptr(0));
  output_types_.resize(2);
  int j=0;
  output_types_[j++]= "vpgl_camera_double_sptr";   // unadjusted local rational camera
  output_types_[j++]= "vil_image_view_base_sptr";  // image ROI

  if (!parameters()->add( "Uncertainty error" , "error" , (float) 10.0 ))
    vcl_cerr << "ERROR: Adding parameters in bvxm_roi_init_process\n";
}

bool bvxm_roi_init_process::execute()
{
  // Sanity check
  if (!this->verify_inputs())
    return false;

  // get the inputs:
  // image
  brdb_value_t<vcl_string>* input0 =
    static_cast<brdb_value_t< vcl_string>* >(input_data_[0].ptr());
  vcl_string image_path = input0->value();

  // camera
  brdb_value_t<vpgl_camera_double_sptr >* input1 =
    static_cast<brdb_value_t<vpgl_camera_double_sptr >* >(input_data_[1].ptr());
  vpgl_camera_double_sptr camera = input1->value();

  //voxel_world
  brdb_value_t<bvxm_voxel_world_sptr >* input2 =
    static_cast<brdb_value_t<bvxm_voxel_world_sptr >* >(input_data_[2].ptr());
  bvxm_voxel_world_sptr voxel_world = input2->value();

  // uncertainity (meters) -- SHOULD BE A PARAM
  float uncertainty=0;
  if (!parameters()->get_value("error", uncertainty))
    return false;

  bvxm_world_params_sptr world_params = voxel_world->get_params();

  vil_image_view<unsigned char>* img_ptr = new vil_image_view<unsigned char>();
  vpgl_rational_camera<double>* rat_camera =
    dynamic_cast<vpgl_rational_camera<double>*> (camera.as_pointer());
  if (!rat_camera) {
    vcl_cerr << "The camera input is not a rational camera\n";
    return false;
  }

  vpgl_local_rational_camera<double> local_camera;
  if (!roi_init(image_path, rat_camera, world_params, uncertainty, img_ptr, local_camera)) {
    vcl_cerr << "The process has failed!\n";
    return false;
  }

  // update the camera and store
  brdb_value_sptr output0 =
    new brdb_value_t<vpgl_camera_double_sptr >(new vpgl_local_rational_camera<double> (local_camera));
  output_data_[0] = output0;

  // store image output
  brdb_value_sptr output1 =
    new brdb_value_t<vil_image_view_base_sptr>(img_ptr);
  output_data_[1] = output1;
  return true;
}

bool bvxm_roi_init_process::roi_init(vcl_string const& image_path,
                                     vpgl_rational_camera<double>* camera,
                                     bvxm_world_params_sptr world_params,
                                     float error,
                                     vil_image_view<unsigned char>* nitf_image_unsigned_char,
                                     vpgl_local_rational_camera<double>& local_camera)
{
  // read the image and extract the camera
  vil_image_resource_sptr img = vil_load_image_resource(image_path.c_str());
  vcl_string format = img->file_format();
  vcl_string prefix = format.substr(0,4);
  if (prefix.compare("nitf") != 0) {
    vcl_cerr << "bvxm_roi_init_process::execute - The image should be an NITF\n";
    return false;
  }

  vil_nitf2_image* nitf =  static_cast<vil_nitf2_image*> (img.ptr());

  vgl_vector_3d<unsigned int> dims = world_params->num_voxels();
  int dimx = dims.x();
  int dimy = dims.y();
  int dimz = dims.z();
  double min_position[3];
  float voxel_length = world_params->voxel_length();
  min_position[0] = world_params->corner().x();
  min_position[1] = world_params->corner().y();
  min_position[2] = world_params->corner().z();
  vgl_box_3d<double> box(min_position,
                         dimx*voxel_length,
                         dimy*voxel_length,
                         dimz*voxel_length,
                         vgl_box_3d<double>::min_pos);

  bgeo_lvcs_sptr lvcs = world_params->lvcs();
  vgl_box_2d<double>* roi_box = project_box(camera, lvcs, box, error);

  brip_roi broi(nitf->ni(), nitf->nj());
  vsol_box_2d_sptr bb = new vsol_box_2d();
  bb->add_point(roi_box->min_x(), roi_box->min_y());
  bb->add_point(roi_box->max_x(), roi_box->max_y());
  bb = broi.clip_to_image_bounds(bb);

  vil_image_view_base_sptr roi =
    nitf->get_copy_view((unsigned int)bb->get_min_x(),
                        (unsigned int)bb->width(),
                        (unsigned int)bb->get_min_y(),
                        (unsigned int)bb->height());
  if (!roi) {
    vcl_cerr << "bvxm_roi_init_process::roi_init()-- clipping box is out of image boundaries\n";
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
          
          //Isa commented out this: This is temporarly commented out in order to reproduce Thom's initiall results
          /*
          if (bigendian) {
            unsigned char* arr = (unsigned char*) &curr_pixel_val;
            // [defgh3][5abc]
            // --> [abcdefgh]
            unsigned char big = *arr;
            unsigned char small = *(++arr);
            big = big >> 3;
            small = small << 5;
            curr_pixel_val = big || small;
          }
          else { // little endian
            // [5abc][defgh3]
            // --> [abcdefgh]
            curr_pixel_val = curr_pixel_val << 5;
            // [defgh3][5abc]
            // --> [abcdefgh]
            curr_pixel_val = curr_pixel_val >> 8;
          }*/

                  
          unsigned char pixel_val = static_cast<unsigned char> (curr_pixel_val);
          
          //Temporary hack: Remove when Thom's results have been proved
          int temp_pix_val = floor(int(curr_pixel_val)*255.0/1500.0);
          if (temp_pix_val > 255)
            temp_pix_val =255;
          pixel_val = static_cast<unsigned char>(temp_pix_val);
          //end hack
       
          (*nitf_image_unsigned_char)(m,n,p) = pixel_val;
        }
      }
    }
  }
  else if (roi->pixel_format() == VIL_PIXEL_FORMAT_BYTE) {
    *nitf_image_unsigned_char = vil_image_view<unsigned char>(roi);
  }
  else
    vcl_cout << "bvxm_roi_init_process - Unsupported Pixel Format = " << roi->pixel_format() << vcl_endl;

  double u, v;
  camera->image_offset(u, v);
  double tu =  u - roi_box->min_x();
  double tv =  v - roi_box->min_y();
  camera->set_image_offset(tu, tv);
  local_camera = vpgl_local_rational_camera<double> (*lvcs, *camera);
  delete roi_box;
  return true;
}

vgl_box_2d<double>*
bvxm_roi_init_process::project_box(vpgl_rational_camera<double>* cam,
                                   bgeo_lvcs_sptr lvcs,
                                   vgl_box_3d<double> box,
                                   float r)
{
  double xoff, yoff, zoff;
  xoff = cam->offset(vpgl_rational_camera<double>::X_INDX);
  yoff = cam->offset(vpgl_rational_camera<double>::Y_INDX);
  zoff = cam->offset(vpgl_rational_camera<double>::Z_INDX);

  // global to local
  double lx, ly, lz;
  lvcs->global_to_local(xoff, yoff, zoff, bgeo_lvcs::wgs84, lx, ly, lz, bgeo_lvcs::DEG, bgeo_lvcs::METERS);
  double center[3];
  center[0] = lx;
  center[1] = ly;
  center[2] = lz;

  // create a box with uncertainty
  vgl_box_3d<double> cam_box(center, 2*r, 2*r, 2*r, vgl_box_3d<double>::centre);
  vcl_vector<vgl_point_3d<double> > cam_corners = bvxm_util::corners_of_box_3d<double>(cam_box);
  vcl_vector<vgl_point_3d<double> > box_corners = bvxm_util::corners_of_box_3d<double>(box);
  vgl_box_2d<double>* roi = new vgl_box_2d<double>();

  double lon, lat, gz;
  for (unsigned i=0; i<cam_corners.size(); i++) {
    vgl_point_3d<double> cam_corner = cam_corners[i];
    lvcs->local_to_global(cam_corner.x(), cam_corner.y(), cam_corner.z(),
                          bgeo_lvcs::wgs84, lon, lat, gz, bgeo_lvcs::DEG, bgeo_lvcs::METERS);
    vpgl_rational_camera<double>* new_cam = cam->clone();
    new_cam->set_offset(vpgl_rational_camera<double>::X_INDX, lon);
    new_cam->set_offset(vpgl_rational_camera<double>::Y_INDX, lat);
    new_cam->set_offset(vpgl_rational_camera<double>::Z_INDX, gz);

    // project the box
    for (unsigned int j=0; j < box_corners.size(); j++) {
      // convert the box corners to world coordinates
      lvcs->local_to_global(box_corners[j].x(), box_corners[j].y(), box_corners[j].z(),
                            bgeo_lvcs::wgs84, lon, lat, gz, bgeo_lvcs::DEG, bgeo_lvcs::METERS);
      vgl_point_2d<double> p2d = new_cam->project(vgl_point_3d<double>(lon, lat, gz));
      roi->add(p2d);
    }
  }
  return roi;
}

#if 0
vcl_vector<vgl_point_3d<double> >
bvxm_roi_init_process::corners_of_box_3d(vgl_box_3d<double> box)
{
  vcl_vector<vgl_point_3d<double> > corners;

  corners.push_back(box.min_point());
  corners.push_back(vgl_point_3d<double> (box.min_x()+box.width(), box.min_y(), box.min_z()));
  corners.push_back(vgl_point_3d<double> (box.min_x()+box.width(), box.min_y()+box.height(), box.min_z()));
  corners.push_back(vgl_point_3d<double> (box.min_x(), box.min_y()+box.height(), box.min_z()));
  corners.push_back(vgl_point_3d<double> (box.min_x(), box.min_y(), box.max_z()));
  corners.push_back(vgl_point_3d<double> (box.min_x()+box.width(), box.min_y(), box.max_z()));
  corners.push_back(box.max_point());
  corners.push_back(vgl_point_3d<double> (box.min_x(), box.min_y()+box.height(), box.max_z()));
  return corners;
}
#endif // 0
