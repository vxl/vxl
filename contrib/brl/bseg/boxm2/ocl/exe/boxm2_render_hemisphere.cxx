#include <iostream>
#include <sstream>
#include <bocl/bocl_cl.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vpgl/vpgl_perspective_camera.h>
#include <vsph/vsph_view_sphere.h>
#include <vsph/vsph_view_point.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>

//executable args
#include <vul/vul_arg.h>

//vbl
#include <vbl/vbl_array_2d.h>

//boxm2 scene stuff
#include <boxm2/io/boxm2_lru_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_util.h>
#include <boxm2/basic/boxm2_block_id.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <bocl/bocl_manager.h>

#include <bpro/core/vil_pro/vil_register.h>
//brdb stuff
#include <brdb/brdb_value.h>
#include <brdb/brdb_selection.h>

#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_parameters.h>
#include <bprb/bprb_macros.h>
#include <bprb/bprb_func_process.h>

int main(int argc,  char** argv)
{
  std::cout<<"Boxm2 Hemisphere"<<std::endl;
  vul_arg<std::string> scene_file("-scene", "scene filename", "");
  vul_arg<std::string> img("-img", "output image directory", "");
  vul_arg<unsigned> ni("-ni", "Width of image", 640);
  vul_arg<unsigned> nj("-nj", "Height of image", 480);
  vul_arg<unsigned> num_az("-num_az", "Number of views along azimuth", 36);
  vul_arg<unsigned> num_in("-num_in", "Number of views along 90 degree incline", 5);
  vul_arg<double> radius("-radius", "Distance from center of bounding box", 5.0);
  vul_arg<bool>   grid("-grid", "Specify grid if saving images in row/col format", false);
  vul_arg<bool>   stitch("-stitch", "stitches the images together into one large image", false);
  vul_arg_parse(argc, argv);

  DECLARE_FUNC_CONS(boxm2_ocl_render_expected_image_process);
  DECLARE_FUNC_CONS(boxm2_ocl_render_expected_color_process);
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_render_expected_image_process, "boxm2OclRenderExpectedImageProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_render_expected_color_process, "boxm2OclRenderExpectedColorProcess");

  REGISTER_DATATYPE(boxm2_opencl_cache_sptr);
  REGISTER_DATATYPE(boxm2_scene_sptr);
  REGISTER_DATATYPE(bocl_mem_sptr);
  vil_register::register_datatype();
  //REGISTER_DATATYPE(vil_image_view_base_sptr);

  //create scene
  boxm2_scene_sptr scene = new boxm2_scene(scene_file());

  bocl_manager_child &mgr =bocl_manager_child::instance();
  bocl_device_sptr device = mgr.gpus_[0];

  //create cache, grab singleton instance
  boxm2_lru_cache::create(scene);
  boxm2_opencl_cache_sptr opencl_cache=new boxm2_opencl_cache( device );

  brdb_value_sptr brdb_device = new brdb_value_t<bocl_device_sptr>(device);
  brdb_value_sptr brdb_scene = new brdb_value_t<boxm2_scene_sptr>(scene);
  brdb_value_sptr brdb_opencl_cache = new brdb_value_t<boxm2_opencl_cache_sptr>(opencl_cache);
  brdb_value_sptr brdb_ni = new brdb_value_t<unsigned>(ni());
  brdb_value_sptr brdb_nj = new brdb_value_t<unsigned>(nj());

  /////////////////////////////////////////////////////////////////////////////
  // CREATE VIEWSPHERE around the bounding box
  /////////////////////////////////////////////////////////////////////////////
  if ( !grid() )
  {
    std::vector<vil_image_view<vxl_byte>* > images;

    vsph_view_sphere<vsph_view_point<std::string> > sphere(scene->bounding_box(), radius());
    sphere.add_uniform_views(vnl_math::pi/3, vnl_math::pi/18.0, ni(), nj());
    std::cout<<"Number of views to render: "<<sphere.size()<<std::endl;

    vsph_view_sphere<vsph_view_point<std::string> >::iterator iter; int img_index=0;
    for (iter = sphere.begin(); iter != sphere.end(); ++iter, ++img_index)
    {
      std::cout<<"view: [id "<<iter->first<<']'<<std::endl;

      vsph_view_point<std::string>& view = iter->second;
      vpgl_camera_double_sptr cam_sptr = view.camera();
      auto* cam = static_cast<vpgl_perspective_camera<double>* >(cam_sptr.ptr());
      brdb_value_sptr brdb_cam = new brdb_value_t<vpgl_camera_double_sptr>(cam_sptr);
      vpgl_calibration_matrix<double> mat = cam->get_calibration();
      mat.set_focal_length(mat.focal_length()/2);
      cam->set_calibration(mat);

      //if scene has RGB data type, use color render process
      bool good;
      if (scene->has_data_type(boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix()) ) {
        good = bprb_batch_process_manager::instance()->init_process("boxm2OclRenderExpectedColorProcess");
      }
      else {
        good = bprb_batch_process_manager::instance()->init_process("boxm2OclRenderExpectedImageProcess");
      }
      if ( !good ) {
        std::cout << "ERROR: couldn't start color render process: " << __FILE__ << __LINE__ << std::endl;
        return -1;
      }
      //set process args and run process
      good = good
          && bprb_batch_process_manager::instance()->set_input(0, brdb_device) // device
          && bprb_batch_process_manager::instance()->set_input(1, brdb_scene)  //  scene
          && bprb_batch_process_manager::instance()->set_input(2, brdb_opencl_cache)
          && bprb_batch_process_manager::instance()->set_input(3, brdb_cam)    // camera
          && bprb_batch_process_manager::instance()->set_input(4, brdb_ni)     // ni for rendered image
          && bprb_batch_process_manager::instance()->set_input(5, brdb_nj)     // nj for rendered image
          && bprb_batch_process_manager::instance()->run_process();
      if ( !good ) {
        std::cout << "ERROR: couldn't set process args: " << __FILE__ << __LINE__ << std::endl;
        return -1;
      }

      unsigned int img_id=0;
      good = good && bprb_batch_process_manager::instance()->commit_output(0, img_id);
      if ( !good ) {
        std::cout << "ERROR: couldn't commit output: " << __FILE__ << __LINE__ << std::endl;
        return -1;
      }
      brdb_query_aptr Q = brdb_query_comp_new("id", brdb_query::EQ, img_id);
      brdb_selection_sptr S = DATABASE->select("vil_image_view_base_sptr_data", std::move(Q));
      if (S->size()!=1) {
        std::cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
                 << " no selections\n";
      }

      brdb_value_sptr value;
      if (!S->get_value(std::string("value"), value)) {
        std::cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
                 << " didn't get value\n";
      }

      vil_image_view_base_sptr outimg=value->val<vil_image_view_base_sptr>();

      auto* expimg_view = static_cast<vil_image_view<float>* >(outimg.ptr());
      auto* byte_img = new vil_image_view<vxl_byte>(ni(), nj());
      for (unsigned int i=0; i<ni(); ++i)
        for (unsigned int j=0; j<nj(); ++j)
          (*byte_img)(i,j) =  (unsigned char)((*expimg_view)(i,j) *255.0f);   //just grab the first byte (all foura r the same)

      //save image
      std::stringstream stream, tfstream, jpgstream;
      stream<<img()<<"/pimg_"<<img_index<<".png";
      tfstream<<img()<<"/timg_"<<img_index<<".tiff";
      jpgstream<<img()<<"/jimg_"<<img_index<<".jpeg";
      auto* filepath = new std::string();
      (*filepath) = stream.str();
      view.set_metadata(filepath);
      vil_save( *byte_img, stream.str().c_str() );

      //save as tiff
      vil_save( *byte_img, tfstream.str().c_str() );

      //save as jpeg
      vil_save( *byte_img, jpgstream.str().c_str() );
    }

    //Save sphere
    std::cout<<"SAVING SPHERE TO SPHERE PATH"<<std::endl;
    std::string sphere_path = img() + "/sphere.bin";
    vsl_b_ofstream sphere_os(sphere_path);
    if (!sphere_os) {
      std::cout<<"cannot open "<<sphere_path<<" for writing\n";
      return -1;
    }
    vsl_b_write(sphere_os, sphere);
    sphere_os.close();
  }

  //////////////////////////////////////////////////////////////////////////////
  // Else IF GRID
  //////////////////////////////////////////////////////////////////////////////
  else
  {
    //set up a view sphere, use find closest for closest neighbors
    vsph_view_sphere<vsph_view_point<std::string> > sphere(scene->bounding_box(), radius());
    sphere.add_uniform_views(vnl_math::pi/3, vnl_math::pi/18.0, ni(), nj());
    std::cout<<"Number of views to render: "<<sphere.size()<<std::endl;

    //rendered array of views
    vbl_array_2d<vil_image_view<vxl_byte>* > imgs(num_in(), num_az());
    double az_incr = vnl_math::twopi/num_az();
    double el_incr = vnl_math::pi/2.0/num_in();
    for (unsigned int az_i = 0; az_i < num_az(); ++az_i)
    {
      double az = vnl_math::twopi - az_i * az_incr;
      for (unsigned int el_i = 0.0; el_i < num_in(); ++el_i)
      {
        double el = vnl_math::pi/2.0 - el_i * el_incr;

        //convert to cartesian (as method is only in cartesian for some reason)
        vsph_sph_point_3d curr_point(radius(), el, az);
        vgl_point_3d<double> cart_point = sphere.cart_coord(curr_point);
        int uid; double dist;
        vsph_view_point<std::string> view = sphere.find_closest(cart_point, uid, dist);
        vpgl_camera_double_sptr cam_sptr = view.camera();
        brdb_value_sptr brdb_cam = new brdb_value_t<vpgl_camera_double_sptr>(cam_sptr);

        //set focal length and image size for camera
        auto* cam = static_cast<vpgl_perspective_camera<double>* >(cam_sptr.ptr());
        vpgl_calibration_matrix<double> mat = cam->get_calibration();
        mat.set_focal_length(mat.focal_length());
        cam->set_calibration(mat);

        //if scene has RGB data type, use color render process
        bool good;
        if (scene->has_data_type(boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix()) ) {
          good = bprb_batch_process_manager::instance()->init_process("boxm2OclRenderExpectedColorProcess");
        }
        else {
          good = bprb_batch_process_manager::instance()->init_process("boxm2OclRenderExpectedImageProcess");
        }
        if ( !good ) {
          std::cout << "ERROR: couldn't start color render process: " << __FILE__ << __LINE__ << std::endl;
          return -1;
        }

        //set process args and run process
        good = good
            && bprb_batch_process_manager::instance()->set_input(0, brdb_device) // device
            && bprb_batch_process_manager::instance()->set_input(1, brdb_scene)  //  scene
            && bprb_batch_process_manager::instance()->set_input(2, brdb_opencl_cache)
            && bprb_batch_process_manager::instance()->set_input(3, brdb_cam)    // camera
            && bprb_batch_process_manager::instance()->set_input(4, brdb_ni)     // ni for rendered image
            && bprb_batch_process_manager::instance()->set_input(5, brdb_nj)     // nj for rendered image
            && bprb_batch_process_manager::instance()->run_process();
        if ( !good ) {
          std::cout << "ERROR: couldn't set process args: " << __FILE__ << __LINE__ << std::endl;
          return -1;
        }

        unsigned int img_id=0;
        good = good && bprb_batch_process_manager::instance()->commit_output(0, img_id);
        if ( !good ) {
          std::cout << "ERROR: couldn't commit output: " << __FILE__ << __LINE__ << std::endl;
          return -1;
        }
        brdb_query_aptr Q = brdb_query_comp_new("id", brdb_query::EQ, img_id);
        brdb_selection_sptr S = DATABASE->select("vil_image_view_base_sptr_data", std::move(Q));
        if (S->size()!=1) {
          std::cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
                   << " no selections\n";
        }

        brdb_value_sptr value;
        if (!S->get_value(std::string("value"), value)) {
          std::cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
                   << " didn't get value\n";
        }

        vil_image_view_base_sptr outimg=value->val<vil_image_view_base_sptr>();

        auto* expimg_view = static_cast<vil_image_view<float>* >(outimg.ptr());
        auto* byte_img = new vil_image_view<vxl_byte>(ni(), nj());
        for (unsigned int i=0; i<ni(); ++i)
          for (unsigned int j=0; j<nj(); ++j)
            (*byte_img)(i,j) =  (unsigned char)((*expimg_view)(i,j) *255.0f);   //just grab the first byte (all foura r the same)

        //save as jpeg
        std::stringstream pngstream, jpgstream;
        jpgstream<<img()<<"/jimg_"<<el_i<<'_'<<az_i<<".jpeg";
        vil_save( *byte_img, jpgstream.str().c_str() );
#if 0
        //save as png
        pngstream<<img()<<"/pimg_"<<el_i<<'_'<<az_i<<".png";
        vil_save( *byte_img, pngstream.str().c_str() );
#endif
        //and store for whatever reason
        imgs(el_i, az_i) = byte_img;
      }
    }

    //if stitch is specified, also save a big image
    if (stitch()) {
      //construct a humungous image
      auto* stitched = new vil_image_view<vxl_byte>(ni() * num_az(), nj() * num_in());
      for (unsigned int row = 0; row < num_in(); ++row) {
        for (unsigned int col = 0; col < num_az(); ++col) {
          //lil image to copy into big image
          vil_image_view<vxl_byte>* lil_img = imgs(row, col);
          for (unsigned int i=0; i<lil_img->ni(); ++i)
            for (unsigned int j=0; j<lil_img->nj(); ++j)
              (*stitched)(ni()*col + i, nj()*row + j) = (*lil_img)(i,j);
        }
      }

      //save as pngv
      std::string big = img() + "/scene-reel.jpeg";
      vil_save( *stitched, big.c_str() );
    }
  }

  return 0;
}
