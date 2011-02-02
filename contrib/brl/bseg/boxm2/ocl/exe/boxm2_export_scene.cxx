#include <bocl/bocl_cl.h>
#include <vcl_sstream.h>
#include <vcl_where_root_dir.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>

#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vsph/vsph_view_sphere.h>
#include <vpgl/vsph/vsph_view_point.h>
#include <vil/vil_convert.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vil/vil_math.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>

//vul args
#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_timer.h>

//vbl
#include <vbl/vbl_array_2d.h>

//boxm2 scene stuff
#include <boxm2/io/boxm2_lru_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_util.h>
#include <boxm2/basic/boxm2_block_id.h>
#include <boxm2/ocl/boxm2_opencl_processor.h>
#include <boxm2/ocl/pro/boxm2_opencl_render_process.h>

//brdb stuff
#include <brdb/brdb_value.h>

//: Boxm2_Export_Scene executable will create a small, portable, pre rendered
// scene that can be viewed on many devices.  Currently the output is a folder
// with the following structure
//   - Base directory (dir name given by input -dir)
//      - index.html  (open this file in a browser to see the model\
//      - js directory (contains all jQuery and Reel javascript files)
//      - img directory (contains a rendering of stills 
// 
//   - Pre Rendered Stills 

int main(int argc,  char** argv)
{
  vcl_cout<<"Boxm2 Hemisphere"<<vcl_endl;
  vul_arg<vcl_string> scene_file("-scene", "scene filename", "");
  vul_arg<vcl_string> dir("-dir", "output image directory", "");
  vul_arg<unsigned> ni("-ni", "Width of image", 640);
  vul_arg<unsigned> nj("-nj", "Height of image", 480);
  vul_arg<unsigned> num_az("-num_az", "Number of views along azimuth", 36); 
  vul_arg<unsigned> num_in("-num_in", "Number of views along 90 degree incline", 3); 
  vul_arg<double> radius("-radius", "Distance from center of bounding box", 5.0); 
  vul_arg_parse(argc, argv);

  //////////////////////////////////////////////////////////////////////////////
  //Set Up Directory Structure
  //////////////////////////////////////////////////////////////////////////////
  //see if directory exists 
  if( vul_file::exists(dir()) && vul_file::is_directory(dir()) ) {
    vcl_cout<<"Directory "<<dir()<<" exists - overwriting it."<<vcl_endl;
  }
  else {
    vul_file::make_directory_path(dir());
    //vcl_cout<<"Couldn't make directory at "<<dir()<<vcl_endl;
    //return -1;
  }
  
  //see if img folder exists
  vcl_string imgdir = dir() + "/img/"; 
  if( vul_file::exists(imgdir) && vul_file::is_directory(imgdir) ){
    vul_file::delete_file_glob(imgdir+"*"); 
  }
  else {
    vul_file::make_directory_path(imgdir);
    //vcl_cout<<"Couldn't make img directory at "<<dir()<<vcl_endl;
    //return -1;
  }
  
  //see if JS folder exists
  vcl_string jsdir = dir() + "/js/"; 
  if( vul_file::exists(jsdir) && vul_file::is_directory(jsdir) ){
    vul_file::delete_file_glob(jsdir+"*"); 
  }
  else {  
    vul_file::make_directory_path(jsdir);
    //vcl_cout<<"Couldn't make js directory at "<<dir()<<vcl_endl;
    //return -1;
  }

  //copy JS files into JS folder
  vcl_string aux_dir = vcl_string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/boxm2/ocl/exe/aux/"; 
  
  //copy html and JS files
  vcl_string index_name = aux_dir + "index.html";
  vcl_string dest_name = dir() + "/index.html"; 
  boxm2_util::copy_file(index_name, dest_name); 
  
  vcl_vector<vcl_string> js_files; 
  js_files.push_back("/js/jquery.min.js"); 
  js_files.push_back("/js/jquery.cookie-min.js"); 
  js_files.push_back("/js/jquery.disabletextselect-min.js"); 
  js_files.push_back("/js/jquery.mousewheel-min.js"); 
  js_files.push_back("/js/jquery.reel-min.js"); 
  js_files.push_back("/js/js.js"); 
  
  //copy files to dir() + js
  for(int i=0; i<js_files.size(); ++i) 
    boxm2_util::copy_file(aux_dir + js_files[i], dir() + js_files[i]); 
  
  //////////////////////////////////////////////////////////////////////////////
  // Now Render Scene Images
  //////////////////////////////////////////////////////////////////////////////
  //create scene
  boxm2_scene_sptr scene = new boxm2_scene(scene_file());
  brdb_value_sptr brdb_scene = new brdb_value_t<boxm2_scene_sptr>(scene);

  //get relevant blocks
  boxm2_lru_cache cache( scene.ptr() );
  
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

  //////////////////////////////////////////////////////////////////////////////
  // Else IF GRID
  //////////////////////////////////////////////////////////////////////////////
  //set up a view sphere, use find closest for closest neighbors
  vsph_view_sphere<vsph_view_point<vcl_string> > sphere(scene->bounding_box(), radius());
  sphere.add_uniform_views(vnl_math::pi/3, vnl_math::pi/18.0, ni(), nj());
  vcl_cout<<"Number of views to render: "<<sphere.size()<<vcl_endl;
  
  //rendered array of views
  vbl_array_2d<vil_image_view<vxl_byte>* > imgs(num_in(), num_az()); 
  double az_incr = 2.0*vnl_math::pi/num_az(); 
  double el_incr = vnl_math::pi/2.0/num_in();
  for(int az_i = 0; az_i < num_az(); ++az_i)
  {
    double az = 2.0*vnl_math::pi - az_i * az_incr; 
    for(int el_i = 0.0; el_i < num_in(); ++el_i)
    {
      double el = vnl_math::pi/2.0 - el_i * el_incr; 
      
      //convert to cartesian (as method is only in cartesian for some reason)
      vsph_sph_point_3d curr_point(radius(), el, az); 
      vgl_point_3d<double> cart_point = sphere.cart_coord(curr_point);
      int uid; double dist;
      vsph_view_point<vcl_string> view = sphere.find_closest(cart_point, uid, dist);
      vpgl_camera_double_sptr cam_sptr = view.camera(); 
      brdb_value_sptr brdb_cam = new brdb_value_t<vpgl_camera_double_sptr>(cam_sptr);

      //set focal length and image size for camera
      vpgl_perspective_camera<double>* cam = static_cast<vpgl_perspective_camera<double>* >(cam_sptr.ptr()); 
      vpgl_calibration_matrix<double> mat = cam->get_calibration();
      mat.set_focal_length(mat.focal_length()); 
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
            
      //save as jpeg
      vcl_stringstream pngstream, jpgstream;
      jpgstream<<imgdir<<"/scene_"<<el_i<<"_"<<az_i<<".jpg";
      vil_save( *byte_img, jpgstream.str().c_str() );
      
      //and store for whatever reason
      //imgs(el_i, az_i) = byte_img; 
    }
  } 
  
  return 0;
}

