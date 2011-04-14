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
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <bocl/bocl_manager.h>

#include <bpro/core/vil_pro/vil_register.h>
#include <vil/vil_image_view_base.h>
#include <brdb/brdb_value.h>
#include <brdb/brdb_selection.h>

#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_parameters.h>
#include <bprb/bprb_macros.h>
#include <bprb/bprb_func_process.h>
//brdb stuff
#include <brdb/brdb_value.h>

// Boxm2_Export_Scene executable will create a small, portable, pre rendered
// scene that can be viewed on many devices.  Currently the output is a folder
// with the following structure
//   * Base directory (dir name given by input -dir)
//      - index.html  (open this file in a browser to see the model
//      - js directory (contains all jQuery and Reel javascript files)
//      - img directory (contains a rendering of stills
//
//   * Pre Rendered Stills

int main(int argc,  char** argv)
{
    vcl_cout<<"Boxm2 Hemisphere"<<vcl_endl;
    vul_arg<vcl_string> scene_file("-scene", "scene filename", "");
    vul_arg<vcl_string> dir("-dir", "output image directory", "");
    vul_arg<unsigned> ni("-ni", "Width of image", 640);
    vul_arg<unsigned> nj("-nj", "Height of image", 480);
    vul_arg<unsigned> num_az("-num_az", "Number of views along azimuth", 36);
    vul_arg<unsigned> num_in("-num_in", "Number of views along 90 degree incline", 3);
    vul_arg<double> incline_0("-init_incline", "Initial angle of incline (degrees)", 45.0); 
    vul_arg<double> incline_1("-end_incline", "Angle of incline nearest zenith (degrees)", 15.0); 
    vul_arg<double> radius("-radius", "Distance from center of bounding box", 5.0);
    vul_arg<bool> stitch("-stitch", "also save a large, stitched image", false); 
    vul_arg_parse(argc, argv);

    //////////////////////////////////////////////////////////////////////////////
    //Set Up Directory Structure
    //////////////////////////////////////////////////////////////////////////////
    //see if directory exists
    if ( vul_file::exists(dir()) && vul_file::is_directory(dir()) ) {
        vcl_cout<<"Directory "<<dir()<<" exists - overwriting it."<<vcl_endl;
    }
    else {
        vul_file::make_directory_path(dir());
#ifdef DEBUG
        vcl_cout<<"Couldn't make directory at "<<dir()<<vcl_endl;
        return -1;
#endif
    }

    //see if img folder exists
    vcl_string imgdir = dir() + "/img/";
    if ( vul_file::exists(imgdir) && vul_file::is_directory(imgdir) ){
        vul_file::delete_file_glob(imgdir+"*");
    }
    else {
        vul_file::make_directory_path(imgdir);
#ifdef DEBUG
        vcl_cout<<"Couldn't make img directory at "<<dir()<<vcl_endl;
        return -1;
#endif
    }

    //see if JS folder exists
    vcl_string jsdir = dir() + "/js/";
    if ( vul_file::exists(jsdir) && vul_file::is_directory(jsdir) ){
        vul_file::delete_file_glob(jsdir+"*");
    }
    else {
        vul_file::make_directory_path(jsdir);
#ifdef DEBUG
        vcl_cout<<"Couldn't make js directory at "<<dir()<<vcl_endl;
        return -1;
#endif
    }

    //see if CSS folder exists
    vcl_string cssdir = dir() + "/css/";
    if ( vul_file::exists(cssdir) && vul_file::is_directory(cssdir) ){
        vul_file::delete_file_glob(cssdir+"*");
    }
    else {
        vul_file::make_directory_path(cssdir);
#ifdef DEBUG
        vcl_cout<<"Couldn't make css directory at "<<dir()<<vcl_endl;
        return -1;
#endif
    }

    //copy JS files into JS folder
    vcl_string aux_dir = vcl_string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/boxm2/ocl/exe/auxiliary/";
    vcl_vector<vcl_string> js_files;
    js_files.push_back("/js/jquery.min.js");
    js_files.push_back("/js/jquery.cookie-min.js");
    js_files.push_back("/js/jquery.disabletextselect-min.js");
    js_files.push_back("/js/jquery.mousewheel-min.js");
    js_files.push_back("/js/jquery.reel-min.js");
    js_files.push_back("/js/miniZoomPan.js");
    //copy files to dir() + js
    for (unsigned int i=0; i<js_files.size(); ++i)
        boxm2_util::copy_file(aux_dir + js_files[i], dir() + js_files[i]);

    //copy CSS file to css folder
    boxm2_util::copy_file(aux_dir + "/css/miniZoomPan.css", dir() + "/css/miniZoomPan.css"); 

    //////////////////////////////////////////////////////////////////////////////
    // Now Render Scene Images
    //////////////////////////////////////////////////////////////////////////////
    //create scene
    DECLARE_FUNC_CONS(boxm2_ocl_render_expected_image_process);
    DECLARE_FUNC_CONS(boxm2_ocl_render_expected_color_process);
    REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_render_expected_image_process, "boxm2OclRenderExpectedImageProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_render_expected_color_process, "boxm2OclRenderExpectedColorProcess");

    REGISTER_DATATYPE(boxm2_opencl_cache_sptr);
    REGISTER_DATATYPE(boxm2_scene_sptr);
    REGISTER_DATATYPE(bocl_mem_sptr);
   // vil_register::register_datatype();
    REGISTER_DATATYPE(vil_image_view_base_sptr);

    //create scene
    boxm2_scene_sptr scene = new boxm2_scene(scene_file());

    bocl_manager_child_sptr mgr =bocl_manager_child::instance();
    bocl_device_sptr device = mgr->gpus_[0];


    //create cache, grab singleton instance
    boxm2_lru_cache::create(scene);
    boxm2_opencl_cache_sptr opencl_cache=new boxm2_opencl_cache(scene, device, 1); //allow 4 blocks inthe cache

    brdb_value_sptr brdb_device = new brdb_value_t<bocl_device_sptr>(device);
    brdb_value_sptr brdb_scene = new brdb_value_t<boxm2_scene_sptr>(scene);
    brdb_value_sptr brdb_opencl_cache = new brdb_value_t<boxm2_opencl_cache_sptr>(opencl_cache);
    brdb_value_sptr brdb_ni = new brdb_value_t<unsigned>(ni());
    brdb_value_sptr brdb_nj = new brdb_value_t<unsigned>(nj());
    // FOR  GRID
    //////////////////////////////////////////////////////////////////////////////
    //set up a view sphere, use find closest for closest neighbors
    vsph_view_sphere<vsph_view_point<vcl_string> > sphere(scene->bounding_box(), radius());
        

    // Uncomment the below for non-grid representation
#if 0
    sphere.add_uniform_views(vnl_math::pi/2, vnl_math::pi/18.0, ni(), nj());
    vcl_cout<<"Number of views on sphere: "<<sphere.size()<<vcl_endl;
#endif
    //map of ID's that have been rendered
    vcl_map<int, vcl_string> saved_imgs; 
    vbl_array_2d<vcl_string> img_grid(num_in(), num_az()); 

    /////////////////////////////////////////////////////////////////////////////
    //rendered array of views
    vcl_map<int, vil_image_view<vxl_byte>* > img_map; 
    vbl_array_2d<vil_image_view<vxl_byte>* > imgs(num_in(), num_az());

    // determine increment along azimuth and elevation (incline)
    double az_incr = 2.0*vnl_math::pi/num_az();
    double el_incr = (incline_0() - incline_1()) / (num_in()-1); //degrees (to include both start and end)
    el_incr = (el_incr/360.0) * 2.0 * vnl_math::pi;  // radians
    for (unsigned int az_i = 0; az_i < num_az(); ++az_i)
    {
        double az = 2.0*vnl_math::pi - az_i * az_incr;
        for (unsigned int el_i = 0.0; el_i < num_in(); ++el_i)
        {
            double el = (2.0*vnl_math::pi) * (incline_0()/360.0) - el_i * el_incr;

            //convert to cartesian (as method is only in cartesian for some reason)
            vsph_sph_point_3d curr_point(radius(), el, az);
            sphere.add_view(curr_point,ni(), nj());
            vgl_point_3d<double> cart_point = sphere.cart_coord(curr_point);
            int uid; double dist;
            vsph_view_point<vcl_string> view = sphere.find_closest(cart_point, uid, dist);

            //if the viewpoint has already been rendered, skip it
            vcl_stringstream fstr, idstream;
            fstr<<"scene_"<<uid<<".jpg"; 
            img_grid(el_i, az_i) = fstr.str(); 
            idstream<<imgdir<<"scene_"<<uid<<".jpg"; 
            if( saved_imgs.find(uid) == saved_imgs.end() )
            {
                vpgl_camera_double_sptr cam_sptr = view.camera();
                brdb_value_sptr brdb_cam = new brdb_value_t<vpgl_camera_double_sptr>(cam_sptr);

                //set focal length and image size for camera
                vpgl_perspective_camera<double>* cam = static_cast<vpgl_perspective_camera<double>* >(cam_sptr.ptr());
                vpgl_calibration_matrix<double> mat = cam->get_calibration();
                mat.set_focal_length(mat.focal_length());
                cam->set_calibration(mat);

                //if scene has RGB data type, use color render process
                bool good = true; 
                if(scene->has_data_type(boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix()) )
                    good = bprb_batch_process_manager::instance()->init_process("boxm2OclRenderExpectedColorProcess");
                else
                    good = bprb_batch_process_manager::instance()->init_process("boxm2OclRenderExpectedImageProcess");

                //set process args
                good = good && bprb_batch_process_manager::instance()->set_input(0, brdb_device); // device
                good = good && bprb_batch_process_manager::instance()->set_input(1, brdb_scene); //  scene 
                good = good && bprb_batch_process_manager::instance()->set_input(2, brdb_opencl_cache); 
                good = good && bprb_batch_process_manager::instance()->set_input(3, brdb_cam);// camera
                good = good && bprb_batch_process_manager::instance()->set_input(4, brdb_ni);  // ni for rendered image
                good = good && bprb_batch_process_manager::instance()->set_input(5, brdb_nj);   // nj for rendered image
                good = good && bprb_batch_process_manager::instance()->run_process();

                unsigned int img_id=0;
                good = good && bprb_batch_process_manager::instance()->commit_output(0, img_id);
                brdb_query_aptr Q = brdb_query_comp_new("id", brdb_query::EQ, img_id);
                brdb_selection_sptr S = DATABASE->select("vil_image_view_base_sptr_data", Q);
                if (S->size()!=1){
                    vcl_cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
                        << " no selections\n";
                }

                brdb_value_sptr value;
                if (!S->get_value(vcl_string("value"), value)) {
                    vcl_cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
                        << " didn't get value\n";
                }

                vil_image_view_base_sptr outimg=value->val<vil_image_view_base_sptr>();
                
                if(scene->has_data_type(boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix()) ) 
                {
                  vil_image_view<vil_rgba<vxl_byte> >* exp_img_out = static_cast<vil_image_view<vil_rgba<vxl_byte> > *>(outimg.ptr()); 
                  saved_imgs[uid] = idstream.str(); 

                  vil_image_view<vxl_byte> jpg_out(ni(), nj(),3); 
                  for (unsigned int i=0; i<ni(); ++i) {
                    for (unsigned int j=0; j<nj(); ++j) {
                      jpg_out(i,j,0) = (*exp_img_out)(i,j).R(); 
                      jpg_out(i,j,1) = (*exp_img_out)(i,j).G(); 
                      jpg_out(i,j,2) = (*exp_img_out)(i,j).B(); 
                    }
                  }
                  vil_save( jpg_out, idstream.str().c_str() );

                  //and store for whatever reason
                  //imgs(el_i, az_i) = exp_img_out;
                }
                else 
                {
                  vil_image_view<float>* expimg_view = static_cast<vil_image_view<float>* >(outimg.ptr());
                  vil_image_view<vxl_byte>* byte_img = new vil_image_view<vxl_byte>(ni(), nj());
                  for (unsigned int i=0; i<ni(); ++i)
                      for (unsigned int j=0; j<nj(); ++j)
                          (*byte_img)(i,j) =  (unsigned char)((*expimg_view)(i,j) *255.0f);   //just grab the first byte (all foura r the same)

                  saved_imgs[uid] = idstream.str(); 
                  vil_save( *byte_img, idstream.str().c_str() );

                  //and store for whatever reason
                  imgs(el_i, az_i) = byte_img;
                }
            }
        }
    }

    //need to generate a JS.JS file that lists an array of these images
    vcl_cout<<"Rows: "<<num_in()<<" cols: "<<num_az()<<vcl_endl;
    boxm2_util::generate_jsfunc(img_grid, dir() + "/js/js.js"); 
    boxm2_util::generate_html(nj(),ni(), num_in(), num_az(),  dir() + "/index.html"); 

    // if stitch is specified, also save a big image
    if (stitch())
    {
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
        vcl_string big = dir() + "/scene-reel.jpg";
        vil_save( *stitched, big.c_str() );
    }

    return 0;
}

