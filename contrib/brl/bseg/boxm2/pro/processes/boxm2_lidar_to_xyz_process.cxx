// This is brl/bseg/boxm2/pro/processes/boxm2_lidar_to_xyz_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process to prepare x y z images from a lidar image (single return), the transformation matrix is coded in the image name
// WARNING: Lidar name is assumed to contain the coordinate of the lower left corner of the image; for now N and W directions are hard-coded
//
// \author Ozge C. Ozcanli
// \date Oct 09, 2012

#include <vcl_fstream.h>
#include <boxm2/boxm2_scene.h>

#include <vpgl/file_formats/vpgl_geo_camera.h>
#include <vpgl/vpgl_utm.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vcl_algorithm.h>

#include <vsol/vsol_box_2d_sptr.h>
#include <vsol/vsol_box_2d.h>

#include <brip/brip_roi.h>

// OZGE TODO: IMPLEMENT THIS PROCESS !!!
namespace boxm2_lidar_to_xyz_process_globals
{
  const unsigned n_inputs_ = 2;
  const unsigned n_outputs_ = 3;
}

bool boxm2_lidar_to_xyz_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_lidar_to_xyz_process_globals;

  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "vcl_string";  // tiff LIDAR image

  vcl_vector<vcl_string>  output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";  // x image
  output_types_[1] = "vil_image_view_base_sptr";  // y image
  output_types_[2] = "vil_image_view_base_sptr";  // z image

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


bool boxm2_lidar_to_xyz_process(bprb_func_process& pro)
{
  using namespace boxm2_lidar_to_xyz_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << ": The number of inputs should be " << n_inputs_<< vcl_endl;
    return false;
  }

  boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(0);
  vgl_box_3d<double> scene_bbox = scene->bounding_box();
  vpgl_lvcs_sptr lvcs = new vpgl_lvcs(scene->lvcs());
  double olat, olon, oelev;
  lvcs->get_origin(olat, olon, oelev);

  // get UTM origin of LVCS to check whether image points are in the same zone
  double xx, yy, elev; int lvcs_zone;
  lvcs->get_utm_origin(xx, yy, elev, lvcs_zone);
  vcl_cout << "scene origin lat: " << olat << " lon: " << olon << " utm zone: " << lvcs_zone << vcl_endl;
  vcl_string tiff_fname = pro.get_input<vcl_string>(1);

  vil_image_view_base_sptr img_sptr = vil_load(tiff_fname.c_str());
  vil_image_view<float> img(img_sptr);
  unsigned ni = img.ni(); unsigned nj = img.nj();
  vcl_cout << " tile size: "<< ni << " x " << nj << vcl_endl;
#if 0
  // determine the translation matrix from the image file name and construct a geo camera
  vcl_string name = vul_file::strip_directory(tiff_fname);
  name = name.substr(name.find_first_of('_')+1, name.size());
  vcl_cout << "will determine transformation matrix from the file name: " << name << vcl_endl;
  vcl_string n = name.substr(name.find_first_of('N')+1, name.find_first_of('W'));
  float lon, lat, scale;
  vcl_stringstream str(n); str >> lat;
  n = name.substr(name.find_first_of('W')+1, name.find_first_of('_'));
  vcl_stringstream str2(n); str2 >> lon;
  n = name.substr(name.find_first_of('x')+1, name.find_last_of('.'));
  vcl_stringstream str3(n); str3 >> scale;
  vcl_cout << " lat: " << lat << " lon: " << lon << " WARNING: using same scale for both ni and nj: scale:" << scale << vcl_endl;

  // determine the upper left corner to use a vpgl_geo_cam, subtract from lat
  vcl_cout << "upper left corner in the image is: " << lat+scale << " N " << lon << " W\n"
           << "lower right corner in the image is: " << lat << " N " << lon-scale << " W" << vcl_endl;
  vnl_matrix<double> trans_matrix(4,4,0.0);
  trans_matrix[0][0] = -scale/ni; trans_matrix[1][1] = -scale/nj;
  trans_matrix[0][3] = lon; trans_matrix[1][3] = lat+scale;
  vpgl_geo_camera cam(trans_matrix, lvcs); cam.set_scale_format(true);
#endif
  vpgl_geo_camera *cam;
  vpgl_geo_camera::init_geo_camera(tiff_fname, ni, nj, lvcs, cam);
  
  double lon2, lat2;
  cam->img_to_global(ni, nj, lon2, lat2);
  vpgl_utm utm; double x, y; int zone; utm.transform(lat2, -lon2, x, y, zone);
  vcl_cout << "lower right corner in the image given by geocam is: " << lat2 << " N " << lon2 << " W " << " zone: " << zone << vcl_endl;

  // create x y z images
  vil_image_view<float>* out_img_x = new vil_image_view<float>(ni, nj, 1);
  vil_image_view<float>* out_img_y = new vil_image_view<float>(ni, nj, 1);
  vil_image_view<float>* out_img_z = new vil_image_view<float>(ni, nj, 1);
  out_img_x->fill(0.0f); out_img_y->fill(0.0f);
  out_img_z->fill((float)(scene_bbox.min_z()-10.0));  // local coord system min z

  bool no_overlap = true;
  // iterate over the image and for each pixel, calculate, xyz in the local coordinate system
  for (unsigned i = 0; i < ni; i++)
    for (unsigned j = 0; j < nj; j++) {
      cam->img_to_global(i, j, lon2, lat2);
      // minus lon because it is WEST, WARNING, directions are hard-coded!
      lon2 = -lon2;

      vpgl_utm utm; double x, y; int zone;
      utm.transform(lat2, lon2, x, y, zone);
      if (zone != lvcs_zone)
        continue;
      double lx, ly, lz;
      if (img(i,j) <= 0)
        continue; 
      lvcs->global_to_local(lon2, lat2, img(i,j), vpgl_lvcs::wgs84, lx, ly, lz);
      vgl_point_3d<double> pt(lx, ly, lz);
      if (scene_bbox.contains(pt)) {
        no_overlap = false;
        (*out_img_x)(i,j) = (float)lx;
        (*out_img_y)(i,j) = (float)ly;
        (*out_img_z)(i,j) = (float)lz;
      }
    }

  if (no_overlap) {
    delete out_img_x;
    delete out_img_y;
    delete out_img_z;
    return false;
  }

  pro.set_output_val<vil_image_view_base_sptr>(0, out_img_x);
  pro.set_output_val<vil_image_view_base_sptr>(1, out_img_y);
  pro.set_output_val<vil_image_view_base_sptr>(2, out_img_z);

  return true;
}

