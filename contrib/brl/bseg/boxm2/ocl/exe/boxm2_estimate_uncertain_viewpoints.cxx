#include <sstream>
#include <iostream>
#include <fstream>
#include <bocl/bocl_cl.h>
#include <vcl_where_root_dir.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vpgl/vpgl_perspective_camera.h>
#include <vsph/vsph_view_sphere.h>
#include <vsph/vsph_view_point.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>
#include <vnl/vnl_math.h>

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
#include <vpgl/algo/vpgl_camera_convert.h>
#include <brip/brip_vil_float_ops.h>
#include <vil/vil_math.h>

// Boxm2_Export_Scene executable will create a small, portable, pre rendered
// scene that can be viewed on many devices.  Currently the output is a folder
// with the following structure
//   * Base directory (dir name given by input -dir)
//      - index.html  (open this file in a browser to see the model
//      - js directory (contains all jQuery and Reel javascript files)
//      - img directory (contains a rendering of stills
//
//   * Pre Rendered Stills
int closest_camera(vgl_point_3d<double> p, std::vector<vpgl_perspective_camera<double> *> cams)
{
    double min_distance = 1e20;
    int min_cam_index = -1;
    for (unsigned i = 0 ; i < cams.size(); ++i)
    {
        vgl_vector_3d<double> diff =( cams[i]->get_camera_center() - p );
        double d = diff.length();
        if (d < min_distance)
        {
            min_distance = d;
            min_cam_index = i;
        }
    }
    return min_cam_index;
}

bool compare_second_element(const std::pair<vgl_point_3d<double>, double> &a,
                            const std::pair<vgl_point_3d<double>, double> &b)
{
    return a.second > b.second;
}

int main(int argc,  char** argv)
{
    std::cout<<"Boxm2 Uncertain Viewponts"<<std::endl;
    vul_arg<std::string> scene_file("-scene", "scene filename", "");
    vul_arg<std::string> dir("-dir", "output image directory", "");
    vul_arg<unsigned> ni("-ni", "Width of image", 1024);
    vul_arg<unsigned> nj("-nj", "Height of image", 768);
    vul_arg<unsigned> num_az("-num_az", "Number of views along azimuth", 36);
    vul_arg<unsigned> num_in("-num_in", "Number of views along 90 degree incline", 5);
    vul_arg<double> incline_0("-init_incline", "Initial angle of incline (degrees)", 60.0);
    vul_arg<double> incline_1("-end_incline", "Angle of incline nearest zenith (degrees)", 15.0);
    vul_arg<double> radius("-radius", "Distance from center of bounding box", 11.12);

    vul_arg<std::string> outfile("-out", "store uncertainties", "");
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
#ifdef DEBUG
        std::cout<<"Couldn't make directory at "<<dir()<<std::endl;
        return -1;
#endif
    }

    //see if img folder exists
    std::string imgdir = dir() + "/img/";
    if ( vul_file::exists(imgdir) && vul_file::is_directory(imgdir) ) {
        vul_file::delete_file_glob(imgdir+"*");
    }
    else {
        vul_file::make_directory_path(imgdir);
#ifdef DEBUG
        std::cout<<"Couldn't make img directory at "<<dir()<<std::endl;
        return -1;
#endif
    }


    //////////////////////////////////////////////////////////////////////////////
    // Now Render Scene Images
    //////////////////////////////////////////////////////////////////////////////
    //create scene
    DECLARE_FUNC_CONS(boxm2_ocl_render_expected_image_process);
    REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_render_expected_image_process, "boxm2OclRenderExpectedImageProcess");

    REGISTER_DATATYPE(boxm2_opencl_cache_sptr);
    REGISTER_DATATYPE(boxm2_scene_sptr);
    REGISTER_DATATYPE(bocl_mem_sptr);
    REGISTER_DATATYPE(vil_image_view_base_sptr);

    //create scene
    boxm2_scene_sptr scene = new boxm2_scene(scene_file());
    bocl_manager_child &mgr =bocl_manager_child::instance();
    bocl_device_sptr device = mgr.gpus_[2];

    //create cache, grab singleton instance
    boxm2_lru_cache::create(scene);
    boxm2_opencl_cache_sptr opencl_cache=new boxm2_opencl_cache(device); //allow 4 blocks inthe cache
    brdb_value_sptr brdb_device = new brdb_value_t<bocl_device_sptr>(device);
    brdb_value_sptr brdb_scene = new brdb_value_t<boxm2_scene_sptr>(scene);
    brdb_value_sptr brdb_opencl_cache = new brdb_value_t<boxm2_opencl_cache_sptr>(opencl_cache);
    brdb_value_sptr brdb_ni = new brdb_value_t<unsigned>(ni());
    brdb_value_sptr brdb_nj = new brdb_value_t<unsigned>(nj());
    brdb_value_sptr brdb_ident = new brdb_value_t<std::string>("cubic_model");

    // FOR  GRID
    //////////////////////////////////////////////////////////////////////////////
    //set up a view sphere, use find closest for closest neighbors
    vsph_view_sphere<vsph_view_point<std::string> > sphere(vgl_point_3d<double>(0,0,-0.28), radius());

    //map of ID's that have been rendered
    std::map<int, std::string> saved_imgs;

    /////////////////////////////////////////////////////////////////////////////
    //rendered array of views
    std::map<int, vil_image_view<vxl_byte>* > img_map;
    vbl_array_2d<vil_image_view<vxl_byte>* > imgs(num_in(), num_az());

    typedef std::pair<vgl_point_3d<double>,double> ptdistpair;
    std::vector<ptdistpair > distances ;
    // determine increment along azimuth and elevation (incline)
    double az_incr = 2.0*vnl_math::pi/num_az();
    double el_incr = (incline_0() - incline_1()) / (num_in()-1); //degrees (to include both start and end)
    el_incr = el_incr * vnl_math::pi_over_180;  // radians
    for (unsigned int el_i = 0.0; el_i < num_in(); ++el_i)
    {
        double el = vnl_math::pi_over_180 * incline_0() - el_i * el_incr;
        for (unsigned int az_i = 0; az_i < num_az(); ++az_i)
        {
            double az = 2.0*vnl_math::pi - az_i * az_incr;

            float mean = 0.0;
            //convert to cartesian (as method is only in cartesian for some reason)
            vsph_sph_point_3d curr_point(radius(), el, az);
            sphere.add_view(curr_point,ni(), nj());
            vgl_point_3d<double> cart_point = sphere.cart_coord(curr_point);
            int uid; double dist;
            vsph_view_point<std::string> view = sphere.find_closest(cart_point, uid, dist);
            std::stringstream     idstream;
            idstream<<imgdir<<"uncertainty_"<<el_i<<'_'<<az_i<<".tiff";

            if ( saved_imgs.find(uid) == saved_imgs.end() )
            {
                vpgl_camera_double_sptr cam_sptr = view.camera();
                //set focal length and image size for camera
                auto* cam = static_cast<vpgl_perspective_camera<double>* >(cam_sptr.ptr());
                vpgl_calibration_matrix<double> mat = cam->get_calibration();
                brdb_value_sptr brdb_cam = new brdb_value_t<vpgl_camera_double_sptr>(cam_sptr);
                mat.set_focal_length(5270.5f);
                cam->set_calibration(mat);
                std::cout<<"Focal Length "<<mat.focal_length()<<std::endl;
                //if scene has RGB data type, use color render process
                bool good;
                if (scene->has_data_type(boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix()) ) {
                  good = bprb_batch_process_manager::instance()->init_process("boxm2OclRenderExpectedColorProcess");
                }
                else {
                  good = bprb_batch_process_manager::instance()->init_process("boxm2OclRenderExpectedImageProcess");
                }
                //set process args
                good = good
                    && bprb_batch_process_manager::instance()->set_input(0, brdb_device)        // device
                    && bprb_batch_process_manager::instance()->set_input(1, brdb_scene)            // scene
                    && bprb_batch_process_manager::instance()->set_input(2, brdb_opencl_cache)    // opencl cache
                    && bprb_batch_process_manager::instance()->set_input(3, brdb_cam)            // camera
                    && bprb_batch_process_manager::instance()->set_input(4, brdb_ni)            // ni for rendered image
                    && bprb_batch_process_manager::instance()->set_input(5, brdb_nj)            // nj for rendered image
                    && bprb_batch_process_manager::instance()->set_input(6, brdb_ident)            // identifier for rendered image
                    && bprb_batch_process_manager::instance()->run_process();
                if( !good ) {
                  std::cout << "ERROR!!: process args not set: " << __FILE__ << __LINE__ << std::endl;
                  return -1;
                }
                unsigned int img_id=0;
                good = good && bprb_batch_process_manager::instance()->commit_output(0, img_id);
                if( !good ) {
                  std::cout << "ERROR!!: commit_output failed: " << __FILE__ << __LINE__ << std::endl;
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
                vil_math_mean<float,float>(mean, *expimg_view,0);
                std::cout<<el<<' '<<az<<' '<<mean<<std::endl;
                distances.emplace_back(cart_point,mean);
                saved_imgs[uid] = idstream.str();
                vil_save(*expimg_view, idstream.str().c_str() );
            }
        }
    }
    std::ofstream ofile(outfile().c_str());
    std::sort(distances.begin(), distances.end(), compare_second_element);

    for (auto & distance : distances)
        ofile<<distance.first.x()<<' '<<distance.first.y()<<' '<<distance.first.z()<<' '<<distance.second<<std::endl;
    return 0;
}
