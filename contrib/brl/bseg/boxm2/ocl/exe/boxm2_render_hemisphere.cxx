#include <bocl/bocl_cl.h>
#include <vcl_sstream.h>

#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vsph/vsph_view_sphere.h>
#include <vpgl/vsph/vsph_view_point.h>
#include <vil/vil_convert.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vil/vil_math.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>

//executable args
#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_timer.h>

//vbl
#include <vbl/vbl_array_2d.h>

//boxm2 scene stuff
#include <boxm2/io/boxm2_nn_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_util.h>
#include <boxm2/basic/boxm2_block_id.h>
#include <boxm2/ocl/boxm2_opencl_processor.h>
#include <boxm2/ocl/pro/boxm2_opencl_render_process.h>

//brdb stuff
#include <brdb/brdb_value.h>

int main(int argc,  char** argv)
{
  vcl_cout<<"Boxm2 Hemisphere"<<vcl_endl;
  vul_arg<vcl_string> scene_file("-scene", "scene filename", "");
  vul_arg<vcl_string> img("-img", "output image directory", "");
  vul_arg<unsigned> ni("-ni", "Width of image", 1280);
  vul_arg<unsigned> nj("-nj", "Height of image", 720);
  vul_arg<unsigned> num_az("-num_az", "Number of views along azimuth", 36); 
  vul_arg<unsigned> num_in("-num_in", "Number of views along 90 degree incline", 5); 
  vul_arg<double> radius("-radius", "Distance from center of bounding box", 3.0); 
  vul_arg_parse(argc, argv);

  //create scene
  boxm2_scene_sptr scene = new boxm2_scene(scene_file());
  brdb_value_sptr brdb_scene = new brdb_value_t<boxm2_scene_sptr>(scene);

  //get relevant blocks
  boxm2_nn_cache cache( scene.ptr() );
  
  //initialize gpu pro / manager
  boxm2_opencl_processor* gpu_pro = boxm2_opencl_processor::instance();
  gpu_pro->set_scene(scene.ptr());
  gpu_pro->set_cpu_cache(&cache);
  gpu_pro->init();

  //initialize the GPU render process
  boxm2_opencl_render_process gpu_render;
  gpu_render.init_kernel(&gpu_pro->context(), &gpu_pro->devices()[0]);

  //create output image buffer
  vil_image_view<unsigned int>* expimg = new vil_image_view<unsigned int>(ni(), nj());
  expimg->fill(0);
  vil_image_view_base_sptr expimg_sptr(expimg);// = new vil_image_view<unsigned int>(ni(), nj());
  brdb_value_sptr brdb_expimg = new brdb_value_t<vil_image_view_base_sptr>(expimg_sptr);

  //create vis image buffer
  vil_image_view<float>* vis_img = new vil_image_view<float>(ni(), nj());
  vis_img->fill(1.0f);
  brdb_value_sptr brdb_vis = new brdb_value_t<vil_image_view_base_sptr>(vis_img);


  /////////////////////////////////////////////////////////////////////////////
  // CREATE VIEWSPHERE around the bounding box
  /////////////////////////////////////////////////////////////////////////////
  vcl_vector<vil_image_view<vxl_byte>* > images; 

  vsph_view_sphere<vsph_view_point<vcl_string> > sphere(scene->bounding_box(), radius());
  sphere.add_uniform_views(vnl_math::pi/3, vnl_math::pi/18.0, ni(), nj());
  vcl_cout<<"Number of views to render: "<<sphere.size()<<vcl_endl;

  vsph_view_sphere<vsph_view_point<vcl_string> >::iterator iter; int img_index=0;
  for(iter = sphere.begin(); iter != sphere.end(); ++iter, ++img_index)
  {
    vcl_cout<<"view: [id "<<iter->first<<"]"<<vcl_endl;
    
    vsph_view_point<vcl_string>& view = iter->second; 
    vpgl_camera_double_sptr cam_sptr = view.camera(); 
    vpgl_perspective_camera<double>* cam = static_cast<vpgl_perspective_camera<double>* >(cam_sptr.ptr()); 
    brdb_value_sptr brdb_cam = new brdb_value_t<vpgl_camera_double_sptr>(cam_sptr);
    vpgl_calibration_matrix<double> mat = cam->get_calibration();
    mat.set_focal_length(mat.focal_length()*2.0); 
    cam->set_calibration(mat); 
    
    //render scene
    vcl_vector<brdb_value_sptr> input;
    input.push_back(brdb_scene);
    input.push_back(brdb_cam);
    input.push_back(brdb_expimg);
    input.push_back(brdb_vis);
    vcl_vector<brdb_value_sptr> output;
    expimg->fill(0);
    vis_img->fill(1.0f);
    gpu_pro->run(&gpu_render, input, output);
    gpu_pro->finish();
      
    vil_image_view<unsigned int>* expimg_view = static_cast<vil_image_view<unsigned int>* >(expimg_sptr.ptr());
    vil_image_view<vxl_byte>* byte_img = new vil_image_view<vxl_byte>(ni(), nj());
    for (unsigned int i=0; i<ni(); ++i)
      for (unsigned int j=0; j<nj(); ++j)
        (*byte_img)(i,j) =  static_cast<vxl_byte>( (*expimg_view)(i,j) );   //just grab the first byte (all foura r the same)
        
    //save image
    vcl_stringstream stream;
    stream<<img()<<"/expimg_"<<img_index<<".png"; 
    vcl_string* filepath = new vcl_string();
    (*filepath) = stream.str();     
    view.set_metadata(filepath);
    vil_save( *byte_img, stream.str().c_str() );
  }
  
  //Save sphere 
  vcl_cout<<"SAVING SPHERE TO SPHERE PATH"<<vcl_endl;
  vcl_string sphere_path = img() + "/sphere.bin";
  vsl_b_ofstream sphere_os(sphere_path);
  if (!sphere_os) {
    vcl_cout<<"cannot open "<<sphere_path<<" for writing\n";
    return false;
  }
  vsl_b_write(sphere_os, sphere); 
  sphere_os.close();

  return 0;
}

