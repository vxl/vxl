#include <sstream>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <bocl/bocl_cl.h>
#include <vcl_where_root_dir.h>

#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_generic_camera.h>
#include <vpgl/algo/vpgl_camera_convert.h>
#include <vsph/vsph_view_sphere.h>
#include <vsph/vsph_view_point.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>

//vul args
#include <vul/vul_arg.h>
#include <vul/vul_file.h>

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
#include <brip/brip_vil_float_ops.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/file_formats/vil_jpeg.h>
#include <vil/vil_open.h>
// Boxm2_Export_Scene executable will create a small, portable, pre rendered
// scene that can be viewed on many devices.  Currently the output is a folder
// with the following structure
//   * Base directory (dir name given by input -dir)
//      - index.html  (open this file in a browser to see the model
//      - js directory (contains all jQuery and Reel javascript files)
//      - img directory (contains a rendering of stills
//
//   * Pre Rendered Stills
bool vil_save_jpeg(const vil_image_view_base &im,const char* filename)
{
  vil_stream* os = vil_open(filename, "w");
  if (!os || !os->ok()) {
    std::wcerr << __FILE__ ": Invalid stream for \"" << filename << "\"\n";
    return false;
  }
  vil_jpeg_image jimage(os, im.ni(),im.nj(),im.nplanes(), vil_pixel_format_component_format(im.pixel_format()));

  jimage.set_quality(30);

  // Use smart copy constructor to convert multi-component images
  // into multi-plane ones.
  switch (vil_pixel_format_component_format(im.pixel_format()))
  {
  case VIL_PIXEL_FORMAT_BYTE:
    std::cout<<"BYTE"<<std::endl;
    return jimage.put_view(vil_image_view<vxl_byte>(im),0,0);
  case VIL_PIXEL_FORMAT_UINT_16:
    return jimage.put_view(vil_image_view<vxl_uint_16>(im),0,0);
  case VIL_PIXEL_FORMAT_INT_16:
    return jimage.put_view(vil_image_view<vxl_int_16>(im),0,0);
  case VIL_PIXEL_FORMAT_UINT_32:
    return jimage.put_view(vil_image_view<vxl_uint_32>(im),0,0);
  case VIL_PIXEL_FORMAT_INT_32:
    return jimage.put_view(vil_image_view<vxl_int_32>(im),0,0);
#if VXL_HAS_INT_64
  case VIL_PIXEL_FORMAT_UINT_64:
    return jimage.put_view(vil_image_view<vxl_uint_64>(im),0,0);
  case VIL_PIXEL_FORMAT_INT_64:
    return jimage.put_view(vil_image_view<vxl_int_64>(im),0,0);
#endif
  case VIL_PIXEL_FORMAT_FLOAT:
    return jimage.put_view(vil_image_view<float>(im),0,0);
  case VIL_PIXEL_FORMAT_BOOL:
    return jimage.put_view(vil_image_view<bool>(im),0,0);
  case VIL_PIXEL_FORMAT_SBYTE:
    return jimage.put_view(vil_image_view<vxl_sbyte>(im),0,0);
  case VIL_PIXEL_FORMAT_DOUBLE:
    return jimage.put_view(vil_image_view<double>(im),0,0);
  default:
    // In case any one has an odd pixel format that actually works with this file_format.
    return jimage.put_view(im, 0, 0);
  }
}
int main(int argc,  char** argv)
{
    std::cout<<"Boxm2 Hemisphere"<<std::endl;
    vul_arg<std::string> scene_file("-scene", "scene filename", "");
    vul_arg<std::string> dir("-dir", "output image directory", "");
    vul_arg<bool> depth("-depth", "output depth maps", false);
    vul_arg<std::string> imgname("-imgname", "name of the image", "scene");
    vul_arg<unsigned> ni("-ni", "Width of image", 640);
    vul_arg<unsigned> nj("-nj", "Height of image", 480);
    vul_arg<unsigned> num_az("-num_az", "Number of views along azimuth", 36);
    vul_arg<unsigned> num_in("-num_in", "Number of views along 90 degree incline", 5);
    vul_arg<double> incline_0("-init_incline", "Initial angle of incline (degrees)", 45.0);
    vul_arg<double> incline_1("-end_incline", "Angle of incline nearest zenith (degrees)", 15.0);
    vul_arg<double> radius("-radius", "Distance from center of bounding box", 5.0);
    vul_arg<bool> stitch("-stitch", "also save a large, stitched image", false);
    vul_arg<double> gsd("-gsd", "GSD of the central pixel", 0.3);
    vul_arg<unsigned> gpu_idx("-gpu", "Device number", 0);
    vul_arg_parse(argc, argv);

    //////////////////////////////////////////////////////////////////////////////
    //Set Up Directory Structure
    //////////////////////////////////////////////////////////////////////////////
    //see if directory exists
    if ( vul_file::exists(dir()) && vul_file::is_directory(dir()) ) {
        std::cout<<"Directory "<<dir()<<" exists - overwriting it."<<std::endl;
    }
    else {
        vul_file::make_directory_path(dir());
    }
    //see if img folder exists
    std::string imgdir = dir() + "/img/";
    if ( vul_file::exists(imgdir) && vul_file::is_directory(imgdir) ) {
        vul_file::delete_file_glob(imgdir+"*");
    }
    else {
        vul_file::make_directory_path(imgdir);
    }
    //////////////////////////////////////////////////////////////////////////////
    // Now Render Scene Images
    //////////////////////////////////////////////////////////////////////////////
    //create scene
    DECLARE_FUNC_CONS(boxm2_ocl_render_expected_image_process);
    DECLARE_FUNC_CONS(boxm2_ocl_render_expected_color_process);
    DECLARE_FUNC_CONS(boxm2_ocl_render_expected_depth_process);
    REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_render_expected_image_process, "boxm2OclRenderExpectedImageProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_render_expected_depth_process, "boxm2OclRenderExpectedDepthProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_render_expected_color_process, "boxm2OclRenderExpectedColorProcess");
    REGISTER_DATATYPE(boxm2_opencl_cache_sptr);
    REGISTER_DATATYPE(boxm2_scene_sptr);
    REGISTER_DATATYPE(bocl_mem_sptr);
    REGISTER_DATATYPE(vil_image_view_base_sptr);
    //create scene
    boxm2_scene_sptr scene = new boxm2_scene(scene_file());

    //make bocl manager
    bocl_manager_child &mgr =bocl_manager_child::instance();
    if (gpu_idx() >= mgr.gpus_.size()){
      std::cout << "GPU index out of bounds" << std::endl;
      return -1;
    }
    bocl_device_sptr device = mgr.gpus_[gpu_idx()];
    std::cout << "Using: " << *device;

    double gsdofcentralpixel = gsd();
    //create cache, grab singleton instance
    boxm2_lru_cache::create(scene);
    boxm2_opencl_cache_sptr opencl_cache=new boxm2_opencl_cache(device); //allow 4 blocks inthe cache
    brdb_value_sptr brdb_device = new brdb_value_t<bocl_device_sptr>(device);
    brdb_value_sptr brdb_scene = new brdb_value_t<boxm2_scene_sptr>(scene);
    brdb_value_sptr brdb_opencl_cache = new brdb_value_t<boxm2_opencl_cache_sptr>(opencl_cache);
    brdb_value_sptr brdb_ni = new brdb_value_t<unsigned>(ni());
    brdb_value_sptr brdb_nj = new brdb_value_t<unsigned>(nj());
    // FOR  GRID
    //////////////////////////////////////////////////////////////////////////////
    //set up a view sphere, use find closest for closest neighbors
    vsph_view_sphere<vsph_view_point<std::string> > sphere(scene->bounding_box(), scene->bounding_box().depth()*radius());

    //map of ID's that have been rendered
    std::map<int, std::string> saved_imgs;
    vbl_array_2d<std::string> img_grid(num_in(), num_az());

    /////////////////////////////////////////////////////////////////////////////
    //rendered array of views
    std::map<int, vil_image_view<vxl_byte>* > img_map;
    vbl_array_2d<vil_image_view<vxl_byte>* > imgs(num_in(), num_az());
    std::stringstream camstream;
    camstream<<dir()<<"/cams.txt";

    std::ofstream cam_file_stream(camstream.str().c_str());
    // determine increment along azimuth and elevation (incline)
    double az_incr = vnl_math::twopi/num_az();
    double el_incr = 0;
    if (num_in() > 1)
    {
       el_incr = (incline_0() - incline_1()) / (num_in() - 1); //degrees (to include both start and end)
       el_incr = el_incr * vnl_math::pi_over_180;  // radians
    }
    for (unsigned int el_i = 0.0; el_i < num_in(); ++el_i)
    {
        double el = vnl_math::pi_over_180 * incline_0() - el_i * el_incr;
        for (unsigned int az_i = 0; az_i < num_az(); ++az_i)
        {
            double az = 2.0*vnl_math::pi - az_i * az_incr;

            //convert to cartesian (as method is only in cartesian for some reason)
            vsph_sph_point_3d curr_point(radius(), el, az);
            sphere.add_view(curr_point,ni(), nj());
            vgl_point_3d<double> cart_point = sphere.cart_coord(curr_point);
            int uid; double dist;
            vsph_view_point<std::string> view = sphere.find_closest(cart_point, uid, dist);

            //if the viewpoint has already been rendered, skip it
            std::stringstream fstr, idstream;
            fstr<<imgname()<<'_'<<uid<<".jpg";
            img_grid(el_i, az_i) = fstr.str();
            idstream<<imgdir<<imgname()<<'_'<<uid<<".jpg";
            {
                vpgl_camera_double_sptr cam_sptr = view.camera();

                //set focal length and image size for camera
                auto* cam = static_cast<vpgl_perspective_camera<double>* >(cam_sptr.ptr());
                const vpgl_calibration_matrix<double>& mat = cam->get_calibration();
                vgl_vector_3d<double> pp = normalized(cam->principal_axis());
                vgl_vector_3d<double> vdir(cam->get_rotation().as_matrix()(1,0),
                                           cam->get_rotation().as_matrix()(1,1),
                                           cam->get_rotation().as_matrix()(1,2));

                vgl_vector_3d<double> udir = normalized(cross_product(vdir,pp));
                vgl_point_3d<double> cc = cam->camera_center();
                double f = 1.0 / gsdofcentralpixel;
                vbl_array_2d<vgl_ray_3d<double> > rays(nj(),ni());
                cam_file_stream<<uid<<' '<<f<<' '<<cc.x()<<' '<<cc.y()<<' '<<cc.z()<<' '
                               <<pp.x()<<' '<<pp.y()<<' '<<pp.z()<<' '
                               <<udir.x()<<' '<<udir.y()<<' '<<udir.z()<<' '
                               <<vdir.x()<<' '<<vdir.y()<<' '<<vdir.z()<<' ';
                for (double k = 0 ; k < ni(); ++k){
                    for (double l = 0 ; l < nj(); ++l)
                    {
                        vgl_point_3d<double> p = cc + udir*(k - ni()/2)/f + vdir*(l - nj()/2)/f;
                        rays((int)l,(int)k)=vgl_ray_3d<double>(p,pp);
                    }
                }
                auto * gcam = new vpgl_generic_camera<double>(rays);
                brdb_value_sptr brdb_cam = new brdb_value_t<vpgl_camera_double_sptr>(gcam);
                //if scene has RGB data type, use color render process
                bool good;
                if (scene->has_data_type(boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix()) )
                {
                    good = bprb_batch_process_manager::instance()->init_process("boxm2OclRenderExpectedColorProcess");
                    //set process args
                    good = good  && bprb_batch_process_manager::instance()->set_input(0, brdb_device) // device
                        && bprb_batch_process_manager::instance()->set_input(1, brdb_scene)  //  scene
                        && bprb_batch_process_manager::instance()->set_input(2, brdb_opencl_cache)
                        && bprb_batch_process_manager::instance()->set_input(3, brdb_cam)    // camera
                        && bprb_batch_process_manager::instance()->set_input(4, brdb_ni)     // ni for rendered image
                        && bprb_batch_process_manager::instance()->set_input(5, brdb_nj)     // nj for rendered image
                        && bprb_batch_process_manager::instance()->run_process();
                    if( !good ) {
                      std::cout << "ERROR!!: process args input not set: " << __FILE__ << __LINE__ << std::endl;
                      return -1;
                    }

                    unsigned int img_id=0;
                    good = good && bprb_batch_process_manager::instance()->commit_output(0, img_id);
                    if( !good ) {
                      std::cout << "ERROR!!:  commit output failed: " << __FILE__ << __LINE__ << std::endl;
                      return -1;
                    }

                    brdb_query_aptr Q = brdb_query_comp_new("id", brdb_query::EQ, img_id);
                    brdb_selection_sptr S = DATABASE->select("vil_image_view_base_sptr_data", std::move(Q));
                    if (S->size()!=1) {
                        std::cout << "in bprb_batch_process_manager::set_input_from_db(.) - no selections\n";
                    }

                    brdb_value_sptr value;
                    if (!S->get_value(std::string("value"), value)) {
                        std::cout << "in bprb_batch_process_manager::set_input_from_db(.) - didn't get value\n";
                    }
                    vil_image_view_base_sptr outimg=value->val<vil_image_view_base_sptr>();
                    auto* exp_img_out = static_cast<vil_image_view<vil_rgba<vxl_byte> > *>(outimg.ptr());
                    saved_imgs[uid] = idstream.str();

                    vil_image_view<vxl_byte> jpg_out(ni(), nj(),3);
                    for (unsigned int i=0; i<ni(); ++i) {
                        for (unsigned int j=0; j<nj(); ++j) {
                            jpg_out(i,j,0) = (*exp_img_out)(i,j).R();
                            jpg_out(i,j,1) = (*exp_img_out)(i,j).G();
                            jpg_out(i,j,2) = (*exp_img_out)(i,j).B();
                        }
                    }
                    std::stringstream colorstream;
                    colorstream<<imgdir<<"scene_"<<uid<<".jpg";
                    vil_save_jpeg( jpg_out, (colorstream.str().c_str()) );

                }
                if (scene->has_data_type(boxm2_data_traits<BOXM2_MOG3_GREY>::prefix()) )
                {
                    good = bprb_batch_process_manager::instance()->init_process("boxm2OclRenderExpectedImageProcess");
                    //set process args
                    good = good  && bprb_batch_process_manager::instance()->set_input(0, brdb_device) // device
                        && bprb_batch_process_manager::instance()->set_input(1, brdb_scene)  //  scene
                        && bprb_batch_process_manager::instance()->set_input(2, brdb_opencl_cache)
                        && bprb_batch_process_manager::instance()->set_input(3, brdb_cam)    // camera
                        && bprb_batch_process_manager::instance()->set_input(4, brdb_ni)     // ni for rendered image
                        && bprb_batch_process_manager::instance()->set_input(5, brdb_nj)     // nj for rendered image
                        && bprb_batch_process_manager::instance()->run_process();
                    if( !good ) {
                      std::cout << "ERROR!!: process args input not set: " << __FILE__ << __LINE__ << std::endl;
                      return -1;
                    }

                    unsigned int img_id=0;
                    good = good && bprb_batch_process_manager::instance()->commit_output(0, img_id);
                    if( !good ) {
                      std::cout << "ERROR!!: commit output failed: " << __FILE__ << __LINE__ << std::endl;
                      return -1;
                    }

                    brdb_query_aptr Q = brdb_query_comp_new("id", brdb_query::EQ, img_id);
                    brdb_selection_sptr S = DATABASE->select("vil_image_view_base_sptr_data", std::move(Q));
                    if (S->size()!=1) {
                        std::cout << "in bprb_batch_process_manager::set_input_from_db(.) - no selections\n";
                    }

                    brdb_value_sptr value;
                    if (!S->get_value(std::string("value"), value)) {
                        std::cout << "in bprb_batch_process_manager::set_input_from_db(.) - didn't get value\n";
                    }
                    vil_image_view_base_sptr outimg=value->val<vil_image_view_base_sptr>();
                    auto* expimg_view = static_cast<vil_image_view<float>* >(outimg.ptr());
                    auto* byte_img = new vil_image_view<vxl_byte>(ni(), nj());
                    for (unsigned int i=0; i<ni(); ++i)
                        for (unsigned int j=0; j<nj(); ++j)
                            (*byte_img)(i,j) =  (unsigned char)((*expimg_view)(i,j) *255.0f);   //just grab the first byte (all foura r the same)
                    std::stringstream graystream;
                    graystream<<imgdir<<"ir_"<<uid<<".jpg";
                    saved_imgs[uid] = idstream.str();
                     vil_save_jpeg( *byte_img, graystream.str().c_str() );
                    //vil_save(*byte_img, graystream.str().c_str() );
                }
                if (scene->has_data_type(boxm2_data_traits<BOXM2_LABEL_SHORT>::prefix()))
                {
                    good = bprb_batch_process_manager::instance()->init_process("boxm2OclRenderExpectedImageProcess");
                    //set process args
                    good = good  && bprb_batch_process_manager::instance()->set_input(0, brdb_device) // device
                        && bprb_batch_process_manager::instance()->set_input(1, brdb_scene)  //  scene
                        && bprb_batch_process_manager::instance()->set_input(2, brdb_opencl_cache)
                        && bprb_batch_process_manager::instance()->set_input(3, brdb_cam)    // camera
                        && bprb_batch_process_manager::instance()->set_input(4, brdb_ni)     // ni for rendered image
                        && bprb_batch_process_manager::instance()->set_input(5, brdb_nj)     // nj for rendered image
                        && bprb_batch_process_manager::instance()->run_process();
                    if( !good ) {
                      std::cout << "ERROR!!: process args input not set: " << __FILE__ << __LINE__ << std::endl;
                      return -1;
                    }

                    unsigned int img_id=0;
                    good = good && bprb_batch_process_manager::instance()->commit_output(0, img_id);
                    if( !good ) {
                      std::cout << "ERROR!!: commit output failed: " << __FILE__ << __LINE__ << std::endl;
                      return -1;
                    }

                    brdb_query_aptr Q = brdb_query_comp_new("id", brdb_query::EQ, img_id);
                    brdb_selection_sptr S = DATABASE->select("vil_image_view_base_sptr_data", std::move(Q));
                    if (S->size()!=1) {
                        std::cout << "in bprb_batch_process_manager::set_input_from_db(.) - no selections\n";
                    }
                    brdb_value_sptr value;
                    if (!S->get_value(std::string("value"), value)) {
                        std::cout << "in bprb_batch_process_manager::set_input_from_db(.) - didn't get value\n";
                    }
                    vil_image_view_base_sptr outimg=value->val<vil_image_view_base_sptr>();
                    auto* expimg_view = static_cast<vil_image_view<float>* >(outimg.ptr());
                    auto* byte_img = new vil_image_view<vxl_byte>(ni(), nj());
                    for (unsigned int i=0; i<ni(); ++i)
                        for (unsigned int j=0; j<nj(); ++j)
                            (*byte_img)(i,j) =  (unsigned char)(std::min((*expimg_view)(i,j),255.0f) );   //just grab the first byte (all foura r the same)
                    std::stringstream depthstream;
                    depthstream<<imgdir<<"label_"<<uid<<".jpg";
                    vil_save(*byte_img, depthstream.str().c_str() );
                }
                if (depth())
                {
                    unsigned int img_id=0;
                    good = bprb_batch_process_manager::instance()->init_process("boxm2OclRenderExpectedDepthProcess")
                        && bprb_batch_process_manager::instance()->set_input(0, brdb_device) // device
                        && bprb_batch_process_manager::instance()->set_input(1, brdb_scene)  //  scene
                        && bprb_batch_process_manager::instance()->set_input(2, brdb_opencl_cache)
                        && bprb_batch_process_manager::instance()->set_input(3, brdb_cam)    // camera
                        && bprb_batch_process_manager::instance()->set_input(4, brdb_ni)     // ni for rendered image
                        && bprb_batch_process_manager::instance()->set_input(5, brdb_nj)     // nj for rendered image
                        && bprb_batch_process_manager::instance()->run_process()
                        && bprb_batch_process_manager::instance()->commit_output(0, img_id);
                    if( !good ) {
                      std::cout << "ERROR!!: process args input not set: " << __FILE__ << __LINE__ << std::endl;
                      return -1;
                    }

                    brdb_query_aptr Q = brdb_query_comp_new("id", brdb_query::EQ, img_id);
                    brdb_selection_sptr S = DATABASE->select("vil_image_view_base_sptr_data", std::move(Q));
                    if (S->size()!=1) {
                        std::cout << "in bprb_batch_process_manager::set_input_from_db(.) - no selections\n";
                    }

                    brdb_value_sptr value;
                    if (!S->get_value(std::string("value"), value)) {
                        std::cout << "in bprb_batch_process_manager::set_input_from_db(.) - didn't get value\n";
                    }
                    vil_image_view_base_sptr depthimg=value->val<vil_image_view_base_sptr>();
                    auto* depthimg_view = static_cast<vil_image_view<float>* >(depthimg.ptr());
                    float vmin=0, vmax = 0;
                    vil_math_value_range<  float>(*depthimg_view, vmin, vmax);
                    vil_image_view<vxl_byte> byte_img = brip_vil_float_ops::convert_to_byte(*depthimg_view);
                    cam_file_stream<<vmin<<' '<<vmax<<'\n';
                    std::stringstream depthstream;
                    depthstream<<imgdir<<"depth_"<<uid<<".jpg";
                    vil_save_jpeg( byte_img, depthstream.str().c_str() );
                    //vil_save( byte_img, depthstream.str().c_str() );
                }
            }
        }
    }
    cam_file_stream.close();
    // if stitch is specified, also save a big image
    if (stitch())
    {
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
        std::string big = dir() + "/scene-reel.jpg";
        vil_save( *stitched, big.c_str() );
    }
    return 0;
}
