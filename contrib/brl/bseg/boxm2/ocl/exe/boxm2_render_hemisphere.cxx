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
  vul_arg<unsigned> ni("-ni", "Width of image", 640);
  vul_arg<unsigned> nj("-nj", "Height of image", 480);
  vul_arg<unsigned> num_az("-num_az", "Number of views along azimuth", 36);
  vul_arg<unsigned> num_in("-num_in", "Number of views along 90 degree incline", 5);
  vul_arg<double> radius("-radius", "Distance from center of bounding box", 5.0);
  vul_arg<bool>   grid("-grid", "Specify grid if saving images in row/col format", false);
  vul_arg<bool>   stitch("-stitch", "stitches the images together into one large image", false);
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
  if ( !grid() )
  {
    vcl_vector<vil_image_view<vxl_byte>* > images;

    vsph_view_sphere<vsph_view_point<vcl_string> > sphere(scene->bounding_box(), radius());
    sphere.add_uniform_views(vnl_math::pi/3, vnl_math::pi/18.0, ni(), nj());
    vcl_cout<<"Number of views to render: "<<sphere.size()<<vcl_endl;

    vsph_view_sphere<vsph_view_point<vcl_string> >::iterator iter; int img_index=0;
    for (iter = sphere.begin(); iter != sphere.end(); ++iter, ++img_index)
    {
      vcl_cout<<"view: [id "<<iter->first<<']'<<vcl_endl;

      vsph_view_point<vcl_string>& view = iter->second;
      vpgl_camera_double_sptr cam_sptr = view.camera();
      vpgl_perspective_camera<double>* cam = static_cast<vpgl_perspective_camera<double>* >(cam_sptr.ptr());
      brdb_value_sptr brdb_cam = new brdb_value_t<vpgl_camera_double_sptr>(cam_sptr);
      vpgl_calibration_matrix<double> mat = cam->get_calibration();
      mat.set_focal_length(mat.focal_length()/2);
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
      vcl_stringstream stream, tfstream, jpgstream;
      stream<<img()<<"/pimg_"<<img_index<<".png";
      tfstream<<img()<<"/timg_"<<img_index<<".tiff";
      jpgstream<<img()<<"/jimg_"<<img_index<<".jpeg";
      vcl_string* filepath = new vcl_string();
      (*filepath) = stream.str();
      view.set_metadata(filepath);
      vil_save( *byte_img, stream.str().c_str() );

      //save as tiff
      vil_save( *byte_img, tfstream.str().c_str() );

      //save as jpeg
      vil_save( *byte_img, jpgstream.str().c_str() );
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
  }

  //////////////////////////////////////////////////////////////////////////////
  // Else IF GRID
  //////////////////////////////////////////////////////////////////////////////
  else
  {
    //set up a view sphere, use find closest for closest neighbors
    vsph_view_sphere<vsph_view_point<vcl_string> > sphere(scene->bounding_box(), radius());
    sphere.add_uniform_views(vnl_math::pi/3, vnl_math::pi/18.0, ni(), nj());
    vcl_cout<<"Number of views to render: "<<sphere.size()<<vcl_endl;

    //rendered array of views
    vbl_array_2d<vil_image_view<vxl_byte>* > imgs(num_in(), num_az());
    double az_incr = 2.0*vnl_math::pi/num_az();
    double el_incr = vnl_math::pi/2.0/num_in();
    for (unsigned int az_i = 0; az_i < num_az(); ++az_i)
    {
      double az = 2.0*vnl_math::pi - az_i * az_incr;
      for (unsigned int el_i = 0.0; el_i < num_in(); ++el_i)
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

        //construct camera
        //vpgl_perspective_camera<double>* cam
            //= boxm2_util::construct_camera(el, az, radius(), ni(), nj(), scene->bounding_box());
        //vpgl_calibration_matrix<double> mat = cam->get_calibration();
        //mat.set_focal_length(mat.focal_length()*5.0);
        //cam->set_calibration(mat);
        //vpgl_camera_double_sptr cam_sptr = cam;

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
        jpgstream<<img()<<"/jimg_"<<el_i<<"_"<<az_i<<".jpeg";
        vil_save( *byte_img, jpgstream.str().c_str() );

        ////save as png
        //pngstream<<img()<<"/pimg_"<<el_i<<"_"<<az_i<<".png";
        //vil_save( *byte_img, pngstream.str().c_str() );

        //and store for whatever reason
        imgs(el_i, az_i) = byte_img;
      }
    }

    //if stitch is specified, also save a big image
    if (stitch()) {
        //construct a humungous image
        vil_image_view<vxl_byte>* stitched = new vil_image_view<vxl_byte>(ni() * num_az(), nj() * num_in());
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
        vcl_string big = img() + "/scene-reel.jpeg";
        vil_save( *stitched, big.c_str() );
    }
  }

  return 0;
}

