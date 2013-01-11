//:
// \file
// \brief executable to match a depth map to volume using the height map of the scene to place the cameras
// \author Ozge C. Ozcanli
// \date Aug 11, 2012

#include <boxm2/reg/ocl/boxm2_ocl_reg_depth_map_to_vol.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/io/boxm2_stream_scene_cache.h>
#include <boxm2/boxm2_util.h>
#include <bpgl/algo/bpgl_transform_camera.h>
#include <vpgl/vpgl_perspective_camera.h>

#include <vgl/vgl_point_3d.h>
#include <vul/vul_arg.h>
#include <vul/vul_timer.h>
#include <vcl_algorithm.h>
#include <bocl/bocl_manager.h>
#include <bocl/bocl_device.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>

#include <vnl/vnl_vector.h>
#include <vnl/vnl_math.h>
#include <vcl_iostream.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>

#include <bvrml/bvrml_write.h>

int main(int argc,  char** argv)
{
  //init vgui (should choose/determine toolkit)
  vul_arg<vcl_string> cam_file("-cam", "cam filename", "");
  vul_arg<vcl_string> depth_file("-depth", "depth map filename", "");
  vul_arg<vcl_string> scene_file("-scene", "scene filename", "");
  vul_arg<vcl_string> out_file("-out", "output text filename", "");
  vul_arg<vcl_string> x_image("-x", "x image of the world", "");
  vul_arg<vcl_string> y_image("-y", "y image of the world", "");
  vul_arg<vcl_string> z_image("-z", "z image of the world", "");
  vul_arg_parse(argc, argv);

  //create scene
  boxm2_scene_sptr scene = new boxm2_scene(scene_file());
  vpgl_camera_double_sptr cam = boxm2_util::camera_from_file(cam_file());
  vgl_point_3d<double> cam_center;
  if ( vpgl_perspective_camera<double> * pcam = dynamic_cast<vpgl_perspective_camera<double> *> (cam.ptr() ))
  {
    cam_center = pcam->camera_center();
  }
  else {
    vcl_cerr << " Cannot cast cam " << cam_file() << " to perspective!\n";
    return -1;
  }

  vil_image_view_base_sptr img = vil_load(depth_file().c_str());
  vil_image_view<float> * fimg  = dynamic_cast<vil_image_view<float> * >(img.ptr());
  if (!fimg) {
    vcl_cout << "Error: depth image is not float!\n";
    return false;
  }
  vil_image_view_base_sptr x_img = vil_load(x_image().c_str());
  vil_image_view<float> * ximg  = dynamic_cast<vil_image_view<float> * >(x_img.ptr());
  vil_image_view_base_sptr y_img = vil_load(y_image().c_str());
  vil_image_view<float> * yimg  = dynamic_cast<vil_image_view<float> * >(y_img.ptr());
  vil_image_view_base_sptr z_img = vil_load(z_image().c_str());
  vil_image_view<float> * zimg  = dynamic_cast<vil_image_view<float> * >(z_img.ptr());
  if (!ximg || !yimg || !zimg) {
    vcl_cout << "Error: x,y,z images are not float!\n";
    return false;
  }

  vcl_vector<vcl_string> data_types;
  vcl_vector<vcl_string> identifiers;
  data_types.push_back("alpha");
  identifiers.push_back("");
  boxm2_stream_scene_cache cache( scene, data_types,identifiers);
  bocl_manager_child_sptr mgr =bocl_manager_child::instance();
  if (mgr->gpus_.size()==0)
    return -1;

  bocl_device_sptr  device = mgr->gpus_[0];
  vnl_vector<double> x;
  x.set_size(6);

  unsigned margin = 2;
  //unsigned i_start = 84;
  unsigned i_start = 81;
  //unsigned j_start = 59;
  unsigned j_start = 35;
  unsigned i_inc = 100;  // 80 pixels corresponds to 50 m in the x_image
  unsigned j_inc = 100;
  unsigned ni = ximg->ni(); unsigned nj = ximg->nj();
  if (ni <= margin || nj <= margin || yimg->ni() != ni || yimg->nj() != nj || zimg->ni() != ni || zimg->nj() != nj) {
    vcl_cout << "Error: x,y,z images are not valid!\n";
    return false;
  }
  ni -= margin;
  nj -= margin;

  vul_timer t;
  t.mark();
  vcl_ofstream ofs(out_file().c_str());

  //x[0] = (*ximg)(i_start,j_start); x[1] = (*yimg)(i_start,j_start); x[2] = (*zimg)(i_start,j_start);
  //x[0]= cam_center.x(); x[1] = cam_center.y(); x[2] =cam_center.z();
  //x[0] = 4833; x[1] = 62151; x[2] = 380;//x[2] = 410;
  x[0] = -42612.26; x[1] = 49902.335; x[2] = 183;
  x[3] = 0.0; x[4] = 0.0; x[5] = 0.0;
  boxm2_ocl_reg_depth_map_to_vol func(cam,fimg,cache,device,2);
  double mi = -func.f(x);
  vcl_cout << "Initial value at " << x[0] << ", " << x[1] << ", " << x[2] << " is " << mi << vcl_endl;
  ofs << x[0] << ' ' << x[1] << ' ' << mi << '\n';

  double best_x = x[0]; double best_y = x[1]; double best_z = x[2];
  double best_mi = mi;
  for (unsigned i = i_start; i <= ni; i+= i_inc) {
    for ( unsigned j = j_start; j <= nj; j+= j_inc) {
      x[0]=  (*ximg)(i,j);
      x[1]=  (*yimg)(i,j);
      x[2] = (*zimg)(i,j);
      double maxmi = 0.0;
      // angles
      for (unsigned int k = 0; k < 1; k++) {
        x[5] = vnl_math::pi_over_2 * ( double ) k;
        double mi = -func.f(x);
        if ( maxmi < mi )
          maxmi = mi;
      }
      ofs << (*ximg)(i,j) << ' ' << (*yimg)(i,j) << ' ' << maxmi << '\n';
      //ofs << (*ximg)(i,j) << ' ' << (*yimg)(i,j) << ' ' << (*zimg)(i,j) << '\n';
      if (maxmi > best_mi) {
        best_mi = maxmi;
        best_x = x[0]; best_y = x[1]; best_z = x[2];
      }
    }
    vcl_cout<<'.';
  }

  double secs = t.all()/1000.0;
  vcl_cout<<"Time "<< secs << " secs " << secs/60.0 << " mins.\n"
          << "Best mi: " << best_mi <<" at x: " << best_x << " y: " << best_y << " z: " << best_z << vcl_endl;

  ofs.close();
  return 0;
}
