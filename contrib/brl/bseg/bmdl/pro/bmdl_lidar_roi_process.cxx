#include "bmdl_lidar_roi_process.h"
//:
// \file

#include <vcl_cstring.h>
#include <vcl_cassert.h>

#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vsol/vsol_box_2d.h>

#include <vil/vil_load.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vil/file_formats/vil_tiff.h>

#include <bprb/bprb_parameters.h>

#include <vpgl/bgeo/bgeo_utm.h>
#include <vpgl/bgeo/bgeo_lvcs.h>
#include <brip/brip_roi.h>

#include <bmdl/bmdl_classify.h>

template<class T>
vcl_vector<vgl_point_3d<T> > corners_of_box_3d(vgl_box_3d<T> box)
{
  vcl_vector<vgl_point_3d<T> > corners;

  corners.push_back(box.min_point());
  corners.push_back(vgl_point_3d<T> (box.min_x()+box.width(), box.min_y(), box.min_z()));
  corners.push_back(vgl_point_3d<T> (box.min_x()+box.width(), box.min_y()+box.height(), box.min_z()));
  corners.push_back(vgl_point_3d<T> (box.min_x(), box.min_y()+box.height(), box.min_z()));
  corners.push_back(vgl_point_3d<T> (box.min_x(), box.min_y(), box.max_z()));
  corners.push_back(vgl_point_3d<T> (box.min_x()+box.width(), box.min_y(), box.max_z()));
  corners.push_back(box.max_point());
  corners.push_back(vgl_point_3d<T> (box.min_x(), box.min_y()+box.height(), box.max_z()));
  return corners;
}

bmdl_lidar_roi_process::bmdl_lidar_roi_process()
{
  //this process takes 2 input:
  //the file paths of the first return and second return
  input_data_.resize(7, brdb_value_sptr(0));
  input_types_.resize(7);

  int i=0;
  input_types_[i++] = "vcl_string";      // first ret. image path (geotiff)
  input_types_[i++] = "vcl_string";      // last ret. image path (geotiff)
  input_types_[i++] = "vcl_string";      // last ret. image path (geotiff)
  input_types_[i++] = "float";           // min_lat "float"
  input_types_[i++] = "float";           // min_lon "float"
  input_types_[i++] = "float";           // max_lat "float"
  input_types_[i++] = "float";           // max_lon "float"

  //output
  output_data_.resize(4,brdb_value_sptr(0));
  output_types_.resize(4);
  int j=0;
  output_types_[j++]= "vil_image_view_base_sptr";  // first return roi
  output_types_[j++]= "vil_image_view_base_sptr";  // last return roi
  output_types_[j++]= "vil_image_view_base_sptr";  // ground roi
  output_types_[j++]= "vpgl_camera_double_sptr";   // lvcs
}

bool bmdl_lidar_roi_process::execute()
{
  // Sanity check
  if (!this->verify_inputs())
    return false;

  // get the inputs:
  // images
  brdb_value_t<vcl_string>* input0 = static_cast<brdb_value_t< vcl_string>* >(input_data_[0].ptr());
  vcl_string first = input0->value();

  brdb_value_t<vcl_string>* input1 = static_cast<brdb_value_t< vcl_string>* >(input_data_[1].ptr());
  vcl_string last = input1->value();

  brdb_value_t<vcl_string>* input2 = static_cast<brdb_value_t< vcl_string>* >(input_data_[2].ptr());
  vcl_string ground = input2->value();

  brdb_value_t<float>* input3 = static_cast<brdb_value_t<float>* >(input_data_[3].ptr());
  float min_lat = input3->value();

  brdb_value_t<float>* input4 = static_cast<brdb_value_t<float>* >(input_data_[4].ptr());
  float min_lon = input4->value();

  brdb_value_t<float>* input5 = static_cast<brdb_value_t<float>* >(input_data_[5].ptr());
  float max_lat = input5->value();

  brdb_value_t<float>* input6 = static_cast<brdb_value_t<float>* >(input_data_[6].ptr());
  float max_lon = input6->value();

  // check first return's validity
  vil_image_resource_sptr first_ret = vil_load_image_resource(first.c_str());
  if (!first_ret) {
    vcl_cout << "bmdl_lidar_roi_process -- First return image path is not valid!\n";
    return false;
  }

  // check last return's validity
  vil_image_resource_sptr last_ret = vil_load_image_resource(last.c_str());
  if (!last_ret) {
    vcl_cout << "bmdl_lidar_roi_process -- Last return image path is not valid!\n";
    return false;
  }

  // Ground image path can be invalid or empty, in that case an estimated ground will be computed
  vil_image_resource_sptr ground_img =0;
  if (ground.size() > 0) {
    ground_img = vil_load_image_resource(ground.c_str());
  }

  vil_image_view_base_sptr first_roi=0, last_roi=0, ground_roi;
  vpgl_geo_camera* lidar_cam =0;
  if (!lidar_roi(first_ret, last_ret, ground_img,
    min_lat, min_lon, max_lat, max_lon, first_roi, last_roi, ground_roi, lidar_cam)) {
    vcl_cout << "bmdl_lidar_roi_process -- The process has failed!\n";
    return false;
  }

  // store image output (first return roi)
  brdb_value_sptr output0 =
    new brdb_value_t<vil_image_view_base_sptr>(first_roi);
  output_data_[0] = output0;

  // store image output (last return roi)
  brdb_value_sptr output1 =
    new brdb_value_t<vil_image_view_base_sptr>(last_roi);
  output_data_[1] = output1;

  // store image output (ground roi)
  brdb_value_sptr output2 =
    new brdb_value_t<vil_image_view_base_sptr>(ground_roi);
  output_data_[2] = output2;

  // store the camera (camera of first return is sufficient)
  brdb_value_sptr output3 =
    new brdb_value_t<vpgl_camera_double_sptr >(lidar_cam);
  output_data_[3] = output3;

  return true;
}

bool bmdl_lidar_roi_process::lidar_roi(vil_image_resource_sptr lidar_first,
                                       vil_image_resource_sptr lidar_last,
                                       vil_image_resource_sptr ground,
                                       float min_lat, float min_lon,
                                       float max_lat, float max_lon,
                                       vil_image_view_base_sptr& first_roi,
                                       vil_image_view_base_sptr& last_roi,
                                       vil_image_view_base_sptr& ground_roi,
                                       vpgl_geo_camera*& camera)
{
  // the file should be a at least a tiff (better, geotiff)
  if (vcl_strcmp(lidar_first->file_format(), "tiff") != 0 &&
      vcl_strcmp(lidar_last->file_format(),"tiff") != 0) {
    vcl_cout << "bmdl_lidar_roi_process::lidar_roi -- The lidar images should be a TIFF!\n";
    return false;
  }

 // vil_image_view<double> first_return, last_return;

#if HAS_GEOTIFF
  vil_tiff_image* tiff_first = static_cast<vil_tiff_image*> (lidar_first.ptr());
  vil_tiff_image* tiff_last  = static_cast<vil_tiff_image*> (lidar_last.ptr());

  // check if the tiff file is geotiff
  if (!tiff_first->is_GEOTIFF()) {
    vcl_cout << "bmdl_lidar_roi_process::lidar_init -- The image should be a GEOTIFF!\n";
    return false;
  }

  vil_geotiff_header* gtif = tiff_first->get_geotiff_header();
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
    bgeo_lvcs_sptr lvcs;
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
      lvcs = new bgeo_lvcs(lat,lon,elev);
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
      vpgl_geo_camera::comp_trans_matrix(sx1, sy1, sz1, tiepoints, trans_matrix);
    } else {
      vcl_cout << "bmdl_lidar_roi_process::comp_trans_matrix -- Transform matrix cannot be formed..\n";
      return false;
    }

    // create the camera

    camera = new vpgl_geo_camera(trans_matrix, lvcs, tiepoints);
    if (is_utm)
      camera->set_utm(utm_zone, h);

    // backproject the 3D world coordinates on the image
    vgl_box_2d<double> roi_box;
    vgl_point_3d<double> min_pos(min_lon, min_lat, 0);
    vgl_point_3d<double> max_pos(max_lon, max_lat, 30);
    vgl_box_3d<double> world(min_pos, max_pos);
    vcl_vector<vgl_point_3d<double> > corners = corners_of_box_3d<double>(world);
    for (unsigned i=0; i<corners.size(); i++) {
      double x = corners[i].x();
      double y = corners[i].y();
      double z = corners[i].z();
      double lx, ly, lz;
      lvcs->global_to_local(x, y, z, bgeo_lvcs::wgs84, lx, ly, lz);
      double u,v;
      camera->project(lx,ly,lz,u,v);
      vgl_point_2d<double> p(u,v);
      roi_box.add(p);
    }

    brip_roi broi(tiff_first->ni(), tiff_first->nj());
    vsol_box_2d_sptr bb = new vsol_box_2d();
    bb->add_point(roi_box.min_x(), roi_box.min_y());
    bb->add_point(roi_box.max_x(), roi_box.max_y());

    bb = broi.clip_to_image_bounds(bb);
    first_roi = tiff_first->get_copy_view((unsigned int)bb->get_min_x(),
                                          (unsigned int)bb->width(),
                                          (unsigned int)bb->get_min_y(),
                                          (unsigned int)bb->height());

    last_roi = tiff_last->get_copy_view((unsigned int)bb->get_min_x(),
                                        (unsigned int)bb->width(),
                                        (unsigned int)bb->get_min_y(),
                                        (unsigned int)bb->height());

    // if no ground input, create an estimated one
    if (ground == 0) {
      compute_ground(ground, first_roi, last_roi, ground_roi);
    } else {   // crop the given one
      ground_roi = ground->get_copy_view((unsigned int)bb->get_min_x(),
                                         (unsigned int)bb->width(),
                                         (unsigned int)bb->get_min_y(),
                                         (unsigned int)bb->height());
    }

    //add the translation to the camera
    camera->translate(bb->get_min_x(), bb->get_min_y());

    if (!first_roi) {
      vcl_cout << "bmdl_lidar_roi_process::lidar_init()-- clipping box is out of image boundaries\n";
      return false;
    }
  }
  else {
      vcl_cout << "bmdl_lidar_roi_process::lidar_init()-- Only ProjectedCSTypeGeoKey=PCS_WGS84_UTM_zoneXX_X is defined rigth now, please define yours!!" << vcl_endl;
      return false;
  }

  return true;
#else // if !HAS_GEOTIFF
  vcl_cout << "bmdl_lidar_roi_process::lidar_init()-- GEOTIFF lib is needed to run bmdl_lidar_roi_process--\n";
  return false;
#endif // HAS_GEOTIFF

  return true;
}

bool bmdl_lidar_roi_process::compute_ground(vil_image_resource_sptr ground,
                                            vil_image_view_base_sptr first_roi,
                                            vil_image_view_base_sptr last_roi,
                                            vil_image_view_base_sptr& ground_roi)
{
  if (ground == 0)
  {
    if ((first_roi->pixel_format() == VIL_PIXEL_FORMAT_FLOAT) &&
        (last_roi->pixel_format() == VIL_PIXEL_FORMAT_FLOAT)) {
      vil_image_view<float>* ground_view = new vil_image_view<float>();
      vil_image_view<float> first_view(first_roi);
      vil_image_view<float> last_view(last_roi);
      bmdl_classify<float> classifier;
      classifier.set_lidar_data(first_view, last_view);
      classifier.estimate_bare_earth();
      ground_view->deep_copy(classifier.bare_earth());
      ground_roi = ground_view;
    }
    else if ((first_roi->pixel_format() == VIL_PIXEL_FORMAT_DOUBLE) &&
             (last_roi->pixel_format() == VIL_PIXEL_FORMAT_DOUBLE)) {
      vil_image_view<double>* ground_view = new vil_image_view<double>();
      vil_image_view<double> first_view(first_roi);
      vil_image_view<double> last_view(last_roi);
      bmdl_classify<double> classifier;
      classifier.set_lidar_data(first_view, last_view);
      classifier.estimate_bare_earth();
      ground_view->deep_copy(classifier.bare_earth());
      ground_roi = ground_view;
    }
    else {
      vcl_cout << "input images have different bit depths" << vcl_endl;
      return false;
    }
  }
  return true;
}
