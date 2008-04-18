#include "bvxm_lidar_init_process.h"
#include "bvxm_voxel_world.h"
#include <bvxm/bvxm_util.h>

#include <vcl_cassert.h>

#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vsol/vsol_box_2d.h>

#include <vil/vil_load.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_pixel_format.h>
#include <vil/file_formats/vil_tiff.h>
#include <vil/file_formats/vil_geotiff_header.h>

#include <bgeo/bgeo_utm.h>
#include <brip/brip_roi.h>

#include <bprb/bprb_parameters.h>

bvxm_lidar_init_process::bvxm_lidar_init_process()
{
  //this process takes 3 input:
  //the filename of the image, the camera and the voxel world
  input_data_.resize(3, brdb_value_sptr(0));
  input_types_.resize(3);

  int i=0;
  input_types_[i++] = "vcl_string";             // first ret. image path (geotiff)
  input_types_[i++] = "vcl_string";             // second ret. image path (geotiff)
  input_types_[i++] = "bvxm_voxel_world_sptr";  // rational camera

  //output
  output_data_.resize(4,brdb_value_sptr(0));
  output_types_.resize(4);
  int j=0;
  output_types_[j++]= "vpgl_camera_double_sptr";   // lidar local camera
  output_types_[j++]= "vil_image_view_base_sptr";  // first ret image ROI
  output_types_[j++]= "vil_image_view_base_sptr";  // second ret image ROI
  output_types_[j++]= "vil_image_view_base_sptr";  // mask

  if (!parameters()->add( "Upper bound for Lidar differences" , "mask_thresh" , (float) 10.0 ))
    vcl_cout << "ERROR: Adding parameters in bvxm_lidar_init_process\n";
}

bool bvxm_lidar_init_process::execute()
{
  // Sanity check
  if (!this->verify_inputs())
    return false;

  // get the inputs:
  // image
  brdb_value_t<vcl_string>* input0 =
    static_cast<brdb_value_t< vcl_string>* >(input_data_[0].ptr());
  vcl_string first = input0->value();

  brdb_value_t<vcl_string>* input1 =
    static_cast<brdb_value_t< vcl_string>* >(input_data_[1].ptr());
  vcl_string second = input1->value();

  //voxel_world
  brdb_value_t<bvxm_voxel_world_sptr >* input2 =
    static_cast<brdb_value_t<bvxm_voxel_world_sptr >* >(input_data_[2].ptr());
  bvxm_voxel_world_sptr voxel_world = input2->value();

  // threshold (meters) -- SHOULD BE A PARAM
  float thresh=0;
  if (!parameters()->get_value("mask_thresh", thresh))
    return false;
  bvxm_world_params_sptr world_params = voxel_world->get_params();
  vil_image_resource_sptr first_ret = vil_load_image_resource(first.c_str());
  if (!first_ret) {
    vcl_cout << "bvxm_lidar_init_process -- First return image path is not valid!\n";
    return false;
  }

  // second return may be null, in that case only first return will be considered
  vil_image_resource_sptr second_ret = vil_load_image_resource(second.c_str());

  bgeo_lvcs_sptr lvcs = world_params->lvcs();
  if (!lvcs) {
    vcl_cout << "bvxm_lidar_init_process -- LVCS is not set!\n";
    return false;
  }

  vil_image_view_base_sptr roi_first=0, roi_second=0;
  bvxm_lidar_camera *cam_first=0, *cam_second=0;

  if (!lidar_init(first_ret, world_params, roi_first, cam_first)) {
    vcl_cout << "bvxm_lidar_init_process -- The process has failed!\n";
    return false;
  }

  if (second_ret) {
    if (!lidar_init(second_ret, world_params, roi_second, cam_second)) {
      vcl_cout << "bvxm_lidar_init_process -- The process has failed!\n";
      return false;
    }
  }

  vil_image_view_base_sptr mask=0;
  if (!gen_mask(roi_first, cam_first, roi_second, cam_second, mask, thresh)) {
    vcl_cout << "bvxm_lidar_init_process -- The process has failed!\n";
    return false;
  }

  // store the camera (camera of first return is sufficient)
  brdb_value_sptr output0 =
    new brdb_value_t<vpgl_camera_double_sptr >(cam_first);
  output_data_[0] = output0;

  // store image output (first return roi)
  brdb_value_sptr output1 =
    new brdb_value_t<vil_image_view_base_sptr>(roi_first);
  output_data_[1] = output1;

  // store image output (second return roi)
  brdb_value_sptr output2 =
    new brdb_value_t<vil_image_view_base_sptr>(roi_second);
  output_data_[2] = output2;

  // store the mask
  brdb_value_sptr output3 =
    new brdb_value_t<vil_image_view_base_sptr>(mask);
  output_data_[3] = output3;
  return true;
}

bool bvxm_lidar_init_process::lidar_init(vil_image_resource_sptr lidar,
                                         bvxm_world_params_sptr params,
                                         vil_image_view_base_sptr& roi,
                                         bvxm_lidar_camera*& camera)
{
  // the file should be a geotiff
  vcl_cout << "FORMAT=" << lidar->file_format();
  if (vcl_strcmp(lidar->file_format(),"tiff") != 0) {
    vcl_cout << "bvxm_lidar_init_process::lidar_init -- The image should be a TIFF!\n";
    return false;
  }

#if HAS_GEOTIFF
  vil_tiff_image* tiff_img = static_cast<vil_tiff_image*> (lidar.as_pointer());

  // check if the tiff file is geotiff
  if (!tiff_img->is_GEOTIFF()) {
    vcl_cout << "bvxm_lidar_init_process::lidar_init -- The image should be a GEOTIFF!\n";
    return false;
  }

  vil_geotiff_header* gtif = tiff_img->get_geotiff_header();
  int utm_zone;
  vil_geotiff_header::GTIF_HEMISPH h;

  // convert given tiepoint to world coordinates (lat, long)
  // based on the model defined in GEOTIFF

  // is this a PCS_WGS84_UTM?
  bool is_utm = false;
  if (gtif->PCS_WGS84_UTM_zone(utm_zone, h))
  {
    vcl_vector<vcl_vector<double> > tiepoints;
    gtif->gtif_tiepoints(tiepoints);
    bool south_flag = false;
    if (h == 1)
      south_flag = true;
    is_utm = true;
    // transform each tiepoint
    bgeo_utm utm;
    double lat, lon, elev ;
    for (unsigned i=0; i< tiepoints.size(); i++) {
      vcl_vector<double> tiepoint = tiepoints[i];
      assert (tiepoint.size() == 6);
      double I = tiepoint[0];
      double J = tiepoint[1];
      double K = tiepoint[2];
      double X = tiepoint[3];
      double Y = tiepoint[4];
      double Z = tiepoint[5];

      utm.transform(utm_zone, X, Y, Z, lat, lon, elev, south_flag );
      //scale_ = 1;
      // now, we have a mapping (I,J,K)->(X,Y,Z)
    }

    // create a transformation matrix
    // if there is a transormation matrix in GEOTIFF, use that
    vnl_matrix<double> trans_matrix;
    double* trans_matrix_values;
    double sx1, sy1, sz1;
    if (gtif->gtif_trans_matrix(trans_matrix_values)){
      vcl_cout << "Transfer matrix is given, using that...." << vcl_endl;
      trans_matrix.copy_in(trans_matrix_values);
    } else if (gtif->gtif_pixelscale(sx1, sy1, sz1)) {
      comp_trans_matrix(sx1, sy1, sz1, tiepoints, trans_matrix);
    } else {
      vcl_cout << "bvxm_lidar_init_process::comp_trans_matrix -- Transform matrix cannot be formed..\n";
      return false;
    }

    // create the camera
    camera = new bvxm_lidar_camera(trans_matrix, params->lvcs(), tiepoints);
    if (is_utm)
      camera->set_utm(utm_zone, h);

    // create image view
    vil_tiff_image* tiff_img = static_cast<vil_tiff_image*> (lidar.as_pointer());

    // crop the image to include the voxel world only
    // backproject the voxel world coordinates on the image
    vgl_box_2d<double> roi_box;
    vgl_box_3d<double> world = params->world_box_local();
    vcl_vector<vgl_point_3d<double> > corners = bvxm_util::corners_of_box_3d<double>(world);
    for (unsigned i=0; i<corners.size(); i++) {
      double x = corners[i].x();
      double y = corners[i].y();
      double z = corners[i].z();
      double u,v;
      camera->project(x,y,z,u,v);
      vgl_point_2d<double> p(u,v);
      roi_box.add(p);

    }
    
    brip_roi broi(tiff_img->ni(), tiff_img->nj());
    vsol_box_2d_sptr bb = new vsol_box_2d();
    bb->add_point(roi_box.min_x(), roi_box.min_y());
    bb->add_point(roi_box.max_x(), roi_box.max_y());
    
    bb = broi.clip_to_image_bounds(bb);
    roi = tiff_img->get_copy_view((unsigned int)bb->get_min_x(),
                                  (unsigned int)bb->width(),
                                  (unsigned int)bb->get_min_y(),
                                  (unsigned int)bb->height());
    //add the translation to the camera
    camera->translate(bb->get_min_x(), bb->get_min_y());

    if (!roi) {
      vcl_cout << "bvxm_lidar_init_process::lidar_init()-- clipping box is out of image boundaries\n";
      return false;
    }
  }
  else {
      vcl_cout << "bvxm_lidar_init_process::lidar_init()-- Only ProjectedCSTypeGeoKey=PCS_WGS84_UTM_zoneXX_X is defined rigth now, please define yours!!" << vcl_endl;
      return false;
  }

  return true;
#else // if !HAS_GEOTIFF
  vcl_cout << "bvxm_lidar_init_process::lidar_init()-- GEOTIFF lib is needed to run bvxm_lidar_init_process--\n";
  return false;
#endif // HAS_GEOTIFF
}

bool bvxm_lidar_init_process::comp_trans_matrix(double sx1, double sy1, double sz1,//vil_geotiff_header* gtif,
                                                vcl_vector<vcl_vector<double> > tiepoints,
                                                vnl_matrix<double>& trans_matrix)
{
  double sx, sy, sz;

  // use tiepoints and scale values to create a transformation matrix
  // for now use the first tiepoint if there are more than one
  assert (tiepoints.size() > 0);
  vcl_vector<double> tiepoint = tiepoints[0];
  assert (tiepoint.size() == 6);
  double I = tiepoint[0];
  double J = tiepoint[1];
  double K = tiepoint[2];
  double X = tiepoint[3];
  double Y = tiepoint[4];
  double Z = tiepoint[5];

  // Define a transformation matrix as follows:
  //
  //      |-                         -|
  //      |   Sx    0.0   0.0   Tx    |
  //      |                           |      Tx = X - I*Sx
  //      |   0.0  -Sy    0.0   Ty    |      Ty = Y + J*Sy
  //      |                           |      Tz = Z - K*Sz
  //      |   0.0   0.0   Sz    Tz    |
  //      |                           |
  //      |   0.0   0.0   0.0   1.0   |
  //      |-                         -|

  double scale = 1;
  sx = scale; sy=scale; sz=scale;
  double Tx = X - I*sx;
  double Ty = Y + J*sy;
  double Tz = Z - K*sz;

  vnl_matrix<double> m(4,4);
  m.fill(0.0);
  m[0][0] = sx;
  m[1][1] = -1*sy;
  m[2][2] = sz;
  m[3][3] = 1.0;
  m[0][3] = Tx;
  m[1][3] = Ty;
  m[2][3] = Tz;
  trans_matrix = m;
  vcl_cout << trans_matrix << vcl_endl;
  return true;
}

bool bvxm_lidar_init_process::gen_mask(vil_image_view_base_sptr roi_first,
                                       bvxm_lidar_camera* cam_first,
                                       vil_image_view_base_sptr roi_second,
                                       bvxm_lidar_camera* cam_second,
                                       vil_image_view_base_sptr& mask,
                                       double thresh)
{
  // compare the cameras, if the second one existed
  if (!cam_first) {
    vcl_cout << "bvxm_lidar_init_process::gen_mask -- camera not found!\n";
    return false;
  }

  if (!roi_first) {
    vcl_cout << "bvxm_lidar_init_process::gen_mask -- image not found!\n";
    return false;
  }

  vil_image_view<bool>* view = new vil_image_view<bool>(roi_first->ni(), roi_first->nj());
  // if there is no second camera and image, just use one
  if (!roi_second || !cam_second) {
    view->fill(0);
    mask = view;
  } else {
    assert(roi_first->ni() == roi_second->ni());
    assert(roi_first->nj() == roi_second->nj());

    if (roi_first->pixel_format() == VIL_PIXEL_FORMAT_FLOAT) {
      vil_image_view<float>* view1 = static_cast<vil_image_view<float>*> (roi_first.as_pointer());
      vil_image_view<float>* view2 = static_cast<vil_image_view<float>*> (roi_second.as_pointer());
      // compare the cameras, they should be the same
      for (unsigned i=0; i<roi_first->ni(); i++)
        for (unsigned j=0; j<roi_first->nj(); j++) {
          double diff = (*view1)(i,j)-(*view2)(i,j);
          if (diff > thresh)
            (*view)(i,j) = false;
          else
            (*view)(i,j) = true;
        }
    }
    mask = view;
  }
  return true;
}
