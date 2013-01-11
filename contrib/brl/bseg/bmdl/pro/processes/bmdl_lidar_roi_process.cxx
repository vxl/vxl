#include <vcl_cstring.h>

#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vsol/vsol_box_2d.h>

#include <vil/vil_load.h>
#include <vil/vil_image_resource.h>
#include <vil/file_formats/vil_tiff.h>

#include <bprb/bprb_parameters.h>
#include <bprb/bprb_func_process.h>

#include <vpgl/vpgl_lvcs.h>
#include <vpgl/file_formats/vpgl_geo_camera.h>

#include <brip/brip_roi.h>
#include <brdb/brdb_value.h>
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

bool compute_ground(vil_image_resource_sptr ground,
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


bool lidar_roi(unsigned type,  //0 for geo coordinates, 1 for image coord
               vil_image_resource_sptr lidar_first,
               vil_image_resource_sptr lidar_last,
               vil_image_resource_sptr ground,
               float min_lat, float min_lon,
               float max_lat, float max_lon,
               vil_image_view_base_sptr& first_roi,
               vil_image_view_base_sptr& last_roi,
               vil_image_view_base_sptr& ground_roi,
               vpgl_geo_camera*& camera)
{
  // the file should be at least a tiff (better, geotiff)
  if (vcl_strcmp(lidar_first->file_format(), "tiff") != 0 &&
      vcl_strcmp(lidar_last->file_format(),"tiff") != 0) {
    vcl_cout << "bmdl_lidar_roi_process::lidar_roi -- The lidar images should be a TIFF!\n";
    return false;
  }

#if HAS_GEOTIFF
  vil_tiff_image* tiff_first = static_cast<vil_tiff_image*> (lidar_first.ptr());
  vil_tiff_image* tiff_last  = static_cast<vil_tiff_image*> (lidar_last.ptr());

  if (vpgl_geo_camera::init_geo_camera(tiff_first, 0, camera))
  {
    vgl_box_2d<double> roi_box;

    if (type == 0)    // geographic coordinates
    {
      // backproject the 3D world coordinates on the image
      vgl_point_3d<double> min_pos(min_lon, min_lat, 0);
      vgl_point_3d<double> max_pos(max_lon, max_lat, 30);
      vgl_box_3d<double> world(min_pos, max_pos);
      vcl_vector<vgl_point_3d<double> > corners = corners_of_box_3d<double>(world);
      for (unsigned i=0; i<corners.size(); i++) {
        double x = corners[i].x();
        double y = corners[i].y();
        double z = corners[i].z();
        double lx, ly, lz;
        camera->lvcs()->global_to_local(x, y, z,vpgl_lvcs::wgs84, lx, ly, lz);
        double u,v;
        camera->project(lx,ly,lz,u,v);
        vgl_point_2d<double> p(u,v);
        roi_box.add(p);
      }
    }
    else if (type == 1) {
      roi_box.add(vgl_point_2d<double> (min_lat, min_lon));
      roi_box.add(vgl_point_2d<double> (max_lat, max_lon));
    }

    brip_roi broi(tiff_first->ni(), tiff_first->nj());
    vsol_box_2d_sptr bb = new vsol_box_2d();
    bb->add_point(roi_box.min_x(), roi_box.min_y());
    bb->add_point(roi_box.max_x(), roi_box.max_y());

    bb = broi.clip_to_image_bounds(bb);
    //vcl_cout << "Cut out area------>" << vcl_endl;
    //vcl_cout << *bb << vcl_endl;
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
    }
    else {   // crop the given one
      ground_roi = ground->get_copy_view((unsigned int)bb->get_min_x(),
                                         (unsigned int)bb->width(),
                                         (unsigned int)bb->get_min_y(),
                                         (unsigned int)bb->height());
    }

    //add the translation to the camera
    double ox = bb->get_min_x();
    double oy = bb->get_min_y();
    double elev = 0;
    if (ground_roi->pixel_format() == VIL_PIXEL_FORMAT_FLOAT) {
      vil_image_view<float> ground_view(ground_roi);
      elev = ground_view(0,0);
    }
    else if (ground_roi->pixel_format() == VIL_PIXEL_FORMAT_DOUBLE) {
      vil_image_view<double> ground_view(ground_roi);
      elev = ground_view(0,0);
    }
    camera->translate(ox, oy, elev);

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

bool bmdl_lidar_roi_process(bprb_func_process& pro)
{
  if (pro.n_inputs()< 8) {
    vcl_cout << "lidar_roi_process: The input number should be 8" << vcl_endl;
    return false;
  }

  unsigned int i=0;
  vcl_string first = pro.get_input<vcl_string>(i++);
  vcl_string last = pro.get_input<vcl_string>(i++);
  vcl_string ground = pro.get_input<vcl_string>(i++);
  float min_lat = pro.get_input<float>(i++);
  float min_lon = pro.get_input<float>(i++);
  float max_lat = pro.get_input<float>(i++);
  float max_lon = pro.get_input<float>(i++);
  unsigned type = pro.get_input<unsigned>(i++);

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
  if (!lidar_roi(type, first_ret, last_ret, ground_img,
    min_lat, min_lon, max_lat, max_lon, first_roi, last_roi, ground_roi, lidar_cam)) {
    vcl_cout << "bmdl_lidar_roi_process -- The process has failed!\n";
    return false;
  }

  unsigned j=0;
  // store image output (first return roi)
  pro.set_output_val<vil_image_view_base_sptr>(j++, first_roi);

  // store image output (last return roi)
  pro.set_output_val<vil_image_view_base_sptr>(j++,last_roi);

  // store image output (ground roi)
  pro.set_output_val<vil_image_view_base_sptr>(j++,ground_roi);

  // store the camera (camera of first return is sufficient)
  pro.set_output_val<vpgl_camera_double_sptr >(j++, lidar_cam);

  return true;
}

bool bmdl_lidar_roi_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vcl_string");
  input_types.push_back("vcl_string");
  input_types.push_back("vcl_string");
  input_types.push_back("float");
  input_types.push_back("float");
  input_types.push_back("float");
  input_types.push_back("float");
  input_types.push_back("unsigned");
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  vcl_vector<vcl_string> output_types;
  output_types.push_back("vil_image_view_base_sptr"); // label image
  output_types.push_back("vil_image_view_base_sptr"); // height image
  output_types.push_back("vil_image_view_base_sptr"); // ground roi
  output_types.push_back("vpgl_camera_double_sptr");  // lvcs
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;

  return true;
}
