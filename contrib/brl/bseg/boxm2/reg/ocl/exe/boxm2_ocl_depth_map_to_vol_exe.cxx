//:
// \file
// \brief executable to match a depth map to volume
// \author Vishal Jain
// \date June 08, 2012

#include <boxm2/reg/ocl/boxm2_ocl_reg_depth_map_to_vol.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/io/boxm2_stream_scene_cache.h>
#include <boxm2/boxm2_util.h>
#include <bpgl/algo/bpgl_transform_camera.h>
#include <vpgl/vpgl_perspective_camera.h>

#include <vgl/vgl_vector_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vul/vul_arg.h>
#include <vul/vul_timer.h>
#include <vcl_algorithm.h>
#include <bocl/bocl_manager.h>
#include <bocl/bocl_device.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>

#include <vnl/vnl_vector.h>
#include <vnl/vnl_math.h>

#include <vcl_iostream.h>
#include <vil/vil_save.h>
#include <vil/vil_math.h>
#include <vil/vil_convert.h>

#include <bvrml/bvrml_write.h>

int main(int argc,  char** argv)
{
  //init vgui (should choose/determine toolkit)
  vul_arg<vcl_string> cam_file("-cam", "cam filename", "");
  vul_arg<vcl_string> depth_file("-depth", "depth map filename", "");
  vul_arg<vcl_string> scene_file("-scene", "scene filename", "");
  vul_arg<vcl_string> out_file("-out", "output image filename", "");
  vul_arg_parse(argc, argv);

  //create scene
  boxm2_scene_sptr scene = new boxm2_scene(scene_file());
  vpgl_camera_double_sptr cam = boxm2_util::camera_from_file(cam_file());
  vgl_point_3d<double> cam_center;
  if ( vpgl_perspective_camera<double> * pcam = dynamic_cast<vpgl_perspective_camera<double> *> (cam.ptr()) )
  {
    cam_center = pcam->camera_center();
  }
  else {
    vcl_cerr << " Cannot cast cam " << cam_file() << " to perspective!\n";
    return -1;
  }

  vil_image_view_base_sptr img = vil_load(depth_file().c_str());
  if (vil_image_view<float> * fimg  = dynamic_cast<vil_image_view<float> * >(img.ptr()))
  {
    vcl_vector<vcl_string> data_types;
    vcl_vector<vcl_string> identifiers;
    data_types.push_back("alpha");
    //data_types.push_back("aux2");
    identifiers.push_back("");
    //identifiers.push_back("vis");
    boxm2_stream_scene_cache cache( scene, data_types,identifiers);
    bocl_manager_child_sptr mgr =bocl_manager_child::instance();
    if (mgr->gpus_.size()==0)
      return -1;

    bocl_device_sptr  device = mgr->gpus_[0];
    vnl_vector<double> x;
    x.set_size(6);
    x[0]= cam_center.x(); x[1] = cam_center.y() ; x[2] =cam_center.z();
    x[3]= 0.0; x[4] = 0.0 ; x[5] =0.0;
    boxm2_ocl_reg_depth_map_to_vol func(cam,fimg,cache,device,2);
    vul_timer t;
    t.mark();
    vil_image_view<float> output(49,31);

    for ( double yinc = -200 ; yinc<=1300; yinc+=50)
    {
      for ( double xinc = -1200 ; xinc<=1200; xinc+=50)
      {
          x[0]=  cam_center.x() + xinc;
          x[1]=  cam_center.y() + yinc;
          double maxmi = 0.0;
          // angles
          for (unsigned int k = 0; k < 1; k++)
          {
            x[5] = vnl_math::pi/2 * ( double ) k ;
            double mi = -func.f(x);
            if ( maxmi < mi )
              maxmi = mi;
          }
          output((int)((xinc+1200.0)/50.0),(int)((yinc+200.0)/50.0)) = (float)maxmi;
      }
      vcl_cout<<".";
    }
    double secs = t.all()/1000.0;
    vcl_cout<<"Time "<< secs << " secs " << secs/60.0 << " mins."<<vcl_endl;
    float min, max;
    vil_math_value_range(output, min, max);
    vcl_cout << " min mutual info in the output: " << min << " max mi: " << max << vcl_endl;
    vil_save(output,out_file().c_str());
    vil_math_scale_values(output, 255.0f/max);
    vcl_stringstream ss; ss << out_file().c_str() << "_stretched.png";
    vil_image_view<vxl_byte> out(output.ni(), output.nj());
    vil_convert_cast(output, out);
    vil_save(out,ss.str().c_str());
  }
#if 0
  vnl_powell powell(&func);
  vnl_vector<double> x(6,0.0);
  powell.set_x_tolerance(1e-1);
  powell.set_max_function_evals(10);
  vul_timer t ;
  t.mark();
  powell.minimize(x);
  vcl_cout<<"Initial Mutual Info "<<func.mutual_info(vgl_rotation_3d<double>(),vgl_vector_3d<double>())<<'\n'
          <<"Final Mutual Info "<<func.mutual_info(r,vgl_vector_3d<double>(x[0],x[1],x[2]))<<vcl_endl;
#endif
  return 0;
}
